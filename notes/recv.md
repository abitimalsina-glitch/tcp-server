# `recv()`

## 1. What It Is

`recv()` is a system call used to read incoming data from a connected TCP socket into a memory buffer allocated by your program.

---

## 2. Why It Exists

When a remote client transmits data across an established TCP connection, the operating system kernel receives the network packets and places the incoming bytes into an internal kernel receive buffer.

Your program cannot access kernel memory directly. `recv()` exists to copy those pending bytes from the kernel's network buffer into a buffer inside your program's memory space.

---

## 3. How It Works

```text
Remote Client
      | (sends TCP segments)
      v
Kernel Socket Receive Buffer
[ H ][ e ][ l ][ l ][ o ][   ][ W ][ o ][ r ][ l ][ d ]
      |
   recv() copies bytes into user space
      v
Your Program's Buffer
char buffer[1024];
```

### TCP Is a Byte Stream, Not a Message System

A crucial concept in network programming is that **TCP is a continuous stream of bytes**:
* TCP does **not** preserve application message boundaries.
* One `send()` from a client does **not** guarantee one `recv()` on the server.

For example, if a client sends `"Hello"` followed by `"World"`, the server calling `recv()` might get:
* All 10 bytes at once: `"HelloWorld"`
* Two separate reads: `"Hello"` then `"World"`
* Arbitrary fragments: `"Hel"`, then `"loWo"`, then `"rld"`

Your application must treat incoming data as an unstructured stream of bytes.

---

## 4. Syntax

Declared in `<sys/socket.h>`:

```c
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

---

## 5. Important Arguments

* **`sockfd`**: The **connected client socket descriptor** (`client_fd`). Do not pass the listening socket (`socket_fd`) here.
* **`buf`**: Pointer to the memory buffer where incoming bytes should be stored.
* **`len`**: The maximum number of bytes to read into the buffer (usually `sizeof(buffer)`). This prevents buffer overflows.
* **`flags`**: Modifiers for receiving data. Pass `0` for standard blocking behavior.

---

## 6. Return Value

`recv()` returns a signed size (`ssize_t`) with **three fundamentally different meanings**:

* **`> 0` (Positive number)**: Success. The return value is the exact number of bytes read and placed into your buffer.
* **`0`**: **Orderly peer shutdown (EOF)**. The remote client closed their connection. This is **not an error**—it signals that the client has finished transmitting.
* **`-1` (Negative number)**: An error occurred. `errno` is set to explain the failure.

---

## 7. Error Handling

You must handle all three return conditions explicitly:

```c
ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

if (bytes_received < 0) {
    perror("recv failed");
    close(client_fd);
    close(socket_fd);
    exit(EXIT_FAILURE);
}

if (bytes_received == 0) {
    printf("Client disconnected gracefully.\n");
    close(client_fd);
    /* Ready to accept a new client or terminate */
}
```

### Crucial Safety Note: Null Termination

`recv()` reads **raw bytes**, not C strings. It does **not** append a null terminator (`\0`). If you attempt to print the buffer with `printf("%s", buffer)`, your program may read garbage memory beyond the buffer and crash.

Always print using precision specifiers:

```c
printf("Received: %.*s\n", (int)bytes_received, buffer);
```

Or manually null-terminate if there is room:

```c
buffer[bytes_received] = '\0';
```

---

## 8. Small Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(void)
{
    /* Assume client_fd is an already connected socket from accept() */
    int client_fd = 4;
    char buffer[BUFFER_SIZE];

    printf("Waiting for client data...\n");
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received < 0) {
        perror("recv");
        close(client_fd);
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        printf("Client closed connection.\n");
    } else {
        printf("Received %zd bytes: %.*s\n", bytes_received, (int)bytes_received, buffer);
    }

    close(client_fd);
    return 0;
}
```
