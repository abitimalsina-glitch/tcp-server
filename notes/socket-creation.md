# Socket Creation with `socket()`

## 1. What It Is

`socket()` is a system call in C that requests the operating system to create a new communication socket and return an integer file descriptor referencing it.

---

## 2. Why It Exists

Before a program can perform any network actions—such as binding an address, waiting for incoming clients, or sending and receiving bytes—it must first obtain an active communication channel from the operating system. The `socket()` call establishes the foundational socket object with defined communication rules.

---

## 3. How It Works

When your program calls `socket()`:

1. The arguments tell the operating system kernel what variety of communication endpoint is needed.
2. The kernel verifies support for the requested address family and protocol.
3. The kernel allocates internal memory structures for the socket (protocol state blocks and buffer queues).
4. An entry is added to your process's file descriptor table pointing to the newly created socket structure.
5. The kernel returns the file descriptor number (e.g., `3`, `4`) to your program.

```text
Program                              Operating System Kernel
   |                                            |
   |--- socket(AF_INET, SOCK_STREAM, 0) ------->|
   |                                            | Allocates socket structure
   |                                            | Assigns file descriptor (e.g., 3)
   |<-- Returns sockfd (3) ---------------------|
```

---

## 4. Syntax

Declared in `<sys/socket.h>`:

```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

---

## 5. Important Arguments

### 1. `domain` (Address Family)
Specifies which network address family the socket will use.
* `AF_INET`: IPv4 addresses (e.g., `127.0.0.1`, `192.168.1.1`).
* `AF_INET6`: IPv6 addresses.

> **Mental model:** "What addressing system am I using?"

### 2. `type` (Socket Type)
Specifies the communication semantics and transport mechanism.
* `SOCK_STREAM`: Provides a sequenced, reliable, two-way, connection-based byte stream. This corresponds to **TCP**.
* `SOCK_DGRAM`: Provides connectionless, unreliable datagrams of fixed maximum length. This corresponds to **UDP**.

> **Mental model:** "How do I want to communicate?"

### 3. `protocol` (Specific Protocol)
Specifies a particular protocol to be used with the socket.
* Passing `0` instructs the kernel to use the default protocol for the given domain and type.
  * `AF_INET` + `SOCK_STREAM` defaults to **TCP** (`IPPROTO_TCP`).
  * `AF_INET` + `SOCK_DGRAM` defaults to **UDP** (`IPPROTO_UDP`).

> **Mental model:** "Which specific protocol should this socket use?"

---

## 6. Return Value

* **On success**: Returns a non-negative integer representing the socket file descriptor (e.g., `3`).
* **On failure**: Returns `-1` and sets `errno` to indicate the error (e.g., `EMFILE` if the process exceeded its open file limit, or `EACCES` if permission was denied).

---

## 7. Error Handling

Always check whether the returned descriptor is less than `0` before attempting any further operations:

```c
int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
if (socket_fd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
}
```

If `socket()` fails, no descriptor was allocated, so there is no descriptor to close.

---

## 8. Small Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    /* Create an IPv4 TCP stream socket */
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully created with descriptor: %d\n", socket_fd);

    /* Clean up when done */
    close(socket_fd);
    return 0;
}
```
