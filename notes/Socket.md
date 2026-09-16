# What Is a Socket?

A **socket** is a communication endpoint/interface that a program uses to communicate with another program over a network.

Think of a socket like a **phone for a program**:

```text
Your C program
      |
      v
   SOCKET
      |
      v
Operating System
      |
      v
 Network Stack
      |
      v
    Network
      |
      v
Another program/device
```

## Important Distinction

A socket is **not**:

* An IP address
* A port
* A physical network card
* The network itself

A socket is the **interface your program uses to access network communication through the operating system**.

For normal application-level network communication in C, sockets are the standard interface provided by the operating system.

## Socket Descriptor

When the OS creates a socket, it gives the program a number called a **file descriptor** on Unix-like systems such as macOS and Linux.

For example:

```text
sockfd = 3
```

The number is a handle that your program uses to refer to the socket managed by the OS.

> **Mental model:** Your program doesn't directly control the physical network. It uses a socket, and the OS handles the networking underneath.

