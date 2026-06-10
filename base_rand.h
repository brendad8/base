
/* 
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

/*
 * Implementation adapted by Brendan Callender
 * 
 * Modified to add implementations for uniform, 
 * normal, exponential, poisson, bernoulli, and 
 * binomial distributions
 *
 */

#ifndef BASE_RAND_H
#define BASE_RAND_H

#include "base_core.h"

void     rand_seed(uint64_t initstate, uint64_t initseq);
uint32_t rand_int(void);
float    rand_unif(void);
float    rand_norm(float mu, float sigma);
float    rand_exp(float lambda);
uint32_t rand_pois(float mu);
bool32   rand_bern(float p);
uint32_t rand_binom(uint32_t n, float p);

#endif


