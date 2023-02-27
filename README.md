# RelyConn chat program

This project is for learning purposes: it's aim is to apply, from theory, multithread socket communication with encryption. The program
works on connecting one or more users to a central server, where they can communicate with each other. To communicate the system uses a login procedure: it
stores the username on the server, which serves as an id, that can be used to redirect the message to the user with that name.

It uses C as a programming language and POSIX's pthread and mutex for multithreading. 
Threads are used 
For the communication between hosts it uses POSIX's socket set to the ipv4 protocol.

#### The encryption
The encryption system is an hybrid assymmetric-symmetric key algorithm; to be more precise it's a RSA-AES hybrid.
It uses RSA encrpytion first communicate the symmetric key, and then it uses the key with the AES algorithm to encrpyt the actual messages.


## Platforms

The program is only tested to work on Ubuntu linux. Other systems need a configuration which has not been implemented yet.

## Dependencies

Dependencies necessary to be installed in to the machine to make the program work:

#### GNU-MP 
For handling arbitrary arithmetic operations I.E 1024 bit values or more (used mainly in RSA encryption)
[here](https://gmplib.org/#DOWNLOAD)

if that doesn't work you can try to install it with this in CLI: 
```
sudo apt-get install libgmp3-dev libgmp10
```

#### AES-encryption
[tiny-AES-c](https://github.com/kokke/tiny-AES-c)

## Installation

To install the program

## Usage



----

## Open source licensing info
1. [TERMS](TERMS.md)
2. [LICENSE](LICENSE)
3. [CFPB Source Code Policy](https://github.com/cfpb/source-code-policy/)

