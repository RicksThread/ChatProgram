#include <unistd.h>
#include <bits/types.h>
#include <stdlib.h>

#define DES_KEY_LENGTH 64
#define DES_IV_LENGTH 32

typedef struct
{
    u_int8_t key[DES_KEY_LENGTH];
    u_int8_t iv[DES_IV_LENGTH];
}des_encryption_data;