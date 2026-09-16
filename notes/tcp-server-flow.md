# TCP Server Lifecycle and Flow

This document serves as the architectural overview connecting all individual socket programming notes. It details the complete step-by-step lifecycle of a basic TCP server in C.

Before diving into the steps below, you may also refer to:
* [What Is a Socket?](./what-is-a-socket.md) – Foundational concepts and OS descriptors.
* [Error Handling](./error-handling.md) – General strategies, `perror()`, and return value checks.

---

## The Lifecycle Flow

```text
       socket()
          │
          ▼
configure sockaddr_in
          │
          ▼
        htons()
          │
          ▼
      inet_pton()
          │
          ▼
        bind()
          │
          ▼
       listen()
          │
          ▼
       accept()  <─────────────────+
          │                        │
          ▼                        │
        recv()                     │
          │                        │ (Wait for next client)
          ▼                        │
        send()                     │
          │                        │
          ▼                        │
  close(client_fd)  ───────────────+
          │
          ▼
  close(socket_fd)  (Server shutdown)
```

---

## Step-by-Step Breakdown

### 1. [Socket Creation (`socket()`)](./socket-creation.md)
* **Action:** Request a new communication endpoint from the operating system.
* **Role in Server:** Creates the initial unbound file descriptor (`socket_fd`) configured for IPv4 stream communication (`AF_INET`, `SOCK_STREAM`, `0`).

### 2. [Configure `struct sockaddr_in`](./sockaddr-in.md)
* **Action:** Allocate and zero-initialize the IPv4 address structure (`struct sockaddr_in server_address = {0};`).
* **Role in Server:** Sets the address family to `AF_INET` and prepares the struct to store the server's listening port and IP address.

### 3. [Convert Port Byte Order (`htons()`)](./htons.md)
* **Action:** Convert the integer port number (e.g. `8080`) from host byte order to network byte order (Big Endian).
* **Role in Server:** Populates `server_address.sin_port = htons(PORT)` so that network hardware interprets the port number correctly.

### 4. [Convert IP Address (`inet_pton()`)](./inet-pton.md)
* **Action:** Convert the human-readable IP string (e.g. `"127.0.0.1"`) into 32-bit binary network format.
* **Role in Server:** Populates `server_address.sin_addr` using `inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr)`.

### 5. [Bind Address to Socket (`bind()`)](./bind.md)
* **Action:** Attach the configured IP address and port to the socket descriptor.
* **Role in Server:** Reserves the port on the system and registers the socket so incoming traffic to that port is directed to this program.

### 6. [Enter Listening State (`listen()`)](./listen.md)
* **Action:** Transition the socket from an active socket into a passive listening socket.
* **Role in Server:** Instructs the OS kernel to start accepting TCP connection handshakes and buffering them in a backlog queue.

### 7. [Accept Client Connection (`accept()`)](./accept.md)
* **Action:** Dequeue the first pending connection from the listening queue.
* **Role in Server:** Spawns a brand new socket descriptor (`client_fd`) dedicated exclusively to communicating with the connected client, leaving `socket_fd` open to accept future clients.

### 8. [Receive Data (`recv()`)](./recv.md)
* **Action:** Read incoming bytes from the client socket into an application buffer.
* **Role in Server:** Retrieves the client's request. Return value determines whether data arrived (`> 0`), client disconnected cleanly (`== 0`), or an error occurred (`< 0`).

### 9. [Send Response (`send()`)](./send.md)
* **Action:** Write outgoing bytes from an application buffer across the client socket.
* **Role in Server:** Transmits response data back to the client. Handles partial sends and connection resets.

### 10. [Resource Cleanup (`close()`)](./close.md)
* **Action:** Close file descriptors and release operating system resources.
* **Role in Server:** Closes `client_fd` when the client conversation finishes, and closes `socket_fd` when the server terminates.

---

## Directory Index

| Concept | Note Link |
| :--- | :--- |
| Socket Fundamentals | [what-is-a-socket.md](./what-is-a-socket.md) |
| Creating a Socket | [socket-creation.md](./socket-creation.md) |
| IPv4 Address Structure | [sockaddr-in.md](./sockaddr-in.md) |
| Byte Ordering | [htons.md](./htons.md) |
| IP Presentation to Binary | [inet-pton.md](./inet-pton.md) |
| Binding IP and Port | [bind.md](./bind.md) |
| Listening for Connections | [listen.md](./listen.md) |
| Accepting Clients | [accept.md](./accept.md) |
| Receiving Data | [recv.md](./recv.md) |
| Sending Data | [send.md](./send.md) |
| Closing Sockets | [close.md](./close.md) |
| Error Handling Patterns | [error-handling.md](./error-handling.md) |
