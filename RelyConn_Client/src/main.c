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

char* username;


// formatted data sent/received
// {name_src}|{message}
// formatted cmd sent
// cmd|{command}

void* read_conndata(void* args)
{
    readhandle read_handle_inst = *(readhandle*)args;
    connto_server_handle handle = read_handle_inst.conn;

    printf("Start listening: \n");
    while(1)
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
    while(1)
    {
        printf("message to send: ");
        fgets(message, MESSAGE_MAXLENGTH, stdin);
        format_linestr(message);
        if (strcmp(message, EXIT_CMD) == 0)
        {           
            printf("\n**Exiting the server**\n");
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

result_check login(const char* username)
{
    result_check check;
    if (!conn_server_handle.connected)
    {
        check = get_error("failed to login, server is not connected");
        return check;
    }


    char error[1];

    send(conn_server_handle.sock, username, strlen(username), 0);
    recv(conn_server_handle.sock, error, 1, 0);
    if (error[0] > 0)
    {
        check.iserror = 1;
        switch(error[0])
        {
            case 1:
                check.error_mess = "username is too short";
                break;
            case 2:
                check.error_mess = "username is already used";
                break;
            default:
                check.error_mess = "unknown username error";
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

    printf("Insert the username: ");
    scanf("%s", username);


    //establish the connection to the main server
    result_check check_conn = connect_to_server(&conn_server_handle, "127.0.0.1");
    if (check_conn.iserror)
    {
        printf("connection unsuccessful\n");
    }
    else
    {
        printf("connection successful\n");

        result_check check_login = login(username);
        if (check_login.iserror)
        {
            print_error(check_login.error_mess);
            exit(EXIT_FAILURE);
        }

        

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
        pthread_join(tid_write, NULL);
        //pthread_join(tid_read, NULL);

        disconnect_from_server(&conn_server_handle);
    }
    
    

    printf("\nProgram terminated\n");

    //valread = read(sock, buffer, sizeof(buffer));
}