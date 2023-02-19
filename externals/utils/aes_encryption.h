#ifndef AES_ENCRYPTION_H
#define AES_ENCRYPTION_H

#include <unistd.h>
#include <bits/types.h>
#include <stdlib.h>
#include <stdint.h>
#include "aes.h"

#define AES_KEY_LENGTH 8
#define AES_IV_LENGTH 16

typedef struct
{
    uint8_t key[AES_KEY_LENGTH];
    uint8_t iv[AES_IV_LENGTH];
    struct AES_ctx aes_data;
}aes_encryption_data;

void aes_init(aes_encryption_data* encryption_data, uint8_t* key, uint8_t* iv);

void aes_cbc_encrypt(aes_encryption_data* encryption_data, void* buffer, size_t size);

void aes_cbc_decrypt(aes_encryption_data* decryption_data, void* buffer, size_t size);

#endif