
#include <stdarg.h>

#include "str.h"

// #define STB_SPRINTF_IMPLEMENTATION
// #include "deps/stb_sprintf.h"

/*********************************************************************************/
string str_new(char* ptr, size_t len) 
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
bool str_equal(string a, string b) 
{
    if (a.len != b.len) 
        return 0;

    return memcmp(a.ptr, b.ptr, a.len) == 0;
}

/*********************************************************************************/
bool str_equal_ci(string a, string b)
{
    if (a.len != b.len)
        return false;

    for (size_t i = 0; i < a.len; i++)
    {
        if (char_to_lower(a.ptr[i]) != char_to_lower(b.ptr[i]))
            return false;
    }

    return true;
}

/*********************************************************************************/
int str_compare(string a, string b) 
{
    size_t min_len = a.len < b.len ? a.len : b.len;
    int cmp = memcmp(a.ptr, b.ptr, min_len);

    return cmp != 0 ? cmp : (int)(a.len - b.len);
}

/*********************************************************************************/
int str_compare_ci(string a, string b)
{
    size_t min_len = a.len < b.len ? a.len : b.len;

    for (size_t i = 0; i < min_len; i++)
    {
        unsigned char ca = char_to_lower(a.ptr[i]);
        unsigned char cb = char_to_lower(b.ptr[i]);

        if (ca != cb)
            return (int)ca - (int)cb;
    }

    if (a.len < b.len)
        return -1;

    if (a.len > b.len)
        return 1;

    return 0;
}

/*********************************************************************************/
int str_find_idx_first(string s, string pattern)
{
    if (s.len < pattern.len || pattern.len == 0) 
        return -1;

    for (size_t i = 0; i <= (s.len - pattern.len); i++) 
    {
        for (size_t j = 0; j < pattern.len; j++) 
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
int str_find_idx_first_ci(string s, string pattern)
{
    if (s.len < pattern.len || pattern.len == 0) 
        return -1;

    for (size_t i = 0; i <= (s.len - pattern.len); i++) 
    {
        for (size_t j = 0; j < pattern.len; j++) 
        {
            if (char_to_lower(s.ptr[i+j]) != char_to_lower(pattern.ptr[j])) 
                break; 
            if (j == pattern.len-1)
                return i;
        }
    }
    return -1;
}

/*********************************************************************************/
int str_find_idx_last(string s, string pattern)
{
    if (s.len < pattern.len || pattern.len == 0) 
        return -1;

    for (size_t i = s.len - 1; i >= pattern.len - 1; i--) 
    {
        for (size_t j = 0; j < pattern.len; j++) 
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
int str_find_idx_last_ci(string s, string pattern)
{
    if (s.len < pattern.len || pattern.len == 0) 
        return -1;

    for (size_t i = s.len - 1; i >= pattern.len - 1; i--) 
    {
        for (size_t j = 0; j < pattern.len; j++) 
        {
            if (char_to_lower(s.ptr[i-j]) != char_to_lower(pattern.ptr[pattern.len - j - 1])) 
                break; 
            if (j == pattern.len - 1)
                return (i - pattern.len + 1);
        }
    }
    return -1;
}

/*********************************************************************************/
bool str_contains(string s, string pattern) 
{
    return str_find_idx_first(s, pattern) != -1;
}

/*********************************************************************************/
bool str_contains_ci(string s, string pattern)
{
    return str_find_idx_first_ci(s, pattern) != -1;
}

/*********************************************************************************/
bool str_starts_with(string s, string prefix) 
{
    if (s.len < prefix.len || prefix.len == 0)
        return 0;

    return memcmp(s.ptr, prefix.ptr, prefix.len) == 0;
}

/*********************************************************************************/
bool str_starts_with_ci(string s, string prefix)
{
    if (s.len < prefix.len || prefix.len == 0)
        return false;

    for (size_t i = 0; i < prefix.len; i++)
    {
        if (char_to_lower(s.ptr[i]) != char_to_lower(prefix.ptr[i]))
            return false;
    }

    return true;
}

/*********************************************************************************/
bool str_ends_with(string s, string suffix) 
{
    if (s.len < suffix.len || suffix.len == 0)
        return 0;

    return memcmp(s.ptr + (s.len - suffix.len), suffix.ptr, suffix.len) == 0;
}

/*********************************************************************************/
bool str_ends_with_ci(string s, string suffix)
{
    if (s.len < suffix.len || suffix.len == 0)
        return false;

    size_t start = s.len - suffix.len;

    for (size_t i = 0; i < suffix.len; i++)
    {
        if (char_to_lower(s.ptr[start + i]) != char_to_lower(suffix.ptr[i]))
            return false;
    }

    return true;
}

/*********************************************************************************/
string str_slice(string s, size_t start, size_t end)
{
    end = MIN(end, s.len);
    start = MIN(start, end);

    return str_new(s.ptr + start, (end - start));
}

/*********************************************************************************/
string str_slice_len(string s, size_t start, size_t length) 
{
    length = CLAMP_TOP(s.len - start, length);
    return str_new(s.ptr + start, length);
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
    uint32_t needed_bytes = stbsp_vsnprintf(0, 0, fmt, args) + 1;
    string result = {0};
    result.ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, needed_bytes);
    result.len = stbsp_vsnprintf((char*)result.ptr, needed_bytes, fmt, args2);
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
    char* ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, s.len + 1);
    for (size_t i = 0; i < s.len; i++)
        ptr[i] = char_to_lower(s.ptr[i]);
    ptr[s.len] = '\0';

    return str_new(ptr, s.len);
}

/*********************************************************************************/
string str_to_upper(Arena* arena, string s)
{
    char* ptr = ARENA_PUSH_ARRAY_NO_ZERO(arena, char, s.len + 1);
    for (size_t i = 0; i < s.len; i++)
        ptr[i] = char_to_upper(s.ptr[i]);
    ptr[s.len] = '\0';

    return str_new(ptr, s.len);
}

/*********************************************************************************/
StringList str_split(Arena* arena, string s, string delim)
{
    string current_split = {0};
    StringList str_list  = {0};

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

        StringNode* node = ARENA_PUSH_STRUCT(arena, StringNode);
        node->s = str_copy(arena, current_split);
        QUEUE_PUSH(str_list.first, str_list.last, node);
        str_list.node_count++;

        if (idx == -1)
            break;
    }

    return str_list;
}


/*********************************************************************************/
StringList str_split_skip_empty(Arena* arena, string s, string delim)
{
    string current_split = {0};
    StringList str_list  = {0};

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

        StringNode* node = ARENA_PUSH_STRUCT(arena, StringNode);
        node->s = str_copy(arena, current_split);

        QUEUE_PUSH(str_list.first, str_list.last, node);
        str_list.node_count++;
    }
    return str_list;
}


/*********************************************************************************/
bool   char_is_space(char c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

/*********************************************************************************/
bool   char_is_upper(char c)
{
    return ('A' <= c && c <= 'Z');
}

/*********************************************************************************/
bool   char_is_lower(char c)
{
    return ('a' <= c && c <= 'z');
}

/*********************************************************************************/
bool   char_is_alpha(char c)
{
    return (char_is_upper(c) || char_is_lower(c));
}

/*********************************************************************************/
bool   char_is_slash(char c)
{
    return (c == '/' || c == '\\');
}

/*********************************************************************************/
// bool   char_is_digit(char c, uint32_t base)
// {
//     bool  result = 0;
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
size_t cstr_length(char* c)
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

       
