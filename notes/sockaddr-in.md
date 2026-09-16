# `struct sockaddr_in`

## 1. What It Is

`struct sockaddr_in` is a data structure in C used to specify an **IPv4 network address endpoint** consisting of an IP address and a port number.

---

## 2. Why It Exists

Socket system calls such as `bind()`, `accept()`, and `connect()` must support multiple address families (IPv4, IPv6, Unix domain sockets). To accept any address type, these functions take a generic pointer:

```c
struct sockaddr *
```

However, `struct sockaddr` is awkward to work with directly because it stores the address and port as an untyped char array. 

To solve this, the C socket API provides protocol-specific structures:
* `struct sockaddr_in` for **IPv4**
* `struct sockaddr_in6` for **IPv6**

You populate the convenient, structured `struct sockaddr_in`, and then cast its pointer to `(struct sockaddr *)` when passing it to socket functions.

---

## 3. How It Works

`struct sockaddr_in` is designed to be the exact same size (16 bytes) as the generic `struct sockaddr`:

```text
struct sockaddr (Generic, 16 bytes)
+-------------------+------------------------------------------+
| sa_family (2B)    | sa_data (14 bytes)                       |
+-------------------+------------------------------------------+

struct sockaddr_in (IPv4 Specific, 16 bytes)
+-------------------+------------------+-----------------+-----+
| sin_family (2B)   | sin_port (2B)    | sin_addr (4B)   | pad |
+-------------------+------------------+-----------------+-----+
                                                          (8B)
```

Because both structures share the exact same byte layout in memory, the operating system reads `sin_family` first, recognizes `AF_INET`, and knows how to parse the subsequent IPv4 address and port fields.

---

## 4. Syntax

Declared in `<netinet/in.h>` (often included via `<arpa/inet.h>`):

```c
struct sockaddr_in {
    sa_family_t    sin_family; /* Address family: AF_INET */
    in_port_t      sin_port;   /* 16-bit port number in Network Byte Order */
    struct in_addr sin_addr;   /* 32-bit IPv4 address */
    char           sin_zero[8];/* Padding bytes to match struct sockaddr */
};

struct in_addr {
    in_addr_t      s_addr;     /* 32-bit IPv4 address in Network Byte Order */
};
```

---

## 5. Important Fields

* **`sin_family`**: Specifies the address family. For IPv4, this must always be `AF_INET`.
* **`sin_port`**: The 16-bit port number (0–65535). Must be stored in **Network Byte Order** using `htons()`.
* **`sin_addr`**: Holds the 32-bit IPv4 address (also in Network Byte Order). Configured using helper functions like `inet_pton()` or set to `INADDR_ANY`.
* **`sin_zero`**: Unused padding bytes. They must be zeroed out so the structure matches the size of generic `struct sockaddr`.

---

## 6. Return Value / Sizing

As a data structure, `struct sockaddr_in` does not have a return value. However, functions that take it (such as `bind()` and `accept()`) require you to pass its size:

```c
sizeof(server_address)
```

For `accept()`, the size is passed as a pointer to a `socklen_t` variable (`&client_len`) so the operating system can write back the actual number of bytes written.

---

## 7. Error Handling & Common Mistakes

1. **Forgetting `htons()` on the port:**
   Assigning a raw integer port (e.g. `server_address.sin_port = 8080;`) flips the byte order on little-endian machines, causing your server to bind to port `36901` instead of `8080`.
2. **Failing to zero-initialize the structure:**
   Uninitialized stack memory leaves garbage values in `sin_zero`, which can cause `bind()` to fail on some operating systems. Always zero the struct:
   ```c
   struct sockaddr_in server_address = {0};
   ```
3. **Omitting the pointer cast:**
   Passing `&server_address` without `(struct sockaddr *)` will trigger compiler warnings because the parameter types do not match.

---

## 8. Small Example

```c
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(void)
{
    /* 1. Zero-initialize the entire structure */
    struct sockaddr_in server_address = {0};

    /* 2. Set address family to IPv4 */
    server_address.sin_family = AF_INET;

    /* 3. Set port in Network Byte Order */
    server_address.sin_port = htons(8080);

    /* 4. Set IPv4 address */
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    printf("sockaddr_in configured for 127.0.0.1:8080\n");
    return 0;
}
```
