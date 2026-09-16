# `htons()`

## 1. What It Is

`htons()` stands for **"Host TO Network Short"**. It is a utility function in C that converts an unsigned 16-bit integer (such as a port number) from **host byte order** to **network byte order**.

---

## 2. Why It Exists

Computers with different CPU architectures store multi-byte values in memory differently:

* **Host Byte Order (Little Endian)**: Common architectures (such as x86 and ARM) store the least significant byte at the lowest memory address.
* **Network Byte Order (Big Endian)**: The TCP/IP network protocol standards strictly require multi-byte numbers to be transmitted with the most significant byte first.

If your host machine is Little Endian and you assign a port number directly without converting it, the bytes will be reversed on the network:

```text
Port 8080 in Hex = 0x1F90

Little Endian (Host):     [ 0x90 ] [ 0x1F ]  --> Read as 36901 in Big Endian!
Big Endian (Network):    [ 0x1F ] [ 0x90 ]  --> Read correctly as 8080
```

`htons()` guarantees that the port number is converted into standard Network Byte Order regardless of the architecture your program runs on.

---

## 3. How It Works

`htons()` inspects the architecture:
* If the machine is already Big Endian, `htons()` does nothing (returns the value unchanged).
* If the machine is Little Endian, `htons()` swaps the two bytes.

```text
Host Value (16-bit short)
+---------+---------+
| Byte A  | Byte B  |  (Little Endian)
+---------+---------+
         |
      htons()
         v
+---------+---------+
| Byte B  | Byte A  |  (Network Byte Order / Big Endian)
+---------+---------+
```

---

## 4. Syntax

Declared in `<arpa/inet.h>`:

```c
#include <arpa/inet.h>

uint16_t htons(uint16_t hostshort);
```

---

## 5. Important Arguments

* **`hostshort`**: The 16-bit unsigned integer to convert. In socket programming, this is almost always the TCP port number (e.g., `8080`, `443`, `80`).

> **Note:** Sockets also provide `htonl()` ("Host to Network Long") for 32-bit values, `ntohs()` ("Network to Host Short"), and `ntohl()` ("Network to Host Long"). For port numbers, always use `htons()` because port numbers are 16-bit integers (`in_port_t`).

---

## 6. Return Value

* Returns the value of `hostshort` formatted in Network Byte Order.

---

## 7. Error Handling

`htons()` cannot fail at runtime. It performs a direct bit-level swap or no-op and does not set `errno`.

The common pitfalls are logical:
* **Forgetting to call `htons()`**: The socket will bind to or connect to a completely different port than intended.
* **Using `htonl()` instead of `htons()`**: Truncates or corrupts 16-bit port values because `htonl()` expects a 32-bit integer.

---

## 8. Small Example

```c
#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
    uint16_t port = 8080;
    uint16_t net_port = htons(port);

    printf("Port in Host Order:    %u (0x%04X)\n", port, port);
    printf("Port in Network Order: %u (0x%04X)\n", net_port, net_port);

    return 0;
}
```
