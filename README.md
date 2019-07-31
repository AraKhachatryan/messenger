# messenger
Linux/Unix terminal command line messenger (chat) with POSIX sockets and threads

## Files
- README.md - the current readme file
- client.cpp - multithreaded client with POSIX sockets for Linux/Unix terminal command line messenger
- encode_decode.cpp	- functions definations for encodeing and decoding messages for sending over TCP socket
- encode_decode.hpp	- functions declarations for encodeing and decoding messages for sending over TCP socket
- makefile - compiling and bild automation with GNU make
- namespace_terminal.hpp - designed for manipulating with linux terminal
- server.cpp - Multi client and multithreaded server with POSIX sockets for Linux/Unix terminal command line messenger

## Requirements
- g++ compiler with C++11 standart
- Supporting POSIX standarts
- GNU make

## Compilation:

Compilation is done with GNU make, just type on terminal "make"

## Usage:

- on server side: ./server

- on client side: ./client  server_IPv4_address


### An screenshot:

<img width="734" height="100%" src="https://pbs.twimg.com/media/EAwKUj6WwAIuGm2?format=png&name=900x900">

