# `inet_pton()`

## 1. What It Is

`inet_pton()` stands for **"Internet Presentation to Network"**. It is a C socket function that converts an IP address from human-readable text format (presentation) into binary format (network byte order).

---

## 2. Why It Exists

Humans read IP addresses as ASCII strings formatted in dotted-decimal notation:

```text
"127.0.0.1"
"192.168.1.50"
```

However, the operating system kernel and network hardware do not understand text strings. An IPv4 address is fundamentally a **32-bit unsigned binary integer** (4 bytes). 

You cannot pass a string like `"127.0.0.1"` into `server_address.sin_addr`. `inet_pton()` validates the string and packs the 4 octets into the binary format required by `struct in_addr`.

> **Note:** `inet_pton()` is the modern, safe replacement for older legacy functions such as `inet_addr()` and `inet_aton()`.

---

## 3. How It Works

`inet_pton()` parses the text string:
1. It validates that there are four valid numbers separated by dots, each in the range 0–255.
2. It converts each number into an 8-bit byte.
3. It copies the 4 bytes in network byte order directly into the destination buffer (`dst`).

```text
Presentation (String)       Binary (Network Byte Order)
     "127.0.0.1"     -->    [ 127 ][ 0 ][ 0 ][ 1 ]
     (10 bytes text)        (4 bytes in struct in_addr)
```

---

## 4. Syntax

Declared in `<arpa/inet.h>`:

```c
#include <arpa/inet.h>

int inet_pton(int af, const char *src, void *dst);
```

---

## 5. Important Arguments

* **`af` (Address Family)**: The address family of the address string. Usually `AF_INET` for IPv4 (or `AF_INET6` for IPv6).
* **`src`**: Pointer to the null-terminated ASCII string containing the IP address (e.g., `"127.0.0.1"`).
* **`dst`**: Pointer to the memory location where the binary address will be written. For IPv4, this is the address of a `struct in_addr` (e.g., `&server_address.sin_addr`).

---

## 6. Return Value

Unlike many POSIX socket functions that return only `0` or `-1`, `inet_pton()` has **three distinct return conditions**:

* **`1`**: Success. The string was a valid address and conversion succeeded.
* **`0`**: The input string is not a valid network address in the specified address family (e.g. `"999.0.0.1"` or `"localhost"`).
* **`-1`**: System error (e.g. invalid `af` family passed). `errno` is set (e.g. `EAFNOSUPPORT`).

---

## 7. Error Handling

Because both `0` (invalid IP string) and `-1` (system error) represent failures, you should always check for `<= 0`:

```c
if (inet_pton(AF_INET, ip_string, &server_address.sin_addr) <= 0) {
    perror("inet_pton failed: invalid address or unsupported family");
    close(socket_fd);
    exit(EXIT_FAILURE);
}
```

---

## 8. Small Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(void)
{
    struct in_addr addr;
    const char *ip_str = "127.0.0.1";

    int status = inet_pton(AF_INET, ip_str, &addr);

    if (status <= 0) {
        if (status == 0) {
            fprintf(stderr, "Invalid IP format: %s\n", ip_str);
        } else {
            perror("inet_pton");
        }
        exit(EXIT_FAILURE);
    }

    printf("Successfully converted IP '%s' to binary: 0x%08X\n", ip_str, addr.s_addr);
    return 0;
}
```
