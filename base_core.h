
// Adapted from Raddebugger base written by Ryan J. Fleury with the following license:
// Copyright (c) Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdint.h>
#include <string.h>


//**************************
// Base Types
//**************************

typedef int8_t     bool8;
typedef int16_t    bool16;
typedef int32_t    bool32;
typedef int64_t    bool64;

typedef float      float32;
typedef double     float64;

//**************************
// Units
//**************************

#define KB(n)  (((uint64_t)(n)) << 10)
#define MB(n)  (((uint64_t)(n)) << 20)
#define GB(n)  (((uint64_t)(n)) << 30)
#define TB(n)  (((uint64_t)(n)) << 40)

#define THOUSAND(n) ((n)*1000)
#define MILLION(n)  ((n)*1000000)
#define BILLION(n)  ((n)*1000000000)


//**************************
// Useful Macros
//**************************

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define CLAMP_TOP(val,max) MIN(val,max)
#define CLAMP_BOT(val,min) MAX(val,min)
#define CLAMP(min,val,max) (((val)<(min))?(min):((val)>(max))?(max):(val))

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define CEIL_INTEGER_DIV(a,b) (((a) + (b) - 1)/(b))

#define SWAP(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)


//**************************
// Memory Macros/Functions
//**************************

#define COMPOSE_64BIT(a,b)  ((((uint64)a) << 32) | ((uint64)b))
#define COMPOSE_32BIT(a,b)  ((((uint32)a) << 16) | ((uint32)b))

#define ALIGN_UP_POW2(x,b)   (((x) + (b) - 1)&(~((b) - 1)))
#define ALIGN_DOWN_POW2(x,b) ((x)&(~((b) - 1)))

#define IS_POW2(x)           ((x)!=0 && ((x)&((x)-1))==0)
#define IS_POW2_OR_ZERO(x)   ((((x) - 1)&(x)) == 0)

#define EXTRACT_BIT(word, idx) (((word) >> (idx)) & 1)
#define EXTRACT8(word, pos)    (((word) >> ((pos)*8))  & max_uint8)
#define EXTRACT16(word, pos)   (((word) >> ((pos)*16)) & max_uint16)
#define EXTRACT32(word, pos)   (((word) >> ((pos)*32)) & max_uint32)

//**************************
// Compiler macros 
//**************************

#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#elif defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
#else
    #error Unknown compiler
#endif


//**************************
// Platform macros 
//**************************

#if defined(_WIN32)
    #define PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
#elif defined(__APPLE__)
    #define PLATFORM_MACOS 1
#else
    #error Unknown platform
#endif

//**************************
// Alignment
//**************************

#if COMPILER_MSVC
    #define align_of(T) __alignof(T)
#elif COMPILER_CLANG
    #define align_of(T) __alignof(T)
#elif COMPILER_GCC
    #define align_of(T) __alignof__(T)
#else
    #error align_of not defined for this compiler.
#endif

#if COMPILER_MSVC
    #define AlignType(x) __declspec(align(x))
#elif COMPILER_CLANG || COMPILER_GCC
    #define AlignType(x) __attribute__((aligned(x)))
#else
    #error AlignType not defined for this compiler.
#endif


//**************************
// Assert
//**************************

#if COMPILER_CLANG || COMPILER_GCC
    #define TRAP() __builtin_trap()
#elif COMPILER_MSVC
    # define TRAP() __debugbreak()
#else
    #error Unknown trap intrinsic for this compiler.
#endif

#define ASSERT_ALWAYS(x) do{if(!(x)) {TRAP();}}while(0)

#if BUILD_DEBUG
    #define ASSERT(x) ASSERT_ALWAYS(x)
#else
    #define ASSERT(x) (void)(x)
#endif


//**************************
// Basic Constants
//**************************

static uint32_t sign32     = 0x80000000;
static uint32_t exponent32 = 0x7F800000;
static uint32_t mantissa32 = 0x007FFFFF;

static float pi32 = 3.1415926535897f;

static uint64_t max_uint64 = 0xffffffffffffffffull;
static uint32_t max_uint32 = 0xffffffff;
static uint16_t max_uint16 = 0xffff;
static uint8_t  max_uint8  = 0xff;

static int64_t max_int64 = (int64_t)0x7fffffffffffffffll;
static int32_t max_int32 = (int32_t)0x7fffffff;
static int16_t max_int16 = (int16_t)0x7fff;
static int8_t  max_int8  =  (int8_t)0x7f;

static int64_t min_int64 = (int64_t)0x8000000000000000ll;
static int32_t min_int32 = (int32_t)0x80000000;
static int16_t min_int16 = (int16_t)0x8000;
static int8_t  min_int8  =  (int8_t)0x80;

static const uint32_t bitmask1  = 0x00000001;
static const uint32_t bitmask2  = 0x00000003;
static const uint32_t bitmask3  = 0x00000007;
static const uint32_t bitmask4  = 0x0000000f;
static const uint32_t bitmask5  = 0x0000001f;
static const uint32_t bitmask6  = 0x0000003f;
static const uint32_t bitmask7  = 0x0000007f;
static const uint32_t bitmask8  = 0x000000ff;
static const uint32_t bitmask9  = 0x000001ff;
static const uint32_t bitmask10 = 0x000003ff;
static const uint32_t bitmask11 = 0x000007ff;
static const uint32_t bitmask12 = 0x00000fff;
static const uint32_t bitmask13 = 0x00001fff;
static const uint32_t bitmask14 = 0x00003fff;
static const uint32_t bitmask15 = 0x00007fff;
static const uint32_t bitmask16 = 0x0000ffff;
static const uint32_t bitmask17 = 0x0001ffff;
static const uint32_t bitmask18 = 0x0003ffff;
static const uint32_t bitmask19 = 0x0007ffff;
static const uint32_t bitmask20 = 0x000fffff;
static const uint32_t bitmask21 = 0x001fffff;
static const uint32_t bitmask22 = 0x003fffff;
static const uint32_t bitmask23 = 0x007fffff;
static const uint32_t bitmask24 = 0x00ffffff;
static const uint32_t bitmask25 = 0x01ffffff;
static const uint32_t bitmask26 = 0x03ffffff;
static const uint32_t bitmask27 = 0x07ffffff;
static const uint32_t bitmask28 = 0x0fffffff;
static const uint32_t bitmask29 = 0x1fffffff;
static const uint32_t bitmask30 = 0x3fffffff;
static const uint32_t bitmask31 = 0x7fffffff;
static const uint32_t bitmask32 = 0xffffffff;

static const uint64_t bitmask33 = 0x00000001ffffffffull;
static const uint64_t bitmask34 = 0x00000003ffffffffull;
static const uint64_t bitmask35 = 0x00000007ffffffffull;
static const uint64_t bitmask36 = 0x0000000fffffffffull;
static const uint64_t bitmask37 = 0x0000001fffffffffull;
static const uint64_t bitmask38 = 0x0000003fffffffffull;
static const uint64_t bitmask39 = 0x0000007fffffffffull;
static const uint64_t bitmask40 = 0x000000ffffffffffull;
static const uint64_t bitmask41 = 0x000001ffffffffffull;
static const uint64_t bitmask42 = 0x000003ffffffffffull;
static const uint64_t bitmask43 = 0x000007ffffffffffull;
static const uint64_t bitmask44 = 0x00000fffffffffffull;
static const uint64_t bitmask45 = 0x00001fffffffffffull;
static const uint64_t bitmask46 = 0x00003fffffffffffull;
static const uint64_t bitmask47 = 0x00007fffffffffffull;
static const uint64_t bitmask48 = 0x0000ffffffffffffull;
static const uint64_t bitmask49 = 0x0001ffffffffffffull;
static const uint64_t bitmask50 = 0x0003ffffffffffffull;
static const uint64_t bitmask51 = 0x0007ffffffffffffull;
static const uint64_t bitmask52 = 0x000fffffffffffffull;
static const uint64_t bitmask53 = 0x001fffffffffffffull;
static const uint64_t bitmask54 = 0x003fffffffffffffull;
static const uint64_t bitmask55 = 0x007fffffffffffffull;
static const uint64_t bitmask56 = 0x00ffffffffffffffull;
static const uint64_t bitmask57 = 0x01ffffffffffffffull;
static const uint64_t bitmask58 = 0x03ffffffffffffffull;
static const uint64_t bitmask59 = 0x07ffffffffffffffull;
static const uint64_t bitmask60 = 0x0fffffffffffffffull;
static const uint64_t bitmask61 = 0x1fffffffffffffffull;
static const uint64_t bitmask62 = 0x3fffffffffffffffull;
static const uint64_t bitmask63 = 0x7fffffffffffffffull;
static const uint64_t bitmask64 = 0xffffffffffffffffull;

static const uint32_t bit1  = (1<<0);
static const uint32_t bit2  = (1<<1);
static const uint32_t bit3  = (1<<2);
static const uint32_t bit4  = (1<<3);
static const uint32_t bit5  = (1<<4);
static const uint32_t bit6  = (1<<5);
static const uint32_t bit7  = (1<<6);
static const uint32_t bit8  = (1<<7);
static const uint32_t bit9  = (1<<8);
static const uint32_t bit10 = (1<<9);
static const uint32_t bit11 = (1<<10);
static const uint32_t bit12 = (1<<11);
static const uint32_t bit13 = (1<<12);
static const uint32_t bit14 = (1<<13);
static const uint32_t bit15 = (1<<14);
static const uint32_t bit16 = (1<<15);
static const uint32_t bit17 = (1<<16);
static const uint32_t bit18 = (1<<17);
static const uint32_t bit19 = (1<<18);
static const uint32_t bit20 = (1<<19);
static const uint32_t bit21 = (1<<20);
static const uint32_t bit22 = (1<<21);
static const uint32_t bit23 = (1<<22);
static const uint32_t bit24 = (1<<23);
static const uint32_t bit25 = (1<<24);
static const uint32_t bit26 = (1<<25);
static const uint32_t bit27 = (1<<26);
static const uint32_t bit28 = (1<<27);
static const uint32_t bit29 = (1<<28);
static const uint32_t bit30 = (1<<29);
static const uint32_t bit31 = (1<<30);
static const uint32_t bit32 = (1<<31);

static const uint64_t bit33 = (1ull<<32);
static const uint64_t bit34 = (1ull<<33);
static const uint64_t bit35 = (1ull<<34);
static const uint64_t bit36 = (1ull<<35);
static const uint64_t bit37 = (1ull<<36);
static const uint64_t bit38 = (1ull<<37);
static const uint64_t bit39 = (1ull<<38);
static const uint64_t bit40 = (1ull<<39);
static const uint64_t bit41 = (1ull<<40);
static const uint64_t bit42 = (1ull<<41);
static const uint64_t bit43 = (1ull<<42);
static const uint64_t bit44 = (1ull<<43);
static const uint64_t bit45 = (1ull<<44);
static const uint64_t bit46 = (1ull<<45);
static const uint64_t bit47 = (1ull<<46);
static const uint64_t bit48 = (1ull<<47);
static const uint64_t bit49 = (1ull<<48);
static const uint64_t bit50 = (1ull<<49);
static const uint64_t bit51 = (1ull<<50);
static const uint64_t bit52 = (1ull<<51);
static const uint64_t bit53 = (1ull<<52);
static const uint64_t bit54 = (1ull<<53);
static const uint64_t bit55 = (1ull<<54);
static const uint64_t bit56 = (1ull<<55);
static const uint64_t bit57 = (1ull<<56);
static const uint64_t bit58 = (1ull<<57);
static const uint64_t bit59 = (1ull<<58);
static const uint64_t bit60 = (1ull<<59);
static const uint64_t bit61 = (1ull<<60);
static const uint64_t bit62 = (1ull<<61);
static const uint64_t bit63 = (1ull<<62);
static const uint64_t bit64 = (1ull<<63);


#endif // BASE_CORE_H
