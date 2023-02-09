#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "errormess.h"
#include "strformatting.h"

#define PORT 25547
#define EXIT_CMD "cmd|exit"
#define MESSAGE_MAXLENGTH 128
#define CMD_PREFIX "cmd|"

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

connto_server_handle conn_server_handle;
char buffer[1024] = { 0 };
bool is_terminated = false;

// formatted data sent/received
// {name_src}|{message}
// formatted cmd sent
// cmd|{command}

void* read_conndata(void* args)
{
    readhandle read_handle_inst = *(readhandle*)args;
    connto_server_handle handle = read_handle_inst.conn;

    printf("Start listening: \n");
    while(!is_terminated)
    {
        recv(handle.sock, buffer, sizeof(buffer), 0);

        if (strlen(buffer) >= 0)
        {
            printf("message received: %s\n\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }
    pthread_exit(0);
}

void* write_conndata(void* args)
{
    writehandle write_handle_inst = *(writehandle*)args;
    connto_server_handle handle = write_handle_inst.conn;
    char message[MESSAGE_MAXLENGTH];
    while(!is_terminated)
    {
        printf("message to send: ");
        fgets(message, MESSAGE_MAXLENGTH, stdin);
        format_linestr(message);
        if (strcmp(message, EXIT_CMD) == 0)
        {
            is_terminated = true;           
            printf("\n**Exiting the program**\n");
            pthread_exit(0);
        }
        else
        {
            send(handle.sock, message, strlen(message), 0);
            printf("\n**message sent**\n");
            memset(message, 0, strlen(message));
        }
    }
    pthread_exit(0);
}

int connect_to_server(connto_server_handle* handle, const char* ip)
{
    handle->connected = false;
    int success_connect;


    //create the socket to connect to the server
    //IPV4 with TCP protocol 
    handle->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (handle->sock < 0)
    {
        print_error("Failed socket creation");
        return -1;
    }

    handle->serv_addr.sin_family = AF_INET;
    handle->serv_addr.sin_port = htons(PORT);

    //convert the address and store it to the address holder struct
    int success_convertbin = inet_pton(AF_INET, ip, &handle->serv_addr.sin_addr);
    if (success_convertbin <= 0)
    {
        print_error("Failed convertion address! The address was invalid");
        return -1;
    }

    success_connect = connect(handle->sock, (struct sockaddr*)&handle->serv_addr, sizeof(handle->serv_addr));

    if (success_connect < 0)
    {
        print_error("Connection to specified address failed!");
        return -1;
    }

    handle->connected = true;
    return 0;
}

void disconnect_from_server(connto_server_handle* handle)
{
    close(handle->sock);
}

int main(int argc, char const* argv[])
{
    pthread_t tid_write;
    pthread_t tid_read;

    //establish the connection to the main server
    int connected = connect_to_server(&conn_server_handle, "127.0.0.1");
    if (connected < 0 )
    {
        print_error("connection unsuccessful\n");
    }
    else
    {
        printf("connection successful\n");
    
        readhandle readhandle_inst;
        readhandle_inst.conn = conn_server_handle;
        int succ_read = pthread_create(&tid_read, NULL, &read_conndata, (void*)&readhandle_inst);
        
        if (succ_read < 0)
        {
            print_error("thread read");
            exit(EXIT_FAILURE);
        }

        writehandle writehandle_inst;
        writehandle_inst.conn = conn_server_handle;
        int succ_write = pthread_create(&tid_write, NULL, &write_conndata, (void*)&writehandle_inst);

        
        if (succ_write < 0)
        {
            print_error("thread write");
            exit(EXIT_FAILURE);
        }
        pthread_join(tid_write, NULL);
        //pthread_join(tid_read, NULL);

        disconnect_from_server(&conn_server_handle);
    }
    
    

    printf("\nProgram terminated\n");

    //valread = read(sock, buffer, sizeof(buffer));
}