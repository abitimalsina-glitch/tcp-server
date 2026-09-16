# `close()`

## 1. What It Is

`close()` is a POSIX system call in C used to close a file descriptor, releasing the underlying operating system resources and terminating the associated network connection.

---

## 2. Why It Exists

Operating systems place strict limits on the number of open file descriptors a single process can maintain (the file descriptor limit). Additionally, active sockets tie up kernel memory for send/receive buffers, TCP state tracking tables, and port reservations.

If your program accepts connections without closing them, your server will suffer a **file descriptor leak**. Eventually, future calls to `accept()`, `socket()`, or `open()` will fail with `EMFILE` ("Too many open files"), and the server will crash or stall.

---

## 3. How It Works

Because sockets in Unix-like systems are represented as file descriptors, they are closed with the standard `<unistd.h>` `close()` function:

### Closing a Connected Client Socket (`client_fd`)
When you close a connected client socket:
1. The OS kernel initiates the **TCP 4-way termination handshake** by sending a `FIN` packet to the remote peer.
2. The remote peer's next `recv()` returns `0` (EOF).
3. The kernel deallocates the client socket's buffers and frees the descriptor index in the process table.

### Closing a Listening Socket (`socket_fd`)
When you close the listening server socket:
1. The socket stops listening for incoming connections.
2. Any unaccepted connections still sitting in the backlog queue are discarded or reset.
3. The port binding is released by the operating system.

```text
Process File Descriptor Table
Before close(client_fd):
Index 3 -> Listening Socket (Port 8080)
Index 4 -> Connected Client Socket (Active TCP Session)

After close(client_fd):
Index 3 -> Listening Socket (Port 8080)
Index 4 -> [ FREE / REUSABLE ]
```

---

## 4. Syntax

Declared in `<unistd.h>`:

```c
#include <unistd.h>

int close(int fd);
```

---

## 5. Important Arguments

* **`fd`**: The file descriptor to close (e.g., `client_fd` or `socket_fd`).

---

## 6. Return Value

* **`0`**: Success. The descriptor was closed.
* **`-1`**: Failure. `errno` is set (e.g., `EBADF` if `fd` is not a valid open file descriptor).

---

## 7. Error Handling & Best Practices

1. **Avoid Double Closes:**
   Closing a descriptor twice is a dangerous bug. If another thread or library call opens a new socket or file that receives the newly freed descriptor number, a second `close()` will inadvertently destroy the unrelated file descriptor.
2. **Order of Cleanup:**
   Always close the client socket when its individual request/response cycle ends. Only close the listening socket when the entire server is shutting down:

```c
/* After completing interaction with client */
close(client_fd);

/* When server is terminating */
close(socket_fd);
```

---

## 8. Small Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

int main(void)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket created with descriptor %d\n", socket_fd);

    /* Clean up and release the socket */
    if (close(socket_fd) < 0) {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket %d successfully closed and freed.\n", socket_fd);
    return 0;
}
```
