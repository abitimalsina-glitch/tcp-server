# Error Handling in C Sockets

## 1. What It Is

Error handling in socket programming is the practice of checking the return values of operating system network calls, inspecting `errno`, and printing descriptive messages using `perror()` to guarantee reliability.

---

## 2. Why It Exists

Networking is inherently unpredictable:
* A port might already be in use by another application (`EADDRINUSE`).
* The system might run out of file descriptors (`EMFILE`).
* A remote peer might suddenly drop connection (`ECONNRESET`).
* An invalid IP string might be provided.

If your code ignores return values, the program will continue executing with invalid file descriptors (such as `-1`), resulting in cascading failures, crashes, and confusing bugs.

---

## 3. How It Works

### The `errno` and `perror()` Mechanism

When a POSIX system call fails:
1. The function returns a sentinel value (usually `-1`).
2. The operating system kernel writes an integer error code into a global variable named `errno`.
3. `perror(const char *prefix)` reads `errno`, finds the corresponding human-readable error description, and prints:
   ```text
   <prefix>: <human-readable OS error message>
   ```

```text
Function Fails
      |
      v
Kernel sets errno = EADDRINUSE (48)
      |
      v
perror("bind")
      |
      v
Prints to stderr: "bind: Address already in use"
```

### Critical Distinction: Function Failure vs. Normal State Transition

Not every non-positive return value is a fatal error:

* **`recv()` returning `-1`**: A genuine error occurred (e.g. connection aborted, timeout).
* **`recv()` returning `0`**: **NOT an error**. It is the standard TCP End-Of-File (EOF) notification indicating that the client performed a clean, graceful disconnection.
* **`inet_pton()` returning `0`**: The IP string syntax was invalid (not a system error, but user input error).

---

## 4. Syntax

Declared in standard library headers:

```c
#include <stdio.h>   /* For perror() */
#include <stdlib.h>  /* For exit() and EXIT_FAILURE */
#include <errno.h>   /* For errno variable */

void perror(const char *s);
```

---

## 5. Return Value Patterns Across the Socket API

| Function | Success Value | Error Value | Special State |
| :--- | :--- | :--- | :--- |
| `socket()` | `fd >= 0` | `-1` | None |
| `inet_pton()` | `1` | `-1` | `0` (invalid IP format) |
| `bind()` | `0` | `-1` | None |
| `listen()` | `0` | `-1` | None |
| `accept()` | `client_fd >= 0` | `-1` | None |
| `recv()` | `> 0` (bytes read) | `-1` | `0` (peer closed connection) |
| `send()` | `> 0` (bytes sent) | `-1` | `< len` (partial send) |
| `close()` | `0` | `-1` | None |

---

## 6. Resource Cleanup on Failure

When a step fails, you should clean up any resources allocated in earlier steps before terminating:

```c
/* Example: If bind() fails, close the already-opened socket_fd */
if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(socket_fd); /* Free socket descriptor before exit */
    exit(EXIT_FAILURE);
}
```

---

## 7. Error Handling Boilerplate Pattern

The standard idiom in C socket programming is:

```c
if (result < 0) {
    perror("descriptive step name");
    /* Close open descriptors */
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

int main(void)
{
    /* Intentionally pass an invalid domain family to trigger an error */
    int socket_fd = socket(-1, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        /* perror will print: "socket: Address family not supported by protocol family" */
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* If successful, close and exit cleanly */
    close(socket_fd);
    return 0;
}
```
