# RelyConn chat program

## Overview
This project is for learning purposes: its aim is to apply, from theory, multithread socket communication with encryption.

### Technical overview
The program is a server-client distributed system where the server forwards the messages from the source client to the given client destination. 


It uses C as a programming language, and posix's pthreads and mutexes for multithreading. 
For the communication between hosts it also uses posix's sockets.
Lastly for the trasmission of packets it uses the TCP protocol for reliable messanging.

#### The encryption
The encryption system is an hybrid assymmetric-symmetric key algorithm; to be more specific it is an RSA-AES hybrid.
It uses RSA encrpytion first communicate the symmetric key, and then it uses the key with the AES algorithm to encrpyt the actual messages
between clients.


## Platforms

The program is only tested to work on Ubuntu linux. Other systems need a configuration which has not been implemented yet.


## Installation

Fork the repository; after that import the dependencies.

## Dependencies

#### GNU-MP 
A library for handling arbitrary arithmetic operations (I.E 1024 bit values or more (used mainly in RSA encryption))
to download the dependecy click [here](https://gmplib.org/#DOWNLOAD)

if that doesn't work you can try to install it with this in CLI: 
```
sudo apt-get install libgmp3-dev libgmp10
```


## Usage
The repository offers both the server and the client program. 
When a client connects to the server the encryption system is initialized and immediatly after 
that a login procedure is required. This procedure asks for a username which will be used as an id on the server. The id is used then by every other use to
communicate with each other.


----

## Open source licensing info
[LICENSE](LICENSE)

