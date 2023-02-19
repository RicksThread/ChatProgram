#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

#include "errormess.h"
#include "strformatting.h"
#include "error_handling.h"
#include "stdrely_cmd.h"
#include "stdrely_accounting.h"
#include "formatter_message.h"
#include "rly_cryptography.h"
#include "aes_encryption.h"
#include "aes.h"
#include "buffer_utils.h"

#define PORT 10004
#define MESSAGE_MAXLENGTH 128

#define RSA_KEY_LENGTH 32

typedef struct
{
    struct sockaddr_in serv_addr;
    bool connected;
    int sock;
}connto_server_handle;

typedef struct
{
    connto_server_handle conn;
}writehandle;

typedef struct
{
    connto_server_handle conn;
}readhandle;

uint8_t key_des[AES_KEY_LENGTH];
uint8_t iv_relyConn[16]  = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

connto_server_handle conn_server_handle;
char buffer[1024] = { 0 };

account_data account;
bool is_terminated = false;
aes_encryption_data aes_data;

#define REFRESH_LISTENER_RATE 300000

// formatted data sent/received
// {name_src}|{message}
// formatted cmd sent
// cmd|{command}

void clear_buffer()
{
    memset(buffer, 0, BUFFER_LENGTH);
}

void print_buffer(char* buffer, size_t size)
{
    for(int i = 0; i < size; i++)
    {
        printf("%c", buffer[i]);
    }
}


void* read_conndata(void* args)
{
    readhandle read_handle_inst = *(readhandle*)args;
    connto_server_handle handle = read_handle_inst.conn;



    fd_set listener_fd;
    struct timeval tv;

    printf("Start listening: \n");
    while(1)
    {
        //reset work variables
        tv.tv_sec = 0;
        tv.tv_usec = REFRESH_LISTENER_RATE;

        FD_ZERO(&listener_fd);
        FD_SET(conn_server_handle.sock, &listener_fd);

        //wait for any sockets activity
        int activity = select(conn_server_handle.sock + 1, &listener_fd, NULL, NULL, &tv);
        if (activity < 0 && (errno!=EINTR))
        {
            print_errorno("select pool for reading filedescriptors;");
        }

        memset(buffer, 0, BUFFER_LENGTH);
        int valread = recv(handle.sock, buffer, BUFFER_LENGTH, 0);
        aes_cbc_decrypt(&aes_data, buffer, BUFFER_LENGTH);

        if (valread > 0)
        {
            printf("\nmessage received: %s\n\n", buffer);
        }
        else
        {
            printf("Disconnected from server!\n");
            is_terminated = true;
            break;
        }
    }
    pthread_exit(0);
}

void* write_conndata(void* args)
{
    printf("CHAT CREATED!\n");
    writehandle write_handle_inst = *(writehandle*)args;
    connto_server_handle handle = write_handle_inst.conn;
    char message[BUFFER_LENGTH];
    while(1)
    {
        fgets(message, BUFFER_LENGTH, stdin);

        //remove the /n from the message to compare it to other strings without to much hassle
        format_linestr(message);
        if (strcmp(message, CMD_EXIT) == 0)
        {           
            printf("\n**Exiting the server**\n");
            is_terminated = true;
            pthread_exit(0);
        }
        else
        {
            aes_cbc_encrypt(&aes_data, message, BUFFER_LENGTH);
            send(handle.sock, message, BUFFER_LENGTH, 0);
            memset(message, 0, BUFFER_LENGTH);
        }
    }
    pthread_exit(0);
}

result_check connect_to_server(connto_server_handle* handle, const char* ip)
{
    result_check check;
    handle->connected = false;
    int success_connect;

    //create the socket to connect to the server
    //IPV4 with TCP protocol 
    handle->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (handle->sock < 0)
    {
        check = get_error_sys("socket creation: ");
        return check;
    }

    handle->serv_addr.sin_family = AF_INET;
    handle->serv_addr.sin_port = htons(PORT);

    //convert the address and store it to the address holder struct
    int success_convertbin = inet_pton(AF_INET, ip, &handle->serv_addr.sin_addr);
    if (success_convertbin <= 0)
    {
        check = get_error_sys("Failed convertion address! The address was invalid; ");
        return check;
    }

    success_connect = connect(handle->sock, (struct sockaddr*)&handle->serv_addr, sizeof(handle->serv_addr));

    if (success_connect < 0)
    {
        check = get_error_sys("Connection to specified address failed! ");
        return check;
    }

    handle->connected = true;
    check.iserror = 0;
    return check;
}

void disconnect_from_server(connto_server_handle* handle)
{
    close(handle->sock);
}

void encryption_system_inizialization(connto_server_handle* handle)
{
    printf("Initialize encryption\n");
    mpz_t key_public_rsa, n;
    mpz_init2(key_public_rsa, RSA_KEY_LENGTH * 8);
    mpz_init2(n, RSA_KEY_LENGTH * 8);

    //receive the public aes_key_mpz
    clear_buffer();
    recv(handle->sock, buffer, BUFFER_LENGTH, 0);

    //get the byte size of the public aes_key_mpz for transforming it in mpz format
    buffer_header key_public_header = extract_buffer_header(buffer, BUFFER_LENGTH);
    buffer_to_mpz(key_public_rsa, buffer, key_public_header.message_length);
    

    //receive the n number
    clear_buffer();
    recv(handle->sock, buffer, BUFFER_LENGTH, 0);

    buffer_header n_header = extract_buffer_header(buffer, BUFFER_LENGTH);
    buffer_to_mpz(n, buffer, n_header.message_length);


    //create the random aes_key_mpz for des
    mpz_t aes_key_mpz;
    mpz_init(aes_key_mpz);
    bn_get_random(aes_key_mpz, AES_KEY_LENGTH * 8);

    //convert to a buffer and store it
    mpz_to_buffer(key_des, aes_key_mpz);

    //encrypt it and put the encrypted key into a buffer to be sent
    mpz_t encrypted_key;
    mpz_init(encrypted_key);
    rsa_encrypt(encrypted_key, aes_key_mpz, key_public_rsa, n);



    clear_buffer();
    mpz_to_buffer(buffer, encrypted_key);
    size_t aes_key_encrypted_bytesz = mpz_sizeinbase(encrypted_key, 256);
    set_buffer_header(buffer, BUFFER_LENGTH, (buffer_header){.message_length = aes_key_encrypted_bytesz});
    
    send(handle->sock, buffer, BUFFER_LENGTH, 0);

    aes_init(&aes_data, key_des, iv_relyConn);
    


    mpz_clear(key_public_rsa);
    mpz_clear(n);
    mpz_clear(aes_key_mpz);
    mpz_clear(encrypted_key);
}

result_check handle_login(char* username)
{
    result_check check;
    if (!conn_server_handle.connected)
    {
        check = get_error("failed to login, server is not connected");
        return check;
    }

    char error[1];

    char username_encrypted[32];
    memcpy(username_encrypted, username, USERNAME_MAXLENGTH);
    aes_cbc_encrypt(&aes_data, username_encrypted, 32);
    
    send(conn_server_handle.sock, username_encrypted, 32, 0);

    recv(conn_server_handle.sock, error, 1, 0);

    if ((int)error[0] != SUCCESSFUL_LOGIN)
    {
        check.iserror = 1;
        switch((int)error[0])
        {
            case ERROR_ACC_USERNAME_EXISTING:
                check.error_mess = "LOGIN FAILED! Username is already used";
                break;
            case ERROR_ACC_USERNAME_LONG:
                check.error_mess = "LOGIN FAILED! Username is too long";
                break;
            case ERROR_ACC_USERNAME_SHORT:
                check.error_mess = "LOGIN FAILED! Username is too short";
                break;
            default:
                check.error_mess = "LOGIN FAILED! Unknown username error";
                break;
        }
        return check;
    }

    check.iserror = 0;
    return check;
}

int main(int argc, char const* argv[])
{
    pthread_t tid_write;
    pthread_t tid_read;

    //establish the connection to the main server

    result_check check_conn = connect_to_server(&conn_server_handle, "127.0.0.1");
        
    if (check_conn.iserror)
    {
        printf("connection unsuccessful\n");
    }
    else
    {
        printf("connection successful to main server\n");

        printf("\n\n=======[LOGIN]======\n\n");


        encryption_system_inizialization(&conn_server_handle);
        

        while(true)
        {
            printf("insert the username: ");
            fgets(account.username, USERNAME_MAXLENGTH, stdin);
            format_linestr(account.username);
            result_check check_login = handle_login(account.username);

            if (check_login.iserror)
                print_error(check_login.error_mess);
            else
                break;
        }
        printf("\n\nLOGIN SUCCESSFUL!\n\n");
        
        readhandle readhandle_inst;
        readhandle_inst.conn = conn_server_handle;
        int succ_read = pthread_create(&tid_read, NULL, &read_conndata, (void*)&readhandle_inst);
        
        if (succ_read < 0)
        {
            print_errorno("thread read");
            exit(EXIT_FAILURE);
        }

        writehandle writehandle_inst;
        writehandle_inst.conn = conn_server_handle;
        int succ_write = pthread_create(&tid_write, NULL, &write_conndata, (void*)&writehandle_inst);

        
        if (succ_write < 0)
        {
            print_errorno("thread write");
            exit(EXIT_FAILURE);
        }
        while(!is_terminated) { sleep(0.5); }

        disconnect_from_server(&conn_server_handle);
    }
    
    

    printf("\nProgram terminated\n");

    //valread = read(sock, buffer, sizeof(buffer));
}