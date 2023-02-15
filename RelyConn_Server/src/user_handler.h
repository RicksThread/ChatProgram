#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include "stdbool.h"

#include "stdrely_accounting.h"
#include "uthash.h"
#include "stdrely_message.h"
#include "des_encryption_data.h"

#define MAX_HOSTS 10

typedef struct
{
    account_data account;
    des_encryption_data encryption_data;

    int sock;
    struct sockaddr_in address;
    char buffer[BUFFER_LENGTH];
    UT_hash_handle hh;
}client_handle;

int clients_connected = 0;

pthread_mutex_t lock_disconnect;

client_handle* clients[MAX_HOSTS];

client_handle* clients_table = NULL;

bool is_client_active(client_handle* client);

void close_client(client_handle* client);


void disconnect_client_index(int index);

void disconnect_client(client_handle* client);

/*
disconnects all clients and it's thread safe
*/
void disconnect_clients();

void add_user(client_handle* handle);