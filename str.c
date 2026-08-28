
#include "str.h"

bool char_is_space(char c)
{
  return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

bool char_is_upper(char c)
{
  return ('A' <= c && c <= 'Z');
}

bool char_is_lower(char c)
{
  return ('a' <= c && c <= 'z');
}

bool char_is_alpha(char c)
{
    return (char_is_upper(c) || char_is_lower(c));
}

bool char_is_slash(char c)
{
    return (c == '/' || c == '\\');
}

bool char_is_digit(char c, int base)
{
    bool result = false;
    if (0 < base && base <= 16)
    {
        result = (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'));
    }
    return result;
}

// int char_symbol_to_value(char c)
// {
//     if (c >= '0' && c <= '9')
//         return c - '0';
//
//     if (c >= 'a' && c <= 'z')
//         return c - 'a' + 10;
//
//     if (c >= 'A' && c <= 'Z')
//         return c - 'A' + 10;
//
//     return -1;
// }

char char_to_lower(char c)
{
    if (char_is_upper(c))
    {
        c += ('a' - 'A');
    }
    return c;
}

char char_to_upper(char c)
{
    if (char_is_lower(c))
    {
        c += ('A' - 'a');
    }
    return c;
}

usize cstr_length(char* c)
{
    usize length = 0;
    if(c)
    {
        char* p = c;
        for (; *p != 0; p += 1);
        length = (usize)(p - c);
    }
    return length;
}

string str_new(char* ptr, size_t len) 
{
    string s = { ptr, len };
    return s;
}

string str_from_cstr(char* ptr) 
{
    size_t len = cstr_length(ptr);
    return str_new(ptr, len);
}

string str_from_range(char* first, char* one_past_last) 
{
    string result = {first, (usize)(one_past_last - first)};
    return result;
}

static int memcmp_ci(char* a, char* b, usize size)
{
    int result = 0;
    for (usize i = 0; i < size; i++)
    {
        result += char_to_lower(a[i]) - char_to_lower(b[i]);
        if (result) break;
    }
    return result;
}

bool str_equal(string a, string b) 
{
    bool result = false;
    if (a.len == b.len) 
    {
        result = memcmp(a.ptr, b.ptr, a.len) == 0;
    }
    return result;
}

bool str_equal_ci(string a, string b, bool ignore_case) 
{
    bool result = false;
    if (a.len == b.len) 
    {
        result = memcmp_ci(a.ptr, b.ptr, a.len) == 0;
    }
    return result;
}


static int str_compare_impl(string a, string b, bool ignore_case) 
{
    int cmp = 0;
    usize min_len = MIN(a.len, b.len);

    if (ignore_case)
    {
        cmp = memcmp_ci(a.ptr, b.ptr, min_len);
    }
    else
    {
        cmp = memcmp(a.ptr, b.ptr, min_len);
    }
    
    if (cmp == 0)
    {
        cmp = (int)(a.len - b.len)
    }

    return cmp;
}

int str_compare(void* a, void* b)
{
    str_compare_impl(*(string*)a, *(string*)b, false);
}

int str_compare_ci(void* a, void* b)
{
    str_compare_impl(*(string*)a, *(string*)b, true);
}


bool char_equal(char a, char b, int flags)
{
    if (flags & STR_MATCH_CI) 
    {
        a = char_to_lower(a);
        b = char_to_lower(b);
    }
    return (a == b);
}
    
int str_find(string s, string sub, StringMatchFlags flags)
{
    if (sub.len > s.len)
        return -1;

    if (flags & STR_MATCH_REVERSE)
    {
        // NOTE(bcall): weird decrementing loop since usize can't be negative
        for (usize i = s.len - sub.len + 1; i-- > 0;)
        {
            usize j = 0;

            for (; j < sub.len; j++)
            {
                if (!char_equal(s[i + j], sub[j], flags))
                    break;
            }

            if (j == sub.len)
                return i;
        }
    }
    else
    {
        for (usize i = 0; i <= s.len - sub.len; i++)
        {
            usize j = 0;

            for (; j < sub.len; j++)
            {
                if (!char_equal(s[i + j], sub[j], flags))
                    break;
            }

            if (j == sub.len)
                return i;
        }
    }

    return -1;
}


bool str_contains(string s, string sub, StringMatchFlags flags) 
{
    return str_find(s, sub, flags) != -1;
}

bool str_starts_with(string s, string prefix, StringMatchFlags flags) 
{
    bool result = false;
    if (s.len < prefix.len || prefix.len == 0)
        return 0;

    if (s.len >= prefix.len)
    {
        if (flags & STR_MATCH_CI)
        {
            result = memcmp(s.ptr, prefix.ptr, prefix.len) == 0;
        }
        else if (flags & STR_MATCH_CI)
        {
            result = memcmp_ci(s.ptr, prefix.ptr, prefix.len) == 0;
        }
    }
    return result;
}


/*********************************************************************************/
bool str_ends_with(string s, string suffix, StringMatchFlags flags) 
{
    bool result = false;
    if (s.len < prefix.len || prefix.len == 0)
        return 0;

    if (s.len >= prefix.len)
    {
        if (flags & STR_MATCH_CI)
        {
            result = memcmp(s.ptr + s.len - suffix.len, suffix.ptr, suffix.len) == 0;
        }
        else if (flags & STR_MATCH_CI)
        {
            result = memcmp_ci(s.ptr + s.len - suffix.len, suffix.len) == 0;
        }
    }
    return result;
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

       
