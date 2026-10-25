
/* rand.h - pseudo-random number generation

   To use this library, do this in *one* C file:
      #define RAND_IMPLEMENTATION
      #include "base/rand.h"

   ACKNOWLEDGMENTS

      PCG Random Number Generation for C.
      Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>

      Licensed under the Apache License, Version 2.0 (the "License");
      you may not use this file except in compliance with the License.
      You may obtain a copy of the License at
*/

#ifndef RAND_H
#define RAND_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include "base.h" 

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

void     rand_seed    (uint64 initstate, uint64 initseq);
uint32   rand_int     (void);
float    rand_unif    (void);
float    rand_norm    (float mu, float sigma);
float    rand_exp     (float lambda);
uint32   rand_pois    (float mu);
bool     rand_bern    (float p);
uint32   rand_binom   (uint32 n, float p);

#ifdef __cplusplus
}
#endif
       
#endif // RAND_H
       
/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef RAND_IMPLEMENTATION
#define RAND_IMPLEMENTATION

#include <math.h>

typedef struct 
{
    uint64 state;         
    uint64 inc;
    float prev_norm;

} prng_state;

static prng_state s_prng_state = {
    0x853c49e6748fea9bull, 0xda3e39cb94b95bdbull, NAN
};

static uint32 prng_rand_r(prng_state* rng)
{
    uint64 oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ull + rng->inc;
    uint32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint32 rand_int(void)
{
    return prng_rand_r(&s_prng_state);
}

static void prng_seed_r(prng_state* rng, uint64 initstate, uint64 initseq)
{
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    prng_rand_r(rng);
    rng->state += initstate;
    prng_rand_r(rng);
}

void rand_seed(uint64 seed, uint64 seq)
{
    prng_seed_r(&s_prng_state, seed, seq);
}

static float prng_runif_r(prng_state* rng)
{
    return prng_rand_r(rng) / (float)UINT32_MAX;
}

float rand_unif(void)
{
    return prng_runif_r(&s_prng_state);
}

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

float rand_norm(float mu, float sigma)
{
    return prng_rnorm_r(&s_prng_state, mu, sigma);
}

static float prng_rexp_r(prng_state* rng, float lambda)
{
    return (-1 / lambda) * log(prng_runif_r(rng));
}

float rand_exp(float lambda)
{
    return prng_rexp_r(&s_prng_state, lambda);
}

static uint32 prng_rpois_r(prng_state* rng, float mu)
{
    uint32 k = 0;
    float t = 0.0f;
    while (t < mu)
    {
        t += prng_rexp_r(&s_prng_state, 1);
        k += 1;
    }
    return k;
}

uint32 rand_pois(float mu)
{
    return prng_rpois_r(&s_prng_state, mu);
}

static bool prng_rbern_r(prng_state* rng, float p)
{
    return prng_runif_r(&s_prng_state) < p;
}

bool rand_bern(float p)
{
    return prng_rbern_r(&s_prng_state, p);
}

static uint32 prng_rbinom_r(prng_state* rng, uint32 n, float p)
{
    uint32 x = 0;
    for (int i = 0; i < n; i++)
        x += (uint32)prng_rbern_r(rng, p);

    return x;
}

uint32 rand_binom(uint32 n, float p)
{
    return prng_rbinom_r(&s_prng_state, n, p);
}

#endif // RAND_IMPLEMENTATION
