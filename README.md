# messenger
Linux/Unix terminal command line messenger(chat) with POSIX sockets and threads

## Files
- README.md
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

on server side:
./server

on client side:

./client server_ip_address

