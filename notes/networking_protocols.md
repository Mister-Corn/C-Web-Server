# Headers and TCP/UDP

## Data Encapsulation

```
+-----------------+
| Ethernet Header |  Deals with routing on the LAN
+-----------------+
| IP Header       |  Deals with routing on the Internet
+-----------------+
| TCP Header      |  Deals with data integrity
+-----------------+
| HTTP Header     |  Deals with web data
+-----------------+
| <h1>Hello, worl |  Whatever you need to send
| d!</h1>         |
|                 |
+-----------------+
```

Notice the stack structure. As the request makes its journey, headers are read and popped.

## TCP (_Transmission Control Protocol_)

TCP is commonly used.

* Extremely reliable. Best for use cases where you want to limit data loss.
This is done with the "three-way-handshake".
Connection must be initialized and estashblished before the exchange of data can happen.

If the "three-way-handshake" fails, the client will try to handshake again, up to a cap.

## UDP (_Uniform Protocol_)

Instead of a verification process, UDP just sprays data like a firehose, with little safeguards against packet loss.

# Sockets

## What are UNIX sockets?

High level: API to send messages between the server and the client.

Low level:

* Everything in Unix is a file.
* Any time we want to do I/O between multiple processes, we used an intermediary shared file to do that. The processes use a "file descriptor", the handle they use to read and write to the shared file.
* Remember: _this isn't the only way for processes to communicate with each other_, but is most common.
* The file descriptor is an integer key that can be used to access a specific file.

"File" can refer to any of the following:
* A network connection
* A pipe
* A terminal
* An actual "normal" file

What we're focusing on "A network connection". The idea is still the same, but now the processes are in different machines, the thing connecting them is a network connection.

## Initializing a Socket

```c
#include <sys/socket.h>

// initialize a socket
int sockfd = socket(. . .)
// This will give us an integer socket descriptor.

// send and receive data using the socket descriptor
send(sockfd, response, responseLength, 0);
recv(sockfd, request, requestBufferSize - 1, 0);
```



