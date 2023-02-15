#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h> 
#include <string.h>
#include <errno.h>
#include "uthash.h"
#include "errormess.h"
#include "strformatting.h"
#include "stdrely_cmd.h"
#include "user_handler.h"
#include "rly_cryptography.h"
#include "des_encryption_data.h"
#include <gmp.h>

#define PORT 10003

#ifndef NULL
#define NULL 0
#endif

#define RSA_KEY_LENGTH 64

struct sockaddr_in;


typedef struct
{
    int sock;
    struct sockaddr_in address;
}socket_local;


socket_local server_listener;
pthread_t tid_listener, tid_reader, tid_writer;

bool is_terminated = false;


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
                    char* message_payload = message_formatted.strs[1];

                    HASH_FIND_STR(clients_table, dest, client_dest);

                    if (client_dest != NULL)
                    {
                        char message_tosend[MESSAGE_USER_LENGTH];
                        sprintf(message_tosend, "%s|%s", clients[i]->account.username, message_payload);

                        send(client_dest->sock, message_tosend, strlen(message_tosend), 0);
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
                char* reason_kick = cmd_segments.strs[2];
                char message_kick[150];
                sprintf(message_kick, "%s%s", PREFIX_KICK, reason_kick);
                printf("\nkicking user: %s, reason: %s\n", target->account.username, reason_kick);
                send(target->sock, message_kick, strlen(message_kick), 0);
                disconnect_client(target);

            }

            free_str_array(&cmd_segments);
        }
        else if(strcmp(command, CMD_END) == 0)
        {
            is_terminated = true;
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

void handle_login(client_handle* client)
{
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

void init_encryption_client(client_handle* client)
{
    //rsa from server
    //pubic key to client
    //client creates the key for the SEA-DES-CBC
    mpz_t key_public, key_private, n;
    rsa_init_keys(key_public, key_private, n, RSA_KEY_LENGTH*8);

    char public_key_buffer[RSA_KEY_LENGTH];

    mpz_to_buffer(public_key_buffer, key_public);

    send(client->sock, public_key_buffer, RSA_KEY_LENGTH, 0);

    des_encryption_data* des_data;

    char encrypted_key_buffer[RSA_KEY_LENGTH];

    //receive the des key from the client
    recv(client->sock, encrypted_key_buffer, RSA_KEY_LENGTH, 0);
    
    mpz_t encrypted_des_key_mpz, decrypted_des_key_mpz;
    mpz_init(encrypted_des_key_mpz);
    mpz_init(decrypted_des_key_mpz);

    //convert to mpz the buffer and then decrypt it; finally put the data into a buffer
    buffer_to_mpz(encrypted_des_key_mpz, encrypted_key_buffer, RSA_KEY_LENGTH);


    rsa_decrypt(decrypted_des_key_mpz, encrypted_des_key_mpz, key_private, n);
    mpz_to_buffer(des_data->key, decrypted_des_key_mpz);
}

void* init_client_conn(void* args)
{
    client_handle* client = (client_handle*)args;


    init_encryption_client(client);
    handle_login(client);
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
            pthread_create(&tid_login, NULL, &init_client_conn, newConnection);
        }
    }
}

int main(int argc, char const* argv[])
{

    //initialize the server and create the threads for listening for connections and reading the incoming data from the users
    init_server();
    printf("\nserver initialized\n");

    pthread_mutex_init(&lock_disconnect, NULL);

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

    while(!is_terminated) { sleep(0.5);}

    pthread_cancel(tid_listener);
    pthread_cancel(tid_reader);
    pthread_cancel(tid_writer);

    disconnect_clients();

    pthread_mutex_destroy(&lock_disconnect);
    shutdown(server_listener.sock, SHUT_RDWR);
    
    
    return 0;
}