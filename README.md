# RelyConn chat program

## Overview
This project is for learning purposes: its aim is to apply, from theory, remote communication through sockets, multithreading and encryption.

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
Once downloaded the executables are found in the "build" folder inside the respective client and server directories. 
When a client connects to the server the encryption system is initialized and immediatly after 
that a login procedure is required. This procedure asks for a username which will be used as an id on the server. The id is used then by the others to
communicate with each other.

here's an image that takes as an example the login procedure. There are 2 clients (the two instances on the left)
and 1 server (the process on the right) running in localhost.

![Inizializzazione](https://user-images.githubusercontent.com/74454203/225747125-1a5525cb-8100-45c2-8245-cf3a1909df71.PNG)

The login accepts a username no shorter than 5 characters and no longer than 16 characters. If a username has already been taken a warning will be
sent to the client to change the name.

In order to communicate with other users there is a pattern to respect:
```
username|message
```
where the username has to be a client present on the server.

![Chatting](https://user-images.githubusercontent.com/74454203/225756166-97d9eca8-7f2c-42a8-9a1e-724f3e067654.PNG)

On the server there are several commands the administrator can invoke:
kick
and shut down

the commands follow the respective patterns:
```
server|kick:username:reason
```
```
server|end
```
----

Here is an example of the administrator kicking marco for bad behaviour.

From the server POV

![kicking](https://user-images.githubusercontent.com/74454203/225756882-96fbde1e-5031-46c0-949b-6218f708fafa.PNG)

From Marco's POV

![kicking_clientPOV](https://user-images.githubusercontent.com/74454203/225756970-335b5786-df85-4fd6-b799-00e5fddf69da.PNG)

## Open source licensing info
[LICENSE](LICENSE)

