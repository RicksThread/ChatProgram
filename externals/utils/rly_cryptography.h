#ifndef RELY_CRYPTOGRAPHY_H
#define RELY_CRYPTOGRAPHY_H

#include <math.h>
#include "math_utils.h"
#include "aes.h"


void rsa_init_keys(mpz_ptr key_public, mpz_ptr key_private, mpz_ptr n, mp_bitcnt_t key_bitlength);

void rsa_encrypt(mpz_ptr message_dest, mpz_srcptr message_src, mpz_srcptr key_public, mpz_srcptr n);

void rsa_decrypt(mpz_ptr message_dest, mpz_srcptr message_src, mpz_srcptr key_private, mpz_srcptr n);

void buffer_to_mpz(mpz_ptr dest, char* buffer, size_t count);

void mpz_to_buffer(void* buffer, mpz_srcptr src);

#endif