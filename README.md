# RelyConn chat program

## Technical overview
This project is for learning purposes: its aim is to apply, from theory, multithread socket communication with encryption. The
program is a server-client distributed system where the server forwards the messages from the source client to the given client destination. 
 
It uses C as a programming language and POSIX's pthreads and mutexes for multithreading. 
For the communication between hosts it uses POSIX's socket set to the ipv4 protocol.

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
To download the dependecy click [here](https://gmplib.org/#DOWNLOAD)

if that doesn't work you can try to install it with this in CLI: 
```
sudo apt-get install libgmp3-dev libgmp10
```


## Usage
The repository offers both the server and the client program. To make it so 

The program works on connecting one or more users to a central server, where they can communicate with each other. To communicate the system uses a login procedure: it
stores the username on the server, which serves as an id.
 can be used to redirect the message to the user with that name.


----

## Open source licensing info
1. [TERMS](TERMS.md)
2. [LICENSE](LICENSE)
3. [CFPB Source Code Policy](https://github.com/cfpb/source-code-policy/)

