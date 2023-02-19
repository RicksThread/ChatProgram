#include "user_handler.h"

int clients_connected = 0;

pthread_mutex_t lock_disconnect;

client_handle* clients[MAX_HOSTS];
client_handle* clients_table = NULL;

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
        printf("Close connection: %s\n", client->account.username);
        //dispose the client: sockets, memory allocation and pointer value
        
        close(client->sock);
        free(client);
    }
}


void disconnect_client_index(int index)
{
    pthread_mutex_lock(&lock_disconnect);
    if (is_client_active(clients[index]) )
    {

        //dispose the client: sockets, memory allocation and pointer value
        client_handle* clienthandle_tmp = clients[index];
        printf("\ndisconnected: %s, index %d\n\n", clienthandle_tmp->account.username, index);

        clients[index] = NULL;
        HASH_DEL(clients_table, clienthandle_tmp);
        close_client(clienthandle_tmp);

        clients_connected--;
    }
    pthread_mutex_unlock(&lock_disconnect);

}

void disconnect_client(client_handle* client)
{
    printf("disconneting client: %s\n", client->account.username);
    for(int i = 0; i < MAX_HOSTS; i++)
    {
        printf("iterating clients: %d; is same = %d\n", i, clients[i] == client);
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
