#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 6969
#define MAX_HOSTS 10

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    
    char buffer[1024] = {0};
    char* hello = "Hello from server";
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int success_setopt = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); 
    if (success_setopt < 0)
    {
        perror("setsocketopt");
        exit(EXIT_FAILURE);
    }

    //
    //uses the ipv4 protocol
    address.sin_family = AF_INET;

    //enables to listen to any IP
    address.sin_addr.s_addr = INADDR_ANY;
    
    //converts from host byte order to network byte order (idk what it is)
    address.sin_port = htons(PORT);

    int success_bind = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (success_bind < 0)
    {
        perror("bind has failed");
        exit(EXIT_FAILURE);
    }

    int success_listen = listen(server_fd, MAX_HOSTS);
    if (success_listen < 0)
    {
        perror("accept faluire");
        exit(EXIT_FAILURE);
    }

    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0)
    {
        perror("failed to accept ");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, sizeof(buffer));
    pritnf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    pritnf("Hello message sent\n");

    close(new_socket);


    
    shutdown(server_fd, SHUT_RDWR);
    
    return 0;
}


/*
asincrono e sincrono

sincorno e' quando gli eventi sono accordati su una base temporale

asincrono è quando c'è una mancanza di sincronizzazione

fourier
shannon
conversione analogico digitale
quantizzazione

*/
