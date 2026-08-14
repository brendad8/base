
/* rand.h - pseudo-random number generator and probability distributions

   To use this library, do this in *one* C file:
      #define RAND_IMPLEMENTATION
      #include "base/rand.h"


ACKNOWLEDGMENTS

   PCG Random Number Generation for C.
   Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at


COMPILE-TIME OPTIONS

  #define RAND_EXPORT

     Declares the export/import specifier used for all public functions.
     Leave undefined for normal static builds or redefine when building
     as part of a shared library.


DOCUMENTATION

  Seed the generator before use:
    rand_seed(42, 54);

  Generate random values:

    uint32_t i = rand_int();
    float u = rand_unif();


  void      rand_seed   (uint64_t initstate, uint64_t initseq)    - Seeds the pseudo-random number generator

  uint32_t  rand_int    (void)                                    - Returns a uniformly distributed 32-bit unsigned integer

  float     rand_unif   (void)                                    - Returns a uniformly distributed float in the range [0, 1]
  float     rand_norm   (float mu, float sigma)                   - Returns a normally distributed random value with mean mu and standard deviation sigma
  float     rand_exp    (float lambda)                            - Returns an exponentially distributed random value with rate lambda
  uint32_t  rand_pois   (float mu)                                - Returns a Poisson distributed random value with mean mu
  bool      rand_bern   (float p)                                 - Returns true with probability p and false otherwise
  uint32_t  rand_binom  (uint32_t n, float p)                     - Returns a binomially distributed random value for n trials with success probability p


NOTES

  The library maintains a single global pseudo-random number generator.
  All public functions operate on this shared state.

  The generator is based on the PCG family of random number generators
  and provides fast, high-quality random numbers suitable for simulations,
  games, procedural generation, and general-purpose use.

  Distribution implementations:

    rand_unif()  - Uniform distribution over [0,1]
    rand_norm()  - Normal (Gaussian) distribution using the Box-Muller transform
    rand_exp()   - Exponential distribution using inverse transform sampling
    rand_pois()  - Poisson distribution using exponential inter-arrival times
    rand_bern()  - Bernoulli distribution
    rand_binom() - Binomial distribution implemented as repeated Bernoulli trials

*/

#ifndef RAND_H
#define RAND_H

#ifndef RAND_EXPORT
#define RAND_EXPORT
#endif

//***************************************************************************
//          INCLUDE_FILES
//***************************************************************************

#include <stdint.h>

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

RAND_EXPORT void       rand_seed    (uint64_t initstate, uint64_t initseq);
RAND_EXPORT uint32_t   rand_int     (void);
RAND_EXPORT float      rand_unif    (void);
RAND_EXPORT float      rand_norm    (float mu, float sigma);
RAND_EXPORT float      rand_exp     (float lambda);
RAND_EXPORT uint32_t   rand_pois    (float mu);
RAND_EXPORT bool       rand_bern    (float p);
RAND_EXPORT uint32_t   rand_binom   (uint32_t n, float p);

//***************************************************************************
//          FUNCTION IMPLEMENTATIONS
//***************************************************************************

#ifdef RAND_IMPLEMENTATION
#define RAND_IMPLEMENTATION

#include <math.h>

typedef struct 
{
    uint64_t state;         
    uint64_t inc;
    float prev_norm;

} prng_state;

static prng_state s_prng_state = {
    0x853c49e6748fea9bull, 0xda3e39cb94b95bdbull, NAN
};

/********************************************************************************/
static uint32_t prng_rand_r(prng_state* rng)
{
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ull + rng->inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

RAND_EXPORT uint32_t rand_int(void)
{
    return prng_rand_r(&s_prng_state);
}

/********************************************************************************/
static void prng_seed_r(prng_state* rng, uint64_t initstate, uint64_t initseq)
{
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    prng_rand_r(rng);
    rng->state += initstate;
    prng_rand_r(rng);
}

RAND_EXPORT void rand_seed(uint64_t seed, uint64_t seq)
{
    prng_seed_r(&s_prng_state, seed, seq);
}


/********************************************************************************/
static float prng_runif_r(prng_state* rng)
{
    return prng_rand_r(rng) / (float)UINT32_MAX;
}

RAND_EXPORT float rand_unif(void)
{
    return prng_runif_r(&s_prng_state);
}

/********************************************************************************/
static float prng_rnorm_r(prng_state* rng, float mu, float sigma)
{
    float u1 = 1.0f, u2 = 0.0f;

    if (!isnan(rng->prev_norm))
    {
        float out = rng->prev_norm;
        rng->prev_norm = NAN;
        return out;
    }

    do {
        u1 = prng_runif_r(rng);
    } while (u1 == 0.0f);

    u2 = prng_runif_r(rng);
    float mag = sigma * sqrt(-2.0f * log(u1));
    float z0 = mag * cos(2 * pi32 * u2) + mu;
    float z1 = mag * sin(2 * pi32 * u2) + mu;

    rng->prev_norm = z1;
    return z0;
}

RAND_EXPORT float rand_norm(float mu, float sigma)
{
    return prng_rnorm_r(&s_prng_state, mu, sigma);
}

/********************************************************************************/
static float prng_rexp_r(prng_state* rng, float lambda)
{
    return (-1 / lambda) * log(prng_runif_r(rng));
}

RAND_EXPORT float rand_exp(float lambda)
{
    return prng_rexp_r(&s_prng_state, lambda);
}

/********************************************************************************/
static uint32_t prng_rpois_r(prng_state* rng, float mu)
{
    uint32_t k = 0;
    float t = 0.0f;
    while (t < mu)
    {
        t += prng_rexp_r(&s_prng_state, 1);
        k += 1;
    }
    return k;
}

RAND_EXPORT uint32_t rand_pois(float mu)
{
    return prng_rpois_r(&s_prng_state, mu);
}


/********************************************************************************/
static bool prng_rbern_r(prng_state* rng, float p)
{
    return prng_runif_r(&s_prng_state) < p;
}

RAND_EXPORT bool rand_bern(float p)
{
    return prng_rbern_r(&s_prng_state, p);
}


/********************************************************************************/
static uint32_t prng_rbinom_r(prng_state* rng, uint32_t n, float p)
{
    uint32_t x = 0;
    for (int i = 0; i < n; i++)
        x += (uint32_t)prng_rbern_r(rng, p);

    return x;
}

RAND_EXPORT uint32_t rand_binom(uint32_t n, float p)
{
    return prng_rbinom_r(&s_prng_state, n, p);
}

#endif // RAND_IMPLEMENTATION

#endif // RAND_H


