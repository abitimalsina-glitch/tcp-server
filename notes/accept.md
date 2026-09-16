# `accept()`

## 1. What It Is

`accept()` is a system call used by a TCP server to extract the first pending connection request from the listening queue, create a **brand new connected socket**, and return a new file descriptor referencing that client connection.

---

## 2. Why It Exists

A server must be able to talk to a client without blocking future clients from connecting.

If the server used the listening socket itself to exchange data, the socket would become busy with that single conversation and could no longer listen for other incoming clients.

To solve this, `accept()` leaves the original listening socket alone and spawns a **new, dedicated socket** specifically for exchanging data with that connected client.

---

## 3. How It Works

### The Receptionist Analogy

Think of the two sockets like an office building:
* **Listening Socket (`socket_fd`)**: The **front desk receptionist**. The receptionist sits at the main phone line (port 8080) waiting for callers. The receptionist does not conduct business meetings.
* **Client Socket (`client_fd`)**: A **private meeting room**. When a client calls, the receptionist transfers them to a private room with a dedicated line (`client_fd`). The receptionist immediately returns to the front desk, ready for the next caller.

```text
Incoming Client Connection
            |
            v
[ Listening Socket (socket_fd) ]  <-- Stays open on Port 8080
            |
         accept()
            |
            v
[ Connected Client Socket (client_fd) ] <-- Used for recv() and send()
```

### Key Differences

| Property | Listening Socket (`socket_fd`) | Client Socket (`client_fd`) |
| :--- | :--- | :--- |
| **Created by** | `socket()` | `accept()` |
| **Purpose** | Listens for new connections | Communicates with one specific client |
| **Used for** | `bind()`, `listen()`, `accept()` | `recv()`, `send()`, `close()` |
| **Lifecycle** | Stays alive for server's lifetime | Closed when client disconnects |

---

## 4. Syntax

Declared in `<sys/socket.h>`:

```c
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

---

## 5. Important Arguments

* **`sockfd`**: The listening socket file descriptor (must have called `listen()` first).
* **`addr`**: A pointer to a `struct sockaddr` (or cast from `struct sockaddr_in *`). The operating system populates this structure with the **connecting client's IP address and port**.
* **`addrlen`**: A pointer to a `socklen_t` integer. Before calling `accept()`, this must be initialized to `sizeof(client_address)`. Upon return, the kernel updates this variable with the actual number of bytes written.

> **Tip:** If you do not care about the client's IP address or port, you can pass `NULL` for both `addr` and `addrlen`.

---

## 6. Return Value

* **On success**: Returns a non-negative integer representing the **newly created connected client socket descriptor** (e.g., `4`).
* **On failure**: Returns `-1` and sets `errno`. The listening socket remains valid and unaffected.

---

## 7. Error Handling

Always verify that `client_fd >= 0`:

```c
int client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
if (client_fd < 0) {
    perror("accept failed");
    close(socket_fd);
    exit(EXIT_FAILURE);
}
```

If `accept()` fails, `client_fd` is invalid and must not be used for `recv()`, `send()`, or `close()`.

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
    /* Assume socket_fd is already created, bound to 8080, and listening */
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(socket_fd, 5);

    printf("Waiting for incoming client connection on port 8080...\n");

    struct sockaddr_in client_address = {0};
    socklen_t client_len = sizeof(client_address);

    /* accept() blocks until a client connects */
    int client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd < 0) {
        perror("accept");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected! New client socket descriptor: %d\n", client_fd);

    /* Clean up both descriptors */
    close(client_fd);
    close(socket_fd);
    return 0;
}
```
