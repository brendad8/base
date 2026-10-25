
#pragma once

#include "base_core.h"

#define str8_lit(s)  str8_new((u8*)(s), (u64)(sizeof(s) - 1))
#define str8_fmt "%.*s"
#define str8_varg(s) (int)((s).len), ((s).ptr)

typedef struct  
{ 
    u8* ptr; 
    u64 len; 

} String8;

String8  str8_new           (u8* ptr, u64 len);

// String8  str_from_cstr     (u8* ptr);
// String8  str_from_range    (u8* start, u8* one_past_last);
// String8  str_slice         (String8 s, u64 start_idx, u64 len);
//
// b32   str_equal         (String8 a, String8 b);
// b32   str_equal_ci      (String8 a, String8 b);
//
// int   str_compare       (void* a, void* b);
// int   str_compare_ci    (void* a, void* b);
//
// int   str_find          (String8 s, String8 pattern);
// b32   str_contains      (String8 s, String8 sub);
// b32   str_starts_with   (String8 s, String8 prefix);
// b32   str_ends_with     (String8 s, String8 suffix);
//
// String8  str_trim          (String8 s);
// String8  str_trim_left     (String8 s);
// String8  str_trim_right    (String8 s);
// String8  str_trim_prefix   (String8 s, String8 prefix);
// String8  str_trim_suffix   (String8 s, String8 suffix);
//
// String8  str_copy          (Allocator*, String8 s);
// String8  str_concat        (Allocator*, String8 a, String8 b);
//
// String8  str_to_lower      (Allocator*, String8 s);
// String8  str_to_upper      (Allocator*, String8 s);
//
// b32   char_is_space     (u8 c);
// b32   char_is_upper     (u8 c);
// b32   char_is_lower     (u8 c);
// b32   char_is_alpha     (u8 c);
// b32   char_is_slash     (u8 c);
// b32   char_is_digit     (u8 c, int base);
//
// u8    char_to_lower     (u8 c);
// u8    char_to_upper     (u8 c);
//
// u64   cstr_length       (u8* c);



