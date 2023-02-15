#include "rly_cryptography.h"

void rsa_init_keys(mpz_ptr key_public, mpz_ptr key_private, mpz_ptr n, mp_bitcnt_t key_bitlength)
{
    mpz_init2(key_public, key_bitlength);
    mpz_init2(key_private, key_bitlength);

    key_bitlength += key_bitlength%2;
    mp_bitcnt_t pqlength = key_bitlength*0.5;
    //initialize the fields
    mpz_t p, q;

    mpz_init2(p, pqlength);
    mpz_init2(q, pqlength);

    mpz_t pminus, qminus;
    mpz_t v;

    mpz_init2(n, key_bitlength);
    mpz_init2(key_private, key_bitlength);
    mpz_init2(key_public, key_bitlength);

    //get the prime numbers p,q and npriv (You don't have to get a prime for the latter, but it still works as a coprime of V)
    bn_getprimeb_random(q, pqlength);
    bn_getprimeb_random(p, pqlength);
    mpz_mul(n, p, q);


    //calculate V = (P-1)*(Q-1)
    mpz_init_set(pminus, p);
    mpz_sub_ui(pminus, pminus, 1);

    mpz_init_set(qminus, q);
    mpz_sub_ui(qminus, qminus, 1);

    mpz_init(v);
    mpz_mul(v, pminus, qminus);

    
    bn_getprimeb_random(key_private, key_bitlength-1);

    //get the modular inverse
    mpz_invert(key_public, key_private, v);

    mpz_clear(p); mpz_clear(q), mpz_clear(v), mpz_clear(pminus), mpz_clear(qminus);
}

void rsa_encrypt(mpz_ptr message_dest, mpz_srcptr message_src, mpz_srcptr key_public, mpz_srcptr n)
{
    mpz_powm(message_dest, message_src, key_public, n);
}

void rsa_decrypt(mpz_ptr message_dest, mpz_srcptr message_src, mpz_srcptr key_private, mpz_srcptr n)
{
    mpz_powm(message_dest, message_src, key_private, n);
}

void buffer_to_mpz(mpz_ptr dest, char* buffer, size_t count)
{
    
    mpz_import (dest, count, 1, sizeof(char), 0, 0, buffer);
}

void mpz_to_buffer(void* buffer, mpz_srcptr src)
{
    mpz_export(buffer, 0, 1, sizeof(char), 0, 0, src);
}
