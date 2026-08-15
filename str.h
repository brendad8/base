#ifndef BASE_STRING_H
#define BASE_STRING_H

//***************************************************************************
//          INCLUDE_FILES
//***************************************************************************

#include "base_core.h"
#include "base_arena.h"
#include <stdarg.h>

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct string string;
struct string 
{ 
    char* ptr; 
    size_t len; 
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
    size_t node_count;
    // size_t total_size;
};

//***************************************************************************
//          MACROS
//***************************************************************************

#define STR_LIT(s)  str_new((char*)(s), sizeof(s) - 1)
#define STR_VARG(s) (int)((s).len), ((s).ptr)

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

string      str_new                  (char* ptr, size_t len);
string      str_from_cstr            (char* ptr);

bool        str_equal                (string a, string b);
bool        str_equal_ci             (string a, string b);
int         str_compare              (string a, string b);
int         str_compare_ci           (string a, string b);

bool        str_contains             (string s, string pattern);
bool        str_contains_ci          (string s, string pattern);

bool        str_starts_with          (string s, string prefix);
bool        str_starts_with_ci       (string s, string prefix);
bool        str_ends_with            (string s, string suffix);
bool        str_ends_with_ci         (string s, string suffix);

int         str_find_idx_first       (string s, string pattern);
int         str_find_idx_first_ci    (string s, string pattern);
int         str_find_idx_last        (string s, string pattern);
int         str_find_idx_last_ci     (string s, string pattern);

string      str_slice                (string s, size_t start, size_t end);
string      str_slice_len            (string s, size_t start, size_t length);

string      str_remove_prefix        (string s, string prefix);
string      str_remove_suffix        (string s, string suffix);

string      str_trim                 (string s);
string      str_trim_left            (string s);
string      str_trim_right           (string s);

string      str_copy                 (Arena *arena, string s);
string      str_cat                  (Arena *arena, string a, string b);

string      str_printfv              (Arena *arena, char* fmt, va_list args);
string      str_printf               (Arena *arena, char *fmt, ...);

string      str_to_lower             (Arena *arena, string s);
string      str_to_upper             (Arena *arena, string s);

StringList  str_split                (Arena *arena, string s, string delim);
StringList  str_split_skip_empty     (Arena *arena, string s, string delim);

bool        char_is_space            (char c);
bool        char_is_upper            (char c);
bool        char_is_lower            (char c);
bool        char_is_alpha            (char c);
bool        char_is_slash            (char c);
bool        char_is_digit            (char c, uint32_t base);
char        char_to_lower            (char c);
char        char_to_upper            (char c);

size_t      cstr_length              (char* c);


#endif // BASE_STRING_H


#ifdef STR_IMPLEMENTATION

