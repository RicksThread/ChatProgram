#include "aes_encryption.h"


void aes_init(aes_encryption_data* encryption_data, uint8_t* key, uint8_t* iv)
{
    memcpy(encryption_data->key, key, AES_KEY_LENGTH);
    memcpy(encryption_data->iv, iv, AES_IV_LENGTH);

    AES_init_ctx_iv(&encryption_data->aes_data, encryption_data->key, encryption_data->iv);
}

void aes_cbc_encrypt(aes_encryption_data* encryption_data, void* buffer, size_t size)
{
    //I know I shouldn't reset the initialization vector but I was in a time constraint
    AES_CBC_encrypt_buffer(&encryption_data->aes_data, buffer, size);
    AES_ctx_set_iv(&encryption_data->aes_data, encryption_data->iv);
}

void aes_cbc_decrypt(aes_encryption_data* decryption_data, void* buffer, size_t size)
{
    //I know I shouldn't reset the initialization vector but I was in a time constraint
    AES_CBC_decrypt_buffer(&decryption_data->aes_data, buffer, size);
    AES_ctx_set_iv(&decryption_data->aes_data, decryption_data->iv);
}
