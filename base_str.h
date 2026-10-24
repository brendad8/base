
#pragma once

#include "base_core.h"

#define str8_lit(s)  str8_new((u8*)(s), (u64)(sizeof(s) - 1))
#define str8_fmt "%.*s"
#define str8_varg(s) (int)((s).len), ((s).ptr)

typedef struct  
{ 
    u8* ptr; 
    u64 len; 

} str8;

str8  str8_new           (u8* ptr, u64 len);

// str8  str_from_cstr     (u8* ptr);
// str8  str_from_range    (u8* start, u8* one_past_last);
// str8  str_slice         (str8 s, u64 start_idx, u64 len);
//
// b32   str_equal         (str8 a, str8 b);
// b32   str_equal_ci      (str8 a, str8 b);
//
// int   str_compare       (void* a, void* b);
// int   str_compare_ci    (void* a, void* b);
//
// int   str_find          (str8 s, str8 pattern);
// b32   str_contains      (str8 s, str8 sub);
// b32   str_starts_with   (str8 s, str8 prefix);
// b32   str_ends_with     (str8 s, str8 suffix);
//
// str8  str_trim          (str8 s);
// str8  str_trim_left     (str8 s);
// str8  str_trim_right    (str8 s);
// str8  str_trim_prefix   (str8 s, str8 prefix);
// str8  str_trim_suffix   (str8 s, str8 suffix);
//
// str8  str_copy          (Allocator*, str8 s);
// str8  str_concat        (Allocator*, str8 a, str8 b);
//
// str8  str_to_lower      (Allocator*, str8 s);
// str8  str_to_upper      (Allocator*, str8 s);
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



