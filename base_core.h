
#pragma once

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef float     f32;
typedef double    f64;

typedef int8_t    b8;
typedef int16_t   b16;
typedef int32_t   b32;
typedef int64_t   b64;


/***************************************************************************
 *          DEFINES
 ***************************************************************************/

#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#elif defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
#else
    #error "unknown compiler"
#endif


#if defined(_WIN32)
    #define OS_WINDOWS 1
#elif defined(__linux__)
    #define OS_LINUX 1
#elif defined(__APPLE__)
    #define OS_MAC 1
#else
    #error "unknown platform"
#endif

#if COMPILER_MSVC
    #define alignof(T) __alignof(T)
#elif COMPILER_CLANG
    #define alignof(T) __alignof(T)
#elif COMPILER_GCC
    #define alignof(T) __alignof__(T)
#else
    #error "alignof not defined for this compiler"
#endif

#if COMPILER_MSVC
    #define align_type(x) __declspec(align(x))
#elif COMPILER_CLANG || COMPILER_GCC
    #define align_type(x) __attribute__((aligned(x)))
#else
    #error "align_type not defined for this compiler"
#endif

#if COMPILER_CLANG || COMPILER_GCC
    #define trap() __builtin_trap()
#elif COMPILER_MSVC
    #define trap() __debugbreak()
#else
    #error "unknown trap intrinsic for this compiler"
#endif

#define assert_always(x) do{if(!(x)) {trap();}}while(0)

#if BUILD_DEBUG
    #define assert(x) assert_always(x)
#else
    #define assert(x) (void)(x)
#endif

#define mem_zero(ptr, len) memset((ptr), 0, (len))
#define mem_copy(dest, src, len) memmove((dest), (src), (len))
#define mem_equal(ptr_a, ptr_b, len) (memcmp((ptr_a), (ptr_b), (len)) == 0)
#define mem_compare(ptr_a, ptr_b, len) (memcmp((ptr_a), (ptr_b), (len)))

#define mem_align_up_pow2(x,b)   (((x) + (b) - 1)&(~((b) - 1)))
#define mem_align_down_pow2(x,b) ((x)&(~((b) - 1)))

#define KB(n)  (((u64)(n)) << 10)
#define MB(n)  (((u64)(n)) << 20)
#define GB(n)  (((u64)(n)) << 30)
#define TB(n)  (((u64)(n)) << 40)

#if COMPILER_CLANG
    #define branch_expect(expr, val) __builtin_expect((expr), (val))
#else
    #define branch_expect(expr, val) (expr)
#endif

#define branch_likely(expr) branch_expect(expr,1)
#define branch_unlikely(expr) branch_expect(expr,0)

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define clamp_top(a,x) min(a,x)
#define clamp_bot(x,b) max(x,b)
#define clamp(a,x,b) (((x)<(a))?(a):((x)>(b))?(b):(x))

#define foreach(it, count) for (int it = 0; i < (count); it += 1)

/***************************************************************************
 *          KEYWORDS
 ***************************************************************************/

#define global static
#define internal static
#define local_persist static

#if COMPILER_MSVC
    #define thread_local __declspec(thread)
#elif COMPILER_GCC || COMPILER_CLANG
    #define thread_local __thread
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define thread_local _Thread_local
#else
    #error "thread_local storage is not supported by this compiler"
#endif

