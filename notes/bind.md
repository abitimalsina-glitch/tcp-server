# `bind()`

## 1. What It Is

`bind()` is a system call that assigns a specific local network address (an IP address and a port number) to an unnamed socket.

---

## 2. Why It Exists

When you create a socket with `socket()`, the kernel creates the socket object, but the socket is completely **unnamed**. It has no IP address and no port assigned to it.

While a client can let the operating system automatically choose an arbitrary temporary (ephemeral) port, a **server must have a fixed, well-known port**. If a server did not bind to a specific port, clients would have no idea which port to connect to.

`bind()` attaches the server socket to a known IP and port.

---

## 3. How It Works

`bind()` registers your socket with the operating system's network routing table:

```text
Incoming Network Traffic:
Packet destined for 127.0.0.1:8080
             |
             v
   Operating System Kernel
   (Checks binding table)
   "127.0.0.1:8080 belongs to sockfd 3"
             |
             v
         sockfd (3)
```

After binding, the kernel knows that any incoming TCP segments addressed to that local IP and port belong to this specific socket descriptor.

---

## 4. Syntax

Declared in `<sys/socket.h>`:

```c
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

---

## 5. Important Arguments

* **`sockfd`**: The socket file descriptor returned by `socket()`.
* **`addr`**: A pointer to the populated `struct sockaddr_in`, cast to the generic `(struct sockaddr *)` pointer.
* **`addrlen`**: The size in bytes of the address structure being pointed to, typically `sizeof(server_address)`.

---

## 6. Return Value

* **`0`**: Success. The socket is successfully bound to the specified IP address and port.
* **`-1`**: Failure. The socket was not bound, and `errno` is set to identify the error.

---

## 7. Error Handling

Common reasons `bind()` fails:

1. **`EADDRINUSE` (Address already in use):**
   Another process is already bound to this port, or a previous instance of your server recently closed and the port is temporarily held in the TCP `TIME_WAIT` state.
2. **`EACCES` (Permission denied):**
   Ports `1` through `1023` are privileged system ports. Binding to them requires superuser (root) privileges.
3. **`EADDRNOTAVAIL` (Address not available):**
   The IP address specified in `addr` does not belong to any local network interface on this machine.

Always check for failure and close the socket to clean up resources:

```c
if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    perror("bind");
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

int main(void)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    /* Bind the socket to 127.0.0.1:8080 */
    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Successfully bound to 127.0.0.1:8080\n");

    close(socket_fd);
    return 0;
}
```
