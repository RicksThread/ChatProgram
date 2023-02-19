#ifndef USERS_HANDLER_H
#define USERS_HANDLER_H

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include "stdbool.h"

#include "stdrely_accounting.h"
#include "uthash.h"
#include "stdrely_message.h"
#include "aes_encryption.h"

#define MAX_HOSTS 10

typedef struct
{
    account_data account;
    aes_encryption_data encryption_data;

    int sock;
    struct sockaddr_in address;
    UT_hash_handle hh;
}client_handle;

extern int clients_connected;

extern pthread_mutex_t lock_disconnect;

extern client_handle* clients[MAX_HOSTS];

extern client_handle* clients_table;

bool is_client_active(client_handle* client);

void close_client(client_handle* client);


void disconnect_client_index(int index);

void disconnect_client(client_handle* client);

/*
disconnects all clients and it's thread safe
*/
void disconnect_clients();

void add_user(client_handle* handle);
#endif