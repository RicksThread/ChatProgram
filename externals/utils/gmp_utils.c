#include "gmp_utils.h"

void buffer_to_mpz(mpz_ptr dest, char* buffer, size_t count)
{
    
    mpz_import (dest, count, 1, sizeof(char), 0, 0, buffer);
}

void mpz_to_buffer(void* buffer, mpz_srcptr src)
{
    mpz_export(buffer, 0, 1, sizeof(char), 0, 0, src);
}