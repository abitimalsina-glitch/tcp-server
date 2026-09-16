# `send()`

## 1. What It Is

`send()` is a system call used to transmit a sequence of bytes from your program's memory buffer over a connected TCP socket to the remote peer.

---

## 2. Why It Exists

After accepting a client connection and receiving a request, a server usually needs to reply. `send()` provides the interface to push response data from application memory into the operating system's network transmission pipeline.

---

## 3. How It Works

When you call `send()`:

1. The OS kernel copies the specified number of bytes from your user-space buffer into the kernel's **socket send buffer**.
2. The operating system's TCP/IP stack packages those bytes into TCP segments with proper sequence numbers and checksums.
3. The network hardware transmits the packets across the wire.

```text
Your Program
char data[] = "Hello Client";
       |
    send() copies bytes to kernel
       v
Kernel Socket Send Buffer
[ H ][ e ][ l ][ l ][ o ][   ][ C ][ l ][ i ][ e ][ n ][ t ]
       |
    OS TCP Stack transmits packets
       v
Remote Client
```

> **Important concept:** A successful `send()` does **not** mean the remote peer has received the data yet. It only confirms that the operating system kernel has successfully accepted the bytes into its send queue.

### Note on Partial Sends

In standard TCP socket programming, `send()` is not guaranteed to send all requested bytes in a single call. If the kernel's send buffer is nearly full, `send()` may transmit only a portion of your buffer and return the number of bytes actually queued (a **partial send**). For small messages in basic servers, all bytes typically send at once, but checking the return value is essential.

---

## 4. Syntax

Declared in `<sys/socket.h>`:

```c
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

---

## 5. Important Arguments

* **`sockfd`**: The **connected client socket descriptor** (`client_fd`).
* **`buf`**: Pointer to the memory buffer containing the data you want to send.
* **`len`**: The number of bytes to send.
  * For string literals, use `sizeof(data) - 1` or `strlen(data)` so you do not accidentally send the trailing null terminator (`'\0'`) unless your protocol explicitly expects it.
* **`flags`**: Special flags modifying transmission behavior. Pass `0` for default blocking behavior.

---

## 6. Return Value

* **On success**: Returns the number of bytes actually sent and accepted by the kernel (`ssize_t > 0`).
* **On failure**: Returns `-1` and sets `errno` to identify the failure reason.

---

## 7. Error Handling

Always check if `send()` returns a negative value:

```c
ssize_t bytes_sent = send(client_fd, data, len, 0);
if (bytes_sent < 0) {
    perror("send failed");
    close(client_fd);
    close(socket_fd);
    exit(EXIT_FAILURE);
}
```

Common causes of `send()` failure include:
* **`EBADF`**: The file descriptor is invalid or has already been closed.
* **`EPIPE` / `ECONNRESET`**: The remote client abruptly closed or reset the connection before the data could be transmitted.

---

## 8. Small Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

int main(void)
{
    /* Assume client_fd is an active connected client socket */
    int client_fd = 4;
    char response[] = "Hello Client\n";
    size_t response_len = sizeof(response) - 1; /* Exclude null terminator */

    ssize_t bytes_sent = send(client_fd, response, response_len, 0);

    if (bytes_sent < 0) {
        perror("send");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Successfully sent %zd of %zu bytes\n", bytes_sent, response_len);

    close(client_fd);
    return 0;
}
```
