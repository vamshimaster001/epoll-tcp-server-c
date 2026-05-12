TCP Echo Server using epoll in C

Overview

This project is a non-blocking TCP echo server written in C using Linux epoll.

The server accepts multiple client connections asynchronously and echoes received messages back to the clients.

The project was built to learn:

- TCP socket programming
- Linux epoll
- Non-blocking sockets
- Event-driven I/O
- Client connection management
- Basic server architecture


Features

- TCP server using POSIX sockets
- Non-blocking sockets using fcntl()
- Event-driven architecture using epoll
- Multiple concurrent client handling
- Dynamic client connection management
- Echo response handling
- Structured logging
- Graceful client disconnect handling


Architecture

                +-------------------+
                |      Client       |
                +-------------------+
                          |
                          v
                +-------------------+
                |   Echo Server     |
                |      epoll        |
                +-------------------+
                          |
                          v
                Echo message to client


Core Concepts Implemented

Socket Programming

- socket()
- bind()
- listen()
- accept()
- recv()
- send()

Event-Driven I/O

The server uses Linux epoll for scalable I/O event notification.

Supported events:

- EPOLLIN

Non-Blocking I/O

Sockets are configured using:

fcntl(fd, F_SETFL, O_NONBLOCK);

This prevents blocking on:

- accept()
- recv()
- send()


Connection Flow

Client connects
    ↓
Server accepts connection
    ↓
Client socket added to epoll
    ↓
Server receives message
    ↓
Server sends echo response
    ↓
Connection stays active until disconnect


Build Instructions

gcc -o server server.c


Running The Server

./server


Testing

Using nc:

nc 127.0.0.1 9090

Type messages and the server will echo them back.


Example

Client:

hello

Server Response:

ECHO SERVER: hello


Technologies Used

- C
- Linux
- epoll
- POSIX sockets
- TCP/IP


Learning Goals

This project was built to understand:

- Linux networking fundamentals
- Event-driven server design
- Non-blocking socket programming
- epoll-based scalability
- Connection lifecycle handling
- Memory management in C
