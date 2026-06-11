
#ifndef BASE_STRING_H
#define BASE_STRING_H

#include "base_core.h"
#include "base_arena.h"
#include <stdarg.h>


typedef struct string string;
struct string 
{ 
    char* ptr; 
    uint64_t len; 
};

typedef struct StringNode StringNode;
struct StringNode 
{
    StringNode* next;
    string s;
};

typedef struct StringList StringList;
struct StringList
{
    StringNode* first;
    StringNode* last;
    uint64_t node_count;
    // uint64_t total_size;
};


#define STR_LIT(s)  str_new((char*)(s), sizeof(s) - 1)
#define STR_VARG(s) (int)((s).len), ((s).ptr)

string     str_new(char* ptr, uint64_t len);
string     str_from_cstr(char* ptr);

bool32     str_equal(string a, string b);
int32_t    str_compare(string a, string b);

bool32     str_contains(string s, string pattern);
bool32     str_starts_with(string s, string prefix);
bool32     str_ends_with(string s, string suffix);

int32_t    str_find_idx_first(string s, string pattern);
int32_t    str_find_idx_last(string s, string pattern);

string     str_slice(string s, uint64_t start, uint64_t end);
string     str_slice_len(string s, uint64_t start, uint64_t length);

string     str_remove_prefix(string s, string prefix);
string     str_remove_suffix(string s, string suffix);

string     str_trim(string s);
string     str_trim_left(string s);
string     str_trim_right(string s);

string     str_copy(Arena *arena, string s);
string     str_cat(Arena *arena, string a, string b);

string     str_printfv(Arena *arena, char* fmt, va_list args);
string     str_printf(Arena *arena, char *fmt, ...);

string     str_to_upper(Arena *arena, string string);
string     str_to_lower(Arena *arena, string string);

StringList str_split(Arena *arena, string s, string delim);
StringList str_split_skip_empty(Arena *arena, string s, string delim);

bool32     char_is_space(char c);
bool32     char_is_upper(char c);
bool32     char_is_lower(char c);
bool32     char_is_alpha(char c);
bool32     char_is_slash(char c);
bool32     char_is_digit(char c, uint32_t base);
char       char_to_lower(char c);
char       char_to_upper(char c);

uint64_t   cstr_length(char* c);

#endif // BASE_STRING_H

