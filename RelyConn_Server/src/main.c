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
#include "uthash.h"
#include "errormess.h"
#include "strformatting.h"
#include "stdrely_accounting.h"
#include "stdrely_cmd.h"

#define PORT 10003
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
    account_data account;


    int sock;
    struct sockaddr_in address;
    char buffer[BUFFER_LENGTH];
    UT_hash_handle hh;
}client_handle;


socket_local server_listener;
client_handle* clients[MAX_HOSTS];

client_handle* clients_table = NULL;

int clients_connected = 0;

pthread_mutex_t lock_disconnect;

bool is_client_active(client_handle* client)
{
    //printf("check if the client is active: %d\n", !(client == NULL));
    if (client == NULL)
        return false;

    return true;
}

void close_client(client_handle* client)
{
    if (is_client_active(client))
    {
        printf("Close connection: %s", client->account.username);
        //dispose the client: sockets, memory allocation and pointer value
        
        close(client->sock);
        free(client);
    }
}

void* delay_disconnect(void* args)
{
    client_handle* handle_tempo = (client_handle*)args;

    //This delay practically makes the chance of race conditions negligeble
    sleep(1);
    close_client(handle_tempo);
    pthread_exit(NULL);
}

void disconnect_client_index(int index)
{
    pthread_mutex_lock(&lock_disconnect);
    if (is_client_active(clients[index]) )
    {

        //dispose the client: sockets, memory allocation and pointer value
        client_handle* clienthandle_tmp = clients[index];
        printf("disconnected: %s, index %d\n", clienthandle_tmp->account.username, index);

        clients[index] = NULL;
        HASH_DEL(clients_table, clienthandle_tmp);
        pthread_t tid_delaydisconnect;
        pthread_create(&tid_delaydisconnect, NULL, &delay_disconnect, (void*)clienthandle_tmp);

        clients_connected--;
    }
    pthread_mutex_unlock(&lock_disconnect);
}

void disconnect_client(client_handle* client)
{
    for(int i = 0; i < MAX_HOSTS; i++)
    {
        if (clients[i] == client)
        {
            disconnect_client_index(i);
            break;
        }
    }
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
            disconnect_client_index(i);
        }
    }
}

void add_user(client_handle* handle)
{
    HASH_ADD_STR(clients_table, account.username, handle);
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

void* read_clients(void* args)
{
    int i = 0;
    int buffer_size = BUFFER_LENGTH;

    int max_fd = -1;

    struct timeval tv;


    fd_set clients_fdset;
    while(1)
    {
        //if no clients are connected sleep and recheck after 0.2 seconds
        if (clients_connected <= 0)
        {
            sleep(1);
            continue;
        }
        
        //reset work variables
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        max_fd = -1;

        FD_ZERO(&clients_fdset);
        
        for(i = 0; i < MAX_HOSTS; i++)
        {
            if (is_client_active(clients[i]))
            {
                FD_SET(clients[i]->sock, &clients_fdset);        
                if (clients[i]->sock > max_fd)
                    max_fd = clients[i]->sock;
            }
        }

        //wait for any sockets activity
        int activity = select(max_fd + 1, &clients_fdset, NULL, NULL, &tv);
        if (activity < 0 && (errno!=EINTR))
        {
            print_errorno("select pool for reading filedescriptors;");
        }

        //loop through all the active users
        for(i = 0; i < MAX_HOSTS; i++)
        {
            if (is_client_active(clients[i]) && FD_ISSET(clients[i]->sock, &clients_fdset))
            {
                
                //retrieves the message &
                //checks if the data format is correct

                int valread = read(clients[i]->sock, clients[i]->buffer, buffer_size);
                if (valread <= 0)
                {
                    disconnect_client_index(i);
                }
                //message received is not of disconnect
                else
                {
                    //check the format of the message 
                    clients[i]->buffer[valread] = '\0';

                    str_array message_formatted = split_str(clients[i]->buffer, "|");


                    if (message_formatted.length < 2)
                    {
                        printf("The message received from: %s is not formatted correctly! %d should be 2\n", clients[i]->account.username, message_formatted.length);
                        continue;
                    }

                    client_handle* client_dest;
                    char* dest = message_formatted.strs[0];
                    //char* message_payload = message_formatted.strs[1];

                    HASH_FIND_STR(clients_table, dest, client_dest);

                    if (client_dest != NULL)
                    {
                        //as a test it echoes it back
                        send(client_dest->sock, clients[i]->buffer, strlen(clients[i]->buffer), 0);
                        memset(clients[i]->buffer, 0, strlen(clients[i]->buffer));
                        free_str_array(&message_formatted);   
                    }
                    else
                    {
                        printf("There is no such user %s on the server!\n", dest);
                        free_str_array(&message_formatted);
                        continue;
                    }
                }
            }
        }
    }

    printf("TERMINATING THREAD");
    pthread_exit(NULL);
}

void* write_clients(void* args)
{
    char command[128];

    while(true)
    {
        fgets(command, 127, stdin);
        format_linestr(command);   
        
        //check if the command is to kick people
        if(strlen(command) > strlen(CMD_KICK) && strncmp(command, CMD_KICK, strlen(CMD_KICK)) == 0)
        {
            str_array cmd_segments = split_str(command, ":");
            char* name_target = cmd_segments.strs[1];

            client_handle* target;

            HASH_FIND_STR(clients_table, name_target, target);

            if (target != NULL)
            {
                char* message_kick = cmd_segments.strs[2];
                printf("kicking user: %s, reason: %s", target->account.username, message_kick);
                send(target->sock, message_kick, strlen(message_kick), 0);
                disconnect_client(target);
            }

            free_str_array(&cmd_segments);
        }
        else if(strcmp(command, CMD_END) == 0)
        {
            pthread_exit(NULL);
        } 
        memset(command, 0, 127);
    }

    pthread_exit(NULL);
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

    int success_bind = bind(sock, (struct sockaddr*)&address, (socklen_t)sizeof(address));
    if (success_bind < 0)
    {
        fprintf(stderr, "error code: %d; bind: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Initialization of server");

    server_listener.address = address;
    server_listener.sock = sock;
}


void* handle_client_login(void* args)
{
    client_handle* client = (client_handle*)args;
    client_handle* client_cmp;
    bool username_invalid = false;

    char* username;
    int username_length;

    char error[1];
    while(true)
    {
        //resetting data for next check
        username_invalid = false;
        error[0] = SUCCESSFUL_LOGIN;


        int login_length = recv(client->sock, client->buffer, BUFFER_LENGTH, 0);

        if (login_length <= 0)
        {
            close_client(client);
            pthread_exit(NULL);
        }

        printf("received data for login\n **processing**\n");
        str_array login_data = split_str(client->buffer, ";");

        username = login_data.strs[ACCOUNT_USERNAME_INDEX];
        username_length = strlen(username);

        printf("Data retrieved: %s; length of username: %d\n", username, username_length);
        
     
        //making sure the username is valid
        {
            if (username_length < USERNAME_MINLENGTH)
            {
                error[0] = (char)ERROR_ACC_USERNAME_SHORT;
                printf("username too short! Length: %d\n", username_length);
                username_invalid = true;
            }
            else if(username_length > USERNAME_MAXLENGTH)
            {
                error[0] = (char)ERROR_ACC_USERNAME_LONG;
                printf("username too long! Length: %d\n", username_length);
                username_invalid = true;
            }

            if (!username_invalid)
            {
                HASH_FIND_STR(clients_table, login_data.strs[ACCOUNT_USERNAME_INDEX], client_cmp);

                if (client_cmp != NULL)
                {
                    error[0] = (char)ERROR_ACC_USERNAME_EXISTING;
                    printf("\nusername already exists!\n");
                    username_invalid = true;
                }
            }
        }


        if (username_invalid)
        {
            send(client->sock, error, 1, 0);
            continue;
        }
        //login successful
        else
        {
            printf("login of: %s, is successful\n", username);
            send(client->sock, error, 1, 0);
            strcpy(client->account.username, username);
            break;
        } 


        memset(client->buffer, 0, login_length);
        free_str_array(&login_data);
    }

    
    add_user(client);
    pthread_exit(NULL);
}

void* listen_connections(void* args)
{
    printf("Start listening\n");

    int success_listen = listen(server_listener.sock, MAX_HOSTS);

    if (success_listen < 0)
    {
        print_errorno("listening server socket; ");
        pthread_exit(NULL);
    }

    int size_addr = sizeof(server_listener.address);
    while(1)
    {
        client_handle* newConnection = malloc(sizeof(client_handle));
        if ((newConnection->sock = accept(server_listener.sock, 
                (struct sockaddr *)&newConnection->address, (socklen_t*)&size_addr))<0)  
        {  
            perror("accept");
            close_client(newConnection);
        }
        else
        {
            pthread_t tid_login;
            pthread_create(&tid_login, NULL, &handle_client_login, newConnection);
        }
    }
}

int main(int argc, char const* argv[])
{
    pthread_t tid_listener, tid_reader, tid_writer;

    //initialize the server and create the threads for listening for connections and reading the incoming data from the users
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
        fprintf(stderr, "failed thread reader: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    int success_writer = pthread_create(&tid_writer, NULL, &write_clients, NULL);
    if (success_writer < 0)
    {
        fprintf(stderr, "failed thread writer: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_join(tid_writer, NULL);
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