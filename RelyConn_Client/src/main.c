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

#define PORT 10003
#define MESSAGE_MAXLENGTH 128

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

account_data account;
bool is_terminated = false;

#define REFRESH_LISTENER_RATE 300000

// formatted data sent/received
// {name_src}|{message}
// formatted cmd sent
// cmd|{command}

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

        int valread = recv(handle.sock, buffer, sizeof(buffer), 0);

        if (valread > 0)
        {
            format_usermessage(buffer);
            printf("\nmessage received: %s\n\n", buffer);
            memset(buffer, 0, sizeof(buffer));
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
    char message[MESSAGE_MAXLENGTH];
    while(1)
    {
        fgets(message, MESSAGE_MAXLENGTH, stdin);

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
            send(handle.sock, message, strlen(message), 0);
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

result_check handle_login(char* username)
{
    result_check check;
    if (!conn_server_handle.connected)
    {
        check = get_error("failed to login, server is not connected");
        return check;
    }

    char error[1];

    send(conn_server_handle.sock, account.username, strlen(account.username), 0);
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

        while(true)
        {
            printf("insert the username: ");
            scanf("%s", account.username);

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