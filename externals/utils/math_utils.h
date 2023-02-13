#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <gmp.h>


#ifndef MATH_UTILS
#define MATH_UTILS

//BN stands for big number

/*
   Computes a random big number
*/
void bn_get_random(mpz_ptr ros, mp_bitcnt_t nbits);
/*
   Computes a random probable prime number with a range given by the number of bits
*/
void bn_getprimeb_random(mpz_ptr ros, mp_bitcnt_t nbits);

#endif