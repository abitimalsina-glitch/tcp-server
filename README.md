# TCP Server

A minimal **TCP server written from scratch in C** to understand how socket-based networking works at a low level.

This project focuses on the fundamentals of TCP communication rather than implementing a full application-layer protocol such as HTTP.

## Purpose

The goal of this project is to understand how a TCP server works internally and become comfortable with the core socket APIs provided by Unix-like systems.

The server demonstrates the basic lifecycle of a TCP connection:

```text
socket()
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
recv()
   ↓
send()
   ↓
close()
```

## What I Learned

* What a socket is
* File descriptors and how sockets are represented
* Creating a TCP socket with `socket()`
* Configuring an IPv4 address with `sockaddr_in`
* Converting ports with `htons()`
* Converting IP addresses with `inet_pton()`
* Binding a socket to an IP address and port with `bind()`
* Putting a socket into listening mode with `listen()`
* Accepting incoming connections with `accept()`
* Understanding the difference between the listening socket and client socket
* Receiving data with `recv()`
* Sending data with `send()`
* Closing sockets with `close()`
* Understanding that TCP provides a **byte stream**, not application-level messages

## How It Works

The server creates a TCP socket and binds it to:

```text
127.0.0.1:8080
```

It then listens for incoming TCP connections.

When a client connects, `accept()` returns a new file descriptor for that specific connection.

The important distinction is:

```text
socket_fd
    │
    └── Listening socket
         Waits for connections

client_fd
    │
    └── Connected socket
         Used for recv() and send()
```

The server receives data from the client and sends a simple response:

```text
HELLO WORLD
```

## Requirements

* macOS or another Unix-like operating system
* C compiler such as Clang or GCC
* Netcat (`nc`) for testing

Check that a compiler is available:

```bash
clang --version
```

Check Netcat:

```bash
nc -h
```

## Build

Compile the server with:

```bash
clang TCP-Server.c -o ./TCP-Server
```

Then run it:

```bash
./TCP-Server
```

The server will listen on:

```text
127.0.0.1:8080
```
[!Running-The-Server](/images/Starting_Server.png)

## Testing

Open another terminal and connect using Netcat:

```bash
nc 127.0.0.1 8080
```

Type some data:

```text
hello
```
[!Sending_Req](/images/Sending_Request.png)

The server receives the bytes sent by the client and responds with:

```text
Hello Client
```

[!Client](/images/Client_Side_Result.png)
[!Server](/images/Server_Side_Result.png)

This uses raw TCP data rather than HTTP, so tools such as `curl` are intentionally not required for testing.

## Core Socket APIs

### `socket()`

Creates the socket:

```c
socket(AF_INET, SOCK_STREAM, 0);
```

* `AF_INET` → IPv4
* `SOCK_STREAM` → TCP
* `0` → use the default protocol for the socket type

### `bind()`

Associates the socket with an IP address and port:

```text
127.0.0.1:8080
```

### `listen()`

Marks the socket as a listening socket and allows incoming connection requests to queue.

### `accept()`

Waits for an incoming connection.

It returns a **new socket file descriptor** used to communicate with the connected client.

```text
socket_fd → listen
client_fd → communicate
```

### `recv()`

Reads bytes from the connected client socket.

```c
recv(client_fd, buffer, sizeof(buffer), 0);
```

The return value indicates how many bytes were received.

### `send()`

Sends bytes to the connected client:

```c
send(client_fd, data, length, 0);
```

### `close()`

Closes the socket file descriptor when it is no longer needed.

## Current Limitations

This is intentionally a minimal learning project.

It currently:

* Handles a single client connection
* Runs synchronously
* Uses IPv4
* Listens only on `127.0.0.1`
* Does not implement HTTP
* Does not support concurrent clients
* Does not implement robust partial-send handling
* Does not provide sophisticated protocol parsing

These limitations are intentional because the purpose of the project is understanding the fundamentals before adding complexity.

## Why C?

C makes the underlying networking model visible.

Instead of hiding sockets behind a high-level networking library, this project works directly with Unix socket APIs and file descriptors.

The point isn't to build the world's greatest server.

It's to understand what is actually happening underneath the abstractions.

---

**Status:** Learning Project
**Language:** C
**Protocol:** TCP
**Address:** `127.0.0.1:8080`
