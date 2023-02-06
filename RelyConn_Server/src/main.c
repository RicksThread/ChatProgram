#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 6969
#define MAX_HOSTS 10

#define BUFFER_LENGTH 1024

#define FAILED_OP -1
#define SUCCESS_OP 0

struct sockaddr_in;
typedef struct
{
    int sock;
    struct sockaddr_in address;
}socket_local;

typedef struct
{
    bool active;
    int sock;
    struct sockaddr_in address;
    char buffer[BUFFER_LENGTH];
}client_handle;

pthread_t tid_read;
pthread_t tid_write;
pthread_t tid_listen;

socket_local server_listener;
client_handle clients[MAX_HOSTS];

void read_clients(void* args)
{
    while(1)
    {

    }
}

void write_clients(void* args)
{
    while(1)
    {
        
    }
}

int init_server()
{
    struct sockaddr_in* address = &server_listener.address;
    int *sock = &server_listener.sock;
    int opt = 1;
    int addrlen = sizeof(*address);

    *sock = socket(AF_INET, SOCK_STREAM, 0);

    if (*sock < 0)
    {
        perror("Socket failed");
        return FAILED_OP;
    }exit(EXIT_FAILURE);

    //uses the ipv4 protocol
    address->sin_family = AF_INET;

    //enables to listen to any IP
    address->sin_addr.s_addr = INADDR_ANY;
    
    //converts from host byte order to network byte order (idk what it is)
    address->sin_port = htons(PORT);

    int success_bind = bind(*sock, (struct sockaddr*)&address, sizeof(address));
    if (success_bind < 0)
    {
        perror("bind has failed");
        return FAILED_OP;
    }

    return SUCCESS_OP;
}

void add_user(client_handle handle)
{
    //add new socket to array of sockets 
    for (int i = 0; i < MAX_HOSTS; i++)  
    {  
        //if position is empty 
        if(!clients[i].active)  
        {  
            clients[i].sock = sock;
            clients[i].active = false;
            printf("Client added to list of sockets as %d\n" , i);  
            break;  
        }  
    }  
}

void listen_connections(void* args)
{
    client_handle newConnection;
    int success_listen = listen(server_listener.sock, MAX_HOSTS);
    int new_socket;

    while(1)
    {
        
    }
    if ((new_socket = accept(server_listener.sock, 
            (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
    {  
        perror("accept");  
        exit(EXIT_FAILURE);  
    }  
             
    //inform user of socket number - used in send and receive commands 
    printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
          (address.sin_port));  
    
    //send new connection greeting message 
    if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
    {  
        perror("send");  
    }  
        
         
}

int main(int argc, char const* argv[])
{
    int success = init_server();
    
    if (success < 0)
    {
        perror("faluire initialization server");
    }

    pthread_create(tid_listen, NULL, &listen_connections, NULL);

    /*
    valread = read(new_socket, buffer, sizeof(buffer));
    printf("%s\n", buffer);


    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    sleep(5);

    close(new_socket);

    */

    
    shutdown(server_listener, SHUT_RDWR);
    
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
