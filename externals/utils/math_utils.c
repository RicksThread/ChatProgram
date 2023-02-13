#include "math_utils.h"

//for some reason the intellisense notifies me of undefined behaviour but
//this solves it.

__uint8_t rand_initiated = 0;

void init_rand()
{
    rand_initiated = 1;
    srand(time(NULL));
}

void bn_get_random(mpz_t ros, mp_bitcnt_t nbits)
{
    if (!rand_initiated)
        init_rand();
    
    unsigned long seed = rand();
    // perform inits to create variable pointers with 0 value
    // initialize state for a Mersenne Twister algorithm. This algorithm is fast and has good randomness properties.
    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);

    // create the generator seed for the random engine to reference 
    gmp_randseed_ui(rstate, seed);


    // return a uniformly distributed random number in the range 0 to n-1, inclusive.
    mpz_urandomb(ros, rstate, nbits);

    // *********************** GARBAGE COLLECTION *********************** //
    gmp_randclear(rstate);
}

void bn_getprimeb_random(mpz_ptr ros, mp_bitcnt_t nbits)
{
    bn_get_random(ros, nbits);
    mpz_nextprime(ros, ros);
}