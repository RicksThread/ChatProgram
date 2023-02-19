#ifndef GMP_UTILS_H
#define GMP_UTILS_H

#include <gmp.h>

void buffer_to_mpz(mpz_ptr dest, char* buffer, size_t count);

void mpz_to_buffer(void* buffer, mpz_srcptr src);

#endif