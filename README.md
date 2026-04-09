# Epoll-based TCP Server in C

## Overview
This project implements a scalable TCP echo server in C using:

- Non-blocking sockets
- epoll (event-driven I/O)
- Per-client connection state management
- Basic logging

## Features
- Handles multiple concurrent clients
- Uses epoll for efficient event handling
- Non-blocking I/O (no thread-per-connection)
- Connection lifecycle logging (connect/disconnect/messages)
- Safe buffer handling

## How it works
- Server socket is set to non-blocking mode
- epoll monitors:
  - server_fd → new connections
  - client_fd → incoming data
- Each client has its own struct:
  ```c
  struct client {
      int fd;
      char buffer[1024];
      int buffer_len;
  }
