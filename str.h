#ifndef STR_H
#define STR_H

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include "arena.h"
#include "base.h"

/***************************************************************************
 *          MACROS
 ***************************************************************************/

#define str_lit(s)  str_new((char*)(s), sizeof(s) - 1)

#define STR_FMT "%.*s"
#define STR_VARG(s) (int)((s).len), ((s).ptr)

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef struct string string;
struct string 
{ 
    char* ptr; 
    usize len; 
};

typedef int StringFindFlags;
enum
{
    STR_FIND_CI      = (1<<0)
    STR_FIND_REVERSE = (1<<1)
};

// typedef struct StringNode StringNode;
// struct StringNode 
// {
//     StringNode* next;
//     string s;
// };
//
// typedef struct StringList StringList;
// struct StringList
// {
//     StringNode* first;
//     StringNode* last;
//     usize node_count;
//     usize total_size;
// };

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

string  str_new           (char* ptr, size_t len);
string  str_from_cstr     (char* ptr);
string  str_from_range    (char* start, char* one_past_last);
string  str_slice         (string s, usize start_idx, usize len);

bool    str_equal         (string a, string b);
bool    str_equal_ci      (string a, string b);

int     str_compare       (void* a, void* b);
int     str_compare_ci    (void* a, void* b);

int     str_find          (string s, string pattern, StringFindFlags flags);
bool    str_contains      (string s, string sub, StringFindFlags flags);
bool    str_starts_with   (string s, string prefix, StringFindFlags flags);
bool    str_ends_with     (string s, string suffix, StringFindFlags flags);

string  str_trim          (string s);
string  str_trim_left     (string s);
string  str_trim_right    (string s);
string  str_trim_prefix   (string s, string prefix);
string  str_trim_suffix   (string s, string suffix);

string  str_copy          (Arena *arena, string s);
string  str_cat           (Arena *arena, string a, string b);

string  str_to_lower      (Arena *arena, string s);
string  str_to_upper      (Arena *arena, string s);


bool    char_is_space     (char c);
bool    char_is_upper     (char c);
bool    char_is_lower     (char c);
bool    char_is_alpha     (char c);
bool    char_is_slash     (char c);
bool    char_is_digit     (char c, int base);

char    char_to_lower     (char c);
char    char_to_upper     (char c);

usize   cstr_length       (char* c);


#endif // BASE_STRING_H


#ifdef STR_IMPLEMENTATION

