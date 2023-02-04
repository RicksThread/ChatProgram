#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "errormess.h"

#define PORT 6969



int main(int argc, char const* argv[])
{
    int sock = 0;
    int valread;
    int success_connect;

    //create the socket internet address holder
    struct sockaddr_in serv_addr;
    
    //create the message
    char* hello = "Hello from client";

    //create the buffer for receiving the data from the server
    char buffer[1024] = { 0 };

    //create the socket to connect to the server
    //IPV4 with TCP protocol 
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        print_error("Failed socket creation");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //convert the address and store it to the address holder struct
    int success_convertbin = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if (success_convertbin <= 0)
    {
        print_error("Failed convertion address! The address was invalid");
        return -1;
    }

    success_connect = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    if (success_connect < 0)
    {
        print_error("Connection to specified address failed!");
        return -1;
    }

    send(sock, hello, strlen(hello), 0);
    printf("\nHello message sent\n");

    valread = read(sock, buffer, sizeof(buffer));

    close(sock);
}