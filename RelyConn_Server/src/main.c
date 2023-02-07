#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h> 

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

fd_set listener_set;
int max_fd;

void disconnect_client()
{

}

void read_clients(void* args)
{

    while(1)
    {
        for(int i = 0; i < MAX_HOSTS; i++)
        {
            if (clients[i].active)
            {
                
            }
        }
    }
}

void write_clients(void* args)
{
    int max_fd;

    while(1)
    {
        for(int i = 0; i < MAX_HOSTS; i++)
        {
            if (clients[i].active)
            {

            }
        }
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
            clients[i] = handle;
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


    FD_ZERO(&listener_set);
    FD_SET(server_listener.sock, &listener_set);  

    int size_addr = sizeof(server_listener.address);
    
    while(1)
    {
        int activity = select(max_fd + 1, &listener_set, NULL, NULL, NULL);

        if ((activity < 0))
        {  
            printf("select error");  
        }

        if (FD_ISSET(server_listener.sock, &listener_set))
        {
            if ((newConnection.sock = accept(server_listener.sock, 
                    (struct sockaddr *)&newConnection.address, (socklen_t*)&size_addr))<0)  
            {  
                perror("accept");
                exit(EXIT_FAILURE);
            }

            newConnection.active = true;
            add_user(newConnection);

            if (newConnection.sock > max_fd)
            {
                max_fd = newConnection.sock;
            }
            
            FD_SET(newConnection.sock, &listener_set);
        }
    }

    
    //inform user of socket number - used in send and receive commands 
    printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(newConnection.address.sin_addr) , ntohs(newConnection.address.sin_port));  

    char* message = "sei stato scoccato\n";    
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
