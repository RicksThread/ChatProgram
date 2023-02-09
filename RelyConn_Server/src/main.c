#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h> 
#include <string.h>
#include <errno.h>
#include "../../externals/uthash_lib/src/uthash.h"

#define PORT 25546
#define MAX_HOSTS 10

#define BUFFER_LENGTH 1024

#define FAILED_OP -1
#define SUCCESS_OP 0

#ifndef NULL
#define NULL 0
#endif

struct sockaddr_in;
typedef struct
{
    int sock;
    struct sockaddr_in address;
}socket_local;

typedef struct
{
    int sock;
    struct sockaddr_in address;
    char buffer[BUFFER_LENGTH];
    UT_hash_handle hh;
}client_handle;


socket_local server_listener;
client_handle* clients[MAX_HOSTS];
int clients_connected = 0;

pthread_mutex_t lock_disconnect;

bool is_client_active(client_handle* client)
{
    //printf("check if the client is active: %d\n", !(client == NULL));
    if (client == NULL)
        return false;

    return true;
}

void* delay_disconnect(void* args)
{
    client_handle* handle_tempo = (client_handle*)args;

    //This delay practically makes the chance of race conditions negligeble
    sleep(1);
    close(handle_tempo->sock);
    free(handle_tempo);
    pthread_exit(NULL);
}

void disconnect_client(int index)
{
    pthread_mutex_lock(&lock_disconnect);
    if (is_client_active(clients[index]) )
    {
        printf("Someone disconnected index: %d\n", index);

        //dispose the client: sockets, memory allocation and pointer value
        client_handle* clienthandle_tmp = clients[index];
        clients[index] = NULL;
        
        pthread_t tid_delaydisconnect;
        pthread_create(&tid_delaydisconnect, NULL, &delay_disconnect, (void*)clienthandle_tmp);

        clients_connected--;
    }
    pthread_mutex_unlock(&lock_disconnect);
}

/*
disconnects all clients and it's thread safe
*/
void disconnect_clients()
{
    for(int i = 0; i < MAX_HOSTS; i++)
    {
        if(is_client_active(clients[i]))
        {
            disconnect_client(i);
        }
    }
}

void* read_clients(void* args)
{
    int buffer_size = BUFFER_LENGTH;

    while(1)
    {

        //loop through all the players
        for(int i = 0; i < MAX_HOSTS; i++)
        {
            if (is_client_active(clients[i]))
            {
                
                    //retrieves the message &
                    //checks if the data format is correct

                    int valread = read(clients[i]->sock, clients[i]->buffer, buffer_size);
                    if (valread <= 0)
                    {
                        //client has disconnected
                        disconnect_client(i);
                    }
                    else
                    {
                        //as a test it echoes it back
                        send(clients[i]->sock, clients[i]->buffer, strlen(clients[i]->buffer), 0);
                        memset(clients[i]->buffer, 0, buffer_size);
                    }
                    
                
            }
        }
    }
}

void write_clients(void* args)
{
    while(true)
    {
    }
}

void init_server()
{
    struct sockaddr_in address;
    int sock;
    int opt = 1;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("\nSocket failed\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("set socket opt: ");
        exit(EXIT_FAILURE);
    }
    //uses the ipv4 protocol
    address.sin_family = AF_INET;

    //enables to listen to any IP
    address.sin_addr.s_addr = INADDR_ANY;
    
    //converts from host byte order to network byte order (idk what it is)
    address.sin_port = htons(PORT);

    int success_bind = bind(sock, (struct sockaddr*)&address, (struct socklen_t*)sizeof(address));
    if (success_bind < 0)
    {
        fprintf(stderr, "error code: %d; bind: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Initialization of server");

    server_listener.address = address;
    server_listener.sock = sock;
    return SUCCESS_OP;
}

void add_user(client_handle* handle)
{
    //add new socket to array of sockets 
    for (int i = 0; i < MAX_HOSTS; i++)  
    {  
        //if position is empty 
        if(!is_client_active(clients[i]))  
        {  
            printf("Client added to list of sockets as %d\n" , i);  

            clients[i] = handle;
            clients_connected++;
            break;  
        }  
    }  
}

void* listen_connections(void* args)
{
    printf("Start listening\n");

    int success_listen = listen(server_listener.sock, MAX_HOSTS);

    int size_addr = sizeof(server_listener.address);
    while(1)
    {
        client_handle* newConnection = malloc(sizeof(client_handle));
        if ((newConnection->sock = accept(server_listener.sock, 
                (struct sockaddr *)&newConnection->address, (socklen_t*)&size_addr))<0)  
        {  
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            add_user(newConnection);
            char* message = "sis\n";
            send(newConnection->sock, message, strlen(message), 0);
        }
    }
    
}

int main(int argc, char const* argv[])
{
    pthread_t tid_listener, tid_reader;


    init_server();
    printf("\nserver initialized\n");
    int success_listener = pthread_create(&tid_listener, NULL, &listen_connections, NULL);
    
    if (success_listener <0 )
    {
        fprintf(stderr, "failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    
    int success_reader = pthread_create(&tid_reader, NULL, &read_clients, NULL);
    if (success_reader <0 )
    {
        fprintf(stderr, "failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    int num;
    printf("insert something to exit the program:\n");
    scanf("%d", &num);

    disconnect_clients();
    
    /*
    valread = read(new_socket, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    close(new_socket);
    
    */

    
    shutdown(server_listener.sock, SHUT_RDWR);
    
    
    return 0;
}