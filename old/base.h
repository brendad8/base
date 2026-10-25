
#ifndef BASE_H
#define BASE_H

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;

typedef uint8_t   byte;
typedef size_t    usize;
typedef ptrdiff_t isize;

/***************************************************************************
 *          MACROS
 ***************************************************************************/

#define KB(n)  (((uint64)(n)) << 10)
#define MB(n)  (((uint64)(n)) << 20)
#define GB(n)  (((uint64)(n)) << 30)
#define TB(n)  (((uint64)(n)) << 40)

#define THOUSAND(n) ((n)*1000)
#define MILLION(n)  ((n)*1000000)
#define BILLION(n)  ((n)*1000000000)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define CLAMP_TOP(val,max) MIN(val,max)
#define CLAMP_BOT(val,min) MAX(val,min)
#define CLAMP(min,val,max) (((val)<(min))?(min):((val)>(max))?(max):(val))

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define CEIL_INTEGER_DIV(a,b) (((a) + (b) - 1)/(b))

#define SWAP(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)

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

/***************************************************************************
 *          COMPILER
 ***************************************************************************/

#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#elif defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
#else
    #error Unknown compiler
#endif


/***************************************************************************
 *          OPERATING SYSTEM
 ***************************************************************************/

#if defined(_WIN32)
    #define OS_WINDOWS 1
#elif defined(__linux__)
    #define OS_LINUX 1
#elif defined(__APPLE__)
    #define OS_MACOS 1
#else
    #error Unknown platform
#endif

/***************************************************************************
 *          ALIGNMENT
 ***************************************************************************/

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
    #define align_type(x) __declspec(align(x))
#elif COMPILER_CLANG || COMPILER_GCC
    #define align_type(x) __attribute__((aligned(x)))
#else
    #error align_type not defined for this compiler.
#endif

/***************************************************************************
 *          ASSERT
 ***************************************************************************/

#if COMPILER_CLANG || COMPILER_GCC
    #define trap() __builtin_trap()
#elif COMPILER_MSVC
    # define trap() __debugbreak()
#else
    #error Unknown trap intrinsic for this compiler.
#endif

#define assert_always(x) do{if(!(x)) {trap();}}while(0)

#if BUILD_DEBUG
    #define assert(x) assert_always(x)
#else
    #define assert(x) (void)(x)
#endif

/***************************************************************************
 *          CONSTANTS
 ***************************************************************************/

static uint64 max_uint64 = 0xffffffffffffffffull;
static uint32 max_uint32 = 0xffffffff;
static uint16 max_uint16 = 0xffff;
static uint8  max_uint8  = 0xff;

static int64  max_int64 = (int64)0x7fffffffffffffffll;
static int32  max_int32 = (int32)0x7fffffff;
static int16  max_int16 = (int16)0x7fff;
static int8   max_int8  =  (int8)0x7f;

static int64  min_int64 = (int64)0x8000000000000000ll;
static int32  min_int32 = (int32)0x80000000;
static int16  min_int16 = (int16)0x8000;
static int8   min_int8  =  (int8)0x80;

/***************************************************************************
 *          MISC
 ***************************************************************************/

#if COMPILER_CLANG || COMPILER_GCC
    #define PRINTF_FORMAT(fmt_index, first_arg) \
        __attribute__((format(printf, fmt_index, first_arg)))
#elif defined(COMPILER_MSVC)
    #define PRINTF_FORMAT(fmt_index, first_arg)
#else
    #define PRINTF_FORMAT(fmt_index, first_arg)
#endif


#endif // BASE_H
