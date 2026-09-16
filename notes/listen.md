# `listen()`

## 1. What It Is

`listen()` is a system call that marks a bound TCP socket as a **passive socket**—a socket that will be used to accept incoming connection requests from clients.

---

## 2. Why It Exists

By default, any socket created with `socket()` is an **active socket**. Active sockets are designed to initiate outgoing connections (like a client connecting to a remote server).

A server program operates in reverse: it does not initiate connections; it waits for clients to connect to it. `listen()` instructs the operating system kernel to convert the socket from an active state into a passive, listening state.

---

## 3. How It Works

When a socket enters the listening state:

1. The OS kernel sets the internal TCP state machine for this socket to `LISTEN`.
2. The kernel sets up a queue for incoming connection requests.
3. When remote clients perform the TCP 3-way handshake (SYN, SYN-ACK, ACK), the operating system handles the handshake in the background.
4. Fully established connections are placed into a **pending connection queue** until your program retrieves them using `accept()`.

```text
Incoming Client SYN
        |
        v
+---------------------------------------------------+
| OS Kernel TCP Stack (Handles 3-Way Handshake)    |
+---------------------------------------------------+
        |
        v
Pending Connection Queue (Backlog)
[ Client C ] -> [ Client B ] -> [ Client A ]
        |
        v (dequeued by accept())
Your Application
```

---

## 4. Syntax

Declared in `<sys/socket.h>`:

```c
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

---

## 5. Important Arguments

* **`sockfd`**: The bound socket file descriptor to place into the listening state.
* **`backlog`**: The maximum number of pending connections that the operating system will hold in the queue before refusing or ignoring new incoming connections.
  * For example, a backlog of `5` means up to 5 fully established connections can wait in line to be accepted by `accept()`.

---

## 6. Return Value

* **`0`**: Success. The socket is now in listening mode.
* **`-1`**: Failure. The socket could not be put into listening mode, and `errno` is set.

---

## 7. Error Handling

Common reasons `listen()` fails:
* **`EBADF`**: `sockfd` is not a valid open file descriptor.
* **`EDESTADDRREQ`**: The socket is not bound to a local address (you forgot to call `bind()`).
* **`EOPNOTSUPP`**: The socket type does not support listening (for example, attempting to call `listen()` on a UDP datagram socket).

Handle failures by checking for `< 0`, printing the error, closing the socket, and exiting:

```c
if (listen(socket_fd, BACKLOG) < 0) {
    perror("listen");
    close(socket_fd);
    exit(EXIT_FAILURE);
}
```

---

## 8. Small Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BACKLOG 5

int main(void)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    /* Transition socket to listening state */
    if (listen(socket_fd, BACKLOG) < 0) {
        perror("listen");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening with backlog queue of %d\n", BACKLOG);

    close(socket_fd);
    return 0;
}
```
