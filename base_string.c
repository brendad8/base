
#include "base_string.h"
#include "base_arena.h"
#include "base_ds.h"

#include <stdarg.h>
#include <stdio.h>


/*********************************************************************************/
string str_new(char* ptr, uint64_t len) 
{
    string s = { ptr, len };
    return s;
}

/*********************************************************************************/
string str_from_cstr(char* ptr) 
{
    size_t len = cstr_length(ptr);
    return str_new(ptr, len);
}

/*********************************************************************************/
bool32 str_equal(string a, string b) 
{
    if (a.len != b.len) 
        return 0;

    return memcmp(a.ptr, b.ptr, a.len) == 0;
}

/*********************************************************************************/
int32_t str_compare(string a, string b) 
{
    uint64_t min_len = a.len < b.len ? a.len : b.len;
    int cmp = memcmp(a.ptr, b.ptr, min_len);

    return cmp != 0 ? cmp : (int)(a.len - b.len);
}

/*********************************************************************************/
int32_t str_find_idx_first(string s, string pattern)
{
    if (s.len < pattern.len || pattern.len == 0) 
        return -1;

    for (int32_t i = 0; i <= (s.len - pattern.len); i++) 
    {
        for (int32_t j = 0; j < pattern.len; j++) 
        {
            if (s.ptr[i+j] != pattern.ptr[j]) 
                break; 
            if (j == pattern.len-1)
                return i;
        }
    }
    return -1;
}

/*********************************************************************************/
int32_t str_find_idx_last(string s, string pattern)
{
    if (s.len < pattern.len || pattern.len == 0) 
        return -1;

    for (int32_t i = s.len - 1; i >= pattern.len - 1; i--) 
    {
        for (int32_t j = 0; j < pattern.len; j++) 
        {
            if (s.ptr[i-j] != pattern.ptr[pattern.len - j - 1]) 
                break; 
            if (j == pattern.len - 1)
                return (i - pattern.len + 1);
        }
    }
    return -1;
}

/*********************************************************************************/
bool32 str_contains(string s, string pattern) 
{
    return str_find_idx_first(s, pattern) != -1;
}

/*********************************************************************************/
string str_slice(string s, uint64_t start, uint64_t end)
{
    end = MIN(end, s.len);
    start = MIN(start, end);

    return str_new(s.ptr + start, (end - start));
}

/*********************************************************************************/
string str_slice_len(string s, uint64_t start, uint64_t length) 
{
    length = CLAMP_TOP(s.len - start, length);
    return str_new(s.ptr + start, length);
}


/*********************************************************************************/
bool32 str_starts_with(string s, string prefix) 
{
    if (s.len < prefix.len)
        return 0;

    return memcmp(s.ptr, prefix.ptr, prefix.len) == 0;
}

/*********************************************************************************/
bool32 str_ends_with(string s, string suffix) 
{
    if (s.len < suffix.len)
        return 0;

    return memcmp(s.ptr + (s.len - suffix.len), suffix.ptr, suffix.len) == 0;
}

/*********************************************************************************/
string str_remove_prefix(string s, string prefix)
{
    if (s.len < prefix.len)
        return str_new(s.ptr, s.len);

    if (memcmp(s.ptr, prefix.ptr, prefix.len) == 0)
        return str_new(s.ptr, s.len);
    
    return str_new(s.ptr + prefix.len, s.len - prefix.len);
}

/*********************************************************************************/
string str_remove_suffix(string s, string suffix)
{
    if (s.len < suffix.len)
        return str_new(s.ptr, s.len);

    if (memcmp(s.ptr + s.len - suffix.len, suffix.ptr, suffix.len) == 0)
        return str_new(s.ptr, s.len);
    
    return str_new(s.ptr, s.len - suffix.len);
}

/*********************************************************************************/
string str_trim_left(string s)
{
    size_t trim_off = 0;
    while (char_is_space(*(s.ptr + trim_off)))
        trim_off++;

    return str_new(s.ptr + trim_off, s.len - trim_off); 
}

/*********************************************************************************/
string str_trim_right(string s)
{
    size_t trim_off = 0;
    while (char_is_space(*(s.ptr + s.len - trim_off)))
        trim_off++;

    return str_new(s.ptr, s.len - trim_off); 
}

/*********************************************************************************/
string str_trim(string s)
{
    string tmp = str_trim_left(s);
    return str_trim_right(tmp);
}

/*********************************************************************************/
string str_copy(Arena *arena, string s)
{
    string res;
    res.len = s.len;
    res.ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, res.len + 1);
    memmove(res.ptr, s.ptr, s.len);
    res.ptr[res.len] = 0;
    return res;
}

/*********************************************************************************/
string str_cat(Arena *arena, string a, string b)
{
    string res;
    res.len = a.len + b.len;
    res.ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, res.len + 1);
    memmove(res.ptr, a.ptr, a.len);
    memmove(res.ptr + a.len, b.ptr, b.len);
    res.ptr[res.len] = 0;
    return res;
}

/*********************************************************************************/
string str_printfv(Arena *arena, char* fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);
    uint32_t needed_bytes = vsnprintf(0, 0, fmt, args) + 1;
    string result = {0};
    result.ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, needed_bytes);
    result.len = vsnprintf((char*)result.ptr, needed_bytes, fmt, args2);
    result.ptr[result.len] = 0;
    va_end(args2);
    return result;
}

string str_printf(Arena *arena, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    string result = str_printfv(arena, fmt, args);
    va_end(args);
    return result;
}

/*********************************************************************************/
string str_to_lower(Arena* arena, string s)
{
    char* ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, s.len);
    for (uint64_t i = 0; i < s.len; i++)
        ptr[i] = char_to_lower(s.ptr[i]);

    return str_new(ptr, s.len);
}

/*********************************************************************************/
string str_to_upper(Arena* arena, string s)
{
    char* ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, s.len);
    for (uint64_t i = 0; i < s.len; i++)
        ptr[i] = char_to_upper(s.ptr[i]);

    return str_new(ptr, s.len);
}

/*********************************************************************************/
stringList str_split(Arena* arena, string s, string delim)
{
    string current_split = {0};
    stringList str_list  = {0};

    bool32 ends_with_delim = str_ends_with(s, delim);

    while (1)
    {
        int32_t idx = str_find_idx_first(s, delim);
        if (idx != -1)
        {
            current_split = str_slice(s, 0, idx);
            s = str_slice(s, idx + delim.len, s.len);
        }
        else
        {
            current_split = s;
            s.len = 0;
        }

        stringNode* node = ARENA_PUSH_STRUCT(arena, stringNode);
        node->s = str_copy(arena, current_split);
        QUEUE_PUSH(str_list.first, str_list.last, node);
        str_list.node_count++;

        if (idx == -1)
            break;
    }

    return str_list;
}


/*********************************************************************************/
stringList str_split_skip_empty(Arena* arena, string s, string delim)
{
    string current_split = {0};
    stringList str_list  = {0};

    while (s.len > 0)
    {
        int32_t idx = str_find_idx_first(s, delim);
        if (idx != -1)
        {
            current_split = str_slice(s, 0, idx);
            s = str_slice(s, idx + delim.len, s.len);
        }
        else
        {
            current_split = s;
            s.len = 0;
        }

        // skip over empty splits
        if (idx == 0) continue;

        stringNode* node = ARENA_PUSH_STRUCT(arena, stringNode);
        node->s = str_copy(arena, current_split);

        QUEUE_PUSH(str_list.first, str_list.last, node);
        str_list.node_count++;
    }
    return str_list;
}


/*********************************************************************************/
bool32 char_is_space(char c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

/*********************************************************************************/
bool32 char_is_upper(char c)
{
    return ('A' <= c && c <= 'Z');
}

/*********************************************************************************/
bool32 char_is_lower(char c)
{
    return ('a' <= c && c <= 'z');
}

/*********************************************************************************/
bool32 char_is_alpha(char c)
{
    return (char_is_upper(c) || char_is_lower(c));
}

/*********************************************************************************/
bool32 char_is_slash(char c)
{
    return (c == '/' || c == '\\');
}

/*********************************************************************************/
// bool32 char_is_digit(char c, uint32_t base)
// {
//     bool32 result = 0;
//     if (0 < base && base <= 16)
//     {
//         char val = integer_symbol_reverse[c];
//         if(val < base)
//         {
//             result = 1;
//         }
//     }
//     return result;
// }

/*********************************************************************************/
char char_to_lower(char c)
{
    if (char_is_upper(c)) 
        c += ('a' - 'A');
    return c;
}

/*********************************************************************************/
char char_to_upper(char c)
{
    if (char_is_lower(c))
        c += ('A' - 'a');
    return c;
}

/*********************************************************************************/
uint64_t cstr_length(char* c)
{
    size_t len = 0;
    if(c)
    {
        char *p = c;
        for (; *p != 0; p += 1);
        len = (size_t)(p - c);
    }
    return len;
}



