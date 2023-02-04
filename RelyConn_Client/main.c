#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in server_addr;

    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    sock = socket(AF_INET, SOCK_STREAM, 0);

    
}