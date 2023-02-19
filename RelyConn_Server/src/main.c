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
#include "aes_encryption.h"
#include "aes.h"
#include <gmp.h>
#include "buffer_utils.h"

#define PORT 10004

#define RSA_KEY_LENGTH 32

struct sockaddr_in;


typedef struct
{
    int sock;
    struct sockaddr_in address;
}socket_local;

char buffer[BUFFER_LENGTH];
socket_local server_listener;
pthread_t tid_listener, tid_reader, tid_writer;

bool is_terminated = false;

uint8_t iv_relyConn[16]  = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

void print_buffer(char* buffer, size_t size)
{
    for(int i = 0; i < size; i++)
    {
        printf("%c", buffer[i]);
    }
}

void* read_clients(void* args)
{

    int i = 0;

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

                memset(buffer, 0, BUFFER_LENGTH);
                int valread = recv(clients[i]->sock, buffer, BUFFER_LENGTH, 0);
                
                
                if (valread <= 0)
                {
                    disconnect_client_index(i);
                }
                //message received is not of disconnect
                else
                {
                    
                    //check the format of the message
                    aes_cbc_decrypt(&clients[i]->encryption_data, buffer, BUFFER_LENGTH);
                    //buffer[valread] = '\0';
                    str_array message_formatted = split_str(buffer, "|");

                    if (message_formatted.length < 2)
                    {
                        printf("The message received from: %s is not formatted correctly! %d should be 2\n", clients[i]->account.username, message_formatted.length);
                        
                        free_str_array(&message_formatted);
                        continue;
                    }

                    client_handle* client_dest;
                    char* dest = message_formatted.strs[0];
                    char* message_payload = message_formatted.strs[1];

                    HASH_FIND_STR(clients_table, dest, client_dest);

                    printf("The user %s has sent the message: %s\n", dest, message_payload);
                    if (client_dest != NULL)
                    {
                        char message_tosend[BUFFER_LENGTH];
                        sprintf(message_tosend, "%s|%s", clients[i]->account.username, message_payload);

                        aes_cbc_encrypt(&client_dest->encryption_data, message_tosend, BUFFER_LENGTH);
                        send(client_dest->sock, message_tosend, BUFFER_LENGTH, 0);


                    }
                    else
                    {
                        printf("There is no such user %s on the server!\n", dest);
                    }
                    free_str_array(&message_formatted);
                    
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
            if (is_str_array_empty(&cmd_segments))
            {
                free_str_array(&cmd_segments);
                continue;
            }

            char* name_target = cmd_segments.strs[1];

            client_handle* target;

            HASH_FIND_STR(clients_table, name_target, target);

            if (target != NULL)
            {
                char* reason_kick = cmd_segments.strs[2];
                char message_kick[BUFFER_LENGTH];
                sprintf(message_kick, "%s%s", PREFIX_KICK, reason_kick);

                printf("\nkicking user: %s, reason: %s\n", target->account.username, reason_kick);
                aes_cbc_encrypt(&target->encryption_data, message_kick, BUFFER_LENGTH);
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
        memset(command, 0, 128);
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
    bool login = true;
    while(login)
    {
        //resetting data for next check
        username_invalid = false;
        error[0] = SUCCESSFUL_LOGIN;

        memset(buffer, 0, BUFFER_LENGTH);
        int login_length = recv(client->sock, buffer, BUFFER_LENGTH, 0);
        aes_cbc_decrypt(&client->encryption_data, buffer, BUFFER_LENGTH);

        if (login_length <= 0)
        {
            close_client(client);
            pthread_exit(NULL);
        }

        printf("received data for login\n **processing**\n");
        str_array login_data = split_str(buffer, ";");
        
        if (is_str_array_empty(&login_data))
        {
            free_str_array(&login_data);
            error[0] = (char)ERROR_ACC_USERNAME_SHORT;
            send(client->sock, error, 1, 0);
            continue;
        }
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
            
        }
        //login successful
        else
        {
            printf("login of: %s, is successful\n", username);
            send(client->sock, error, 1, 0);
            strcpy(client->account.username, username);
            login = false;
        } 

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
    
    //initialize the rsa keys in mpz_t type
    mpz_t key_public, key_private, n;
    rsa_init_keys(key_public, key_private, n, RSA_KEY_LENGTH * 8);
    
    //buffer the public key and n; and send them to the client

    size_t key_public_bytesz = mpz_sizeinbase(key_public, 256);
    size_t n_bytesz = mpz_sizeinbase(n, 256);
    
    memset(buffer, 0, BUFFER_LENGTH);
    mpz_to_buffer(buffer, key_public);
    
    
    set_buffer_header(buffer, BUFFER_LENGTH, (buffer_header){.message_length = key_public_bytesz});
    send(client->sock, buffer, BUFFER_LENGTH, 0);

    memset(buffer, 0, BUFFER_LENGTH);
    mpz_to_buffer(buffer, n);
    set_buffer_header(buffer, BUFFER_LENGTH, (buffer_header){.message_length = n_bytesz});
    send(client->sock, buffer, BUFFER_LENGTH, 0);
    



    //receive the des key from the client
    memset(buffer, 0, BUFFER_LENGTH);
    recv(client->sock, buffer, BUFFER_LENGTH, 0);
    buffer_header aes_key_encrypted_header = extract_buffer_header(buffer, BUFFER_LENGTH);

    //decrypt the keys
    mpz_t encrypted_des_key_mpz, decrypted_des_key_mpz;
    
    mpz_init(encrypted_des_key_mpz);
    mpz_init(decrypted_des_key_mpz);

    //convert to mpz the buffer and then decrypt it; finally put the data into the client data

    buffer_to_mpz(encrypted_des_key_mpz, buffer, aes_key_encrypted_header.message_length);
    rsa_decrypt(decrypted_des_key_mpz, encrypted_des_key_mpz, key_private, n);



    memset(buffer, 0, BUFFER_LENGTH);
    mpz_to_buffer(buffer, decrypted_des_key_mpz);

    aes_encryption_data des_data;
    
    aes_init(&des_data, buffer, iv_relyConn);
    client->encryption_data = des_data;

    //clear the memory
    mpz_clear(key_public);
    mpz_clear(key_private);
    mpz_clear(n);
    mpz_clear(encrypted_des_key_mpz);
    mpz_clear(decrypted_des_key_mpz);
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

    //initialize the server and create the threads for listening for connections and reading the incoming data from keythe users
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

/*
void init_encryption_client(client_handle* client)
{

    //rsa from server
    //pubic key to client
    //client creates the key for the SEA-DES-CBC
    
    //initialize the rsa keys in mpz_t type
    mpz_t key_public, key_private, n;
    rsa_init_keys(key_public, key_private, n, RSA_KEY_LENGTH * 8);
    
    //buffer the public key and n; and send them to the client

    size_t key_public_bytesz = mpz_sizeinbase(key_public, 256);
    size_t n_bytesz = mpz_sizeinbase(n, 256);
    memset(buffer, 0, BUFFER_LENGTH);
    

    mpz_to_buffer(buffer, key_public);
    memcpy(&buffer[BUFFER_LENGTH-sizeof(size_t)-1], &key_public_bytesz, sizeof(size_t));
    send(client->sock, buffer, BUFFER_LENGTH, 0);

    memset(buffer, 0, BUFFER_LENGTH);
    mpz_to_buffer(buffer, n);
    memcpy(&buffer[BUFFER_LENGTH-sizeof(size_t)-1], &n_bytesz, sizeof(size_t));
    send(client->sock, buffer, BUFFER_LENGTH, 0);
    
    //send the private key just of testing

    
    send(client->sock, buffer,)

    char encrypted_key_buffer[RSA_KEY_LENGTH];
    char decrypted_des_key_buffer[AES_KEY_LENGTH];
    
    //receive the des key from the client
    recv(client->sock, encrypted_key_buffer, n_bytesz, 0);

    //decrypt the keys
    mpz_t encrypted_des_key_mpz, decrypted_des_key_mpz;
    
    mpz_init(encrypted_des_key_mpz);
    mpz_init(decrypted_des_key_mpz);

    //convert to mpz the buffer and then decrypt it; finally put the data into the client data
    buffer_to_mpz(encrypted_des_key_mpz, encrypted_key_buffer, n_bytesz);
    gmp_printf("The encrypted key is: %Zd\n", encrypted_des_key_mpz);
    rsa_decrypt(decrypted_des_key_mpz, encrypted_des_key_mpz, key_private, n);
    gmp_printf("The decrypted key is: %Zd\n", decrypted_des_key_mpz);
    mpz_to_buffer(decrypted_des_key_buffer, decrypted_des_key_mpz);

    aes_encryption_data des_data;
    
    aes_init(&des_data, decrypted_des_key_buffer, iv_relyConn);
    client->encryption_data = des_data;
    
    //clear the memory
    mpz_clear(key_public);
    mpz_clear(key_private);
    mpz_clear(n);
    mpz_clear(encrypted_des_key_mpz);
    mpz_clear(decrypted_des_key_mpz);


*/