# How to Create a Socket in C

In C, we create a socket using the `socket()` function.

The basic form is:

```c
socket(domain, type, protocol);
```

A common example is:

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

The three arguments tell the OS **what kind of socket we want**.

---

## 1. Domain

The **domain** tells the OS which **address family** the socket will use.

Example:

```c
AF_INET
```

means:

```text
AF_INET → IPv4
```

For example, if we want our socket to communicate using IPv4 addresses such as:

```text
192.168.1.10
10.0.0.5
8.8.8.8
```

we use:

```c
AF_INET
```

There are other address families too. For example:

```c
AF_INET6
```

means IPv6.

So:

```text
Domain
   |
   ├── AF_INET  → IPv4
   └── AF_INET6 → IPv6
```

> **Think of domain as:** "What addressing system am I using?"

---

## 2. Type

The **type** tells the OS what kind of communication mechanism the socket should use.

### `SOCK_STREAM`

```c
SOCK_STREAM
```

means a **stream socket**.

It is normally used with **TCP**.

```text
SOCK_STREAM → TCP
```

Example:

```c
socket(AF_INET, SOCK_STREAM, 0);
```

Think:

> "I want a continuous stream of data."

### `SOCK_DGRAM`

```c
SOCK_DGRAM
```

means a **datagram socket**.

It is normally used with **UDP**.

```text
SOCK_DGRAM → UDP
```

Example:

```c
socket(AF_INET, SOCK_DGRAM, 0);
```

Think:

> "I want to send individual datagrams."

So:

```text
Type
   |
   ├── SOCK_STREAM → TCP
   └── SOCK_DGRAM  → UDP
```

> **Think of type as:** "How do I want to communicate?"

---

## 3. Protocol

The **protocol** tells the OS which specific network protocol should be used.

For many common sockets, we can simply use:

```c
0
```

This means:

> **Let the OS choose the appropriate protocol based on the domain and type.**

For example:

```c
socket(AF_INET, SOCK_STREAM, 0);
```

The OS sees:

```text
AF_INET      → IPv4
SOCK_STREAM  → Stream
0            → Choose appropriate protocol
```

and normally chooses:

```text
TCP
```

Another example:

```c
socket(AF_INET, SOCK_DGRAM, 0);
```

The OS normally chooses:

```text
UDP
```

> **Think of protocol as:** "Which specific protocol should this socket use?"

---

# Putting Everything Together

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

Break it down:

```text
socket(
    AF_INET,       → Domain: IPv4
    SOCK_STREAM,   → Type: Stream / TCP
    0              → Protocol: OS chooses
);
```

So the program is basically saying:

> **"OS, create me an IPv4 stream socket and choose the appropriate protocol."**

The OS then returns a descriptor:

```text
socket()
   |
   v
Operating System
   |
   v
Creates socket
   |
   v
Returns a descriptor
   |
   v
sockfd = 3
```

---

# Easy Mental Model

Remember the three arguments like this:

```text
DOMAIN   → WHERE / WHAT addressing system?
TYPE     → HOW do I communicate?
PROTOCOL → WHICH specific protocol?
```

Example:

```c
socket(AF_INET, SOCK_STREAM, 0);
```

```text
AF_INET      → IPv4
SOCK_STREAM  → TCP-style stream
0            → OS chooses protocol
```

---

## One Important Distinction

These three things are related, but they are **not the same thing**:

```text
Domain    → Address family
Type      → Socket communication style
Protocol  → Network protocol
```

For now, the most important combinations to remember are:

```text
AF_INET + SOCK_STREAM → IPv4 + TCP
AF_INET + SOCK_DGRAM  → IPv4 + UDP
```

We will learn about other socket types and protocols later.

