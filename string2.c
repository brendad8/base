
#include <stdbool>

#define STR_ABS(x) ((x) < (0) ? -(x) : (x))
#define STR_MIN(a,b) ((a) < (b) ? (a) : (b))
#define STR_MAX(a,b) ((a) < (b) ? (b) : (a))

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

int char_symbol_to_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;

    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;

    return -1;
}
nt char_to_int_table[] = {
    ['0'] = 0,
    ['1'] = 1,
    ['2'] = 2,
    ['3'] = 3,
    ['4'] = 4,
    ['5'] = 5,
    ['6'] = 6,
    ['7'] = 7,
    ['8'] = 8,
    ['9'] = 9,

    ['A'] = 10,
    ['B'] = 11,
    ['C'] = 12,
    ['D'] = 13,
    ['E'] = 14,
    ['F'] = 15,

    ['a'] = 10,
    ['b'] = 11,
    ['c'] = 12,
    ['d'] = 13,
    ['e'] = 14,
    ['f'] = 15,
};

bool char_is_digit(char c, int base)
{
    bool result = false;
    if (0 < base && base <= 16)
    {
        char val = char_to_int_table[c];
        if (val < base)
        {
            result = true;
        }
  }
  return result;
}

char char_to_lower(char c)
{
    if(char_is_upper(c))
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


uint64_t cstr_length(char* c)
{
    uint64_t length = 0;
    if(c)
    {
        char* p = c;
        for (; *p != 0; p += 1);
        length = (U64)(p - c);
    }
    return length;
}

String8 str8(char* str, uint64_t size)
{
  String8 result = {str, size};
  return result;
}

String8 str8_from_range(char* first, char* one_past_last)
{
    String8 result = {first, (uint64_t)(one_past_last - first)};
    return result;
}

String8 str8_zero(void)
{
  String8 result = {0};
  return result;
}

String8 str8_from_cstr(char *c)
{
    String8 result = {c, cstring8_length(c)};
    return result;
}

String8 str8_to_upper(Arena* arena, String8 string)
{
    string.ptr = arena_push_array_no_zero(arena, char, string.len + 1);
    for (uint64_t idx = 0; idx < string.len; idx += 1)
    {
        string.ptr[idx] = char_to_upper(string.ptr[idx]);
    }
    string[string.len] = '\0';
    return string;
}

String8 str8_to_upper(Arena* arena, String8 string)
{
    string.ptr = arena_push_array_no_zero(arena, char, string.len + 1);
    for (uint64_t idx = 0; idx < string.len; idx += 1)
    {
        string.ptr[idx] = char_to_lower(string.ptr[idx]);
    }
    string[string.len] = '\0';
    return string;
}

String8 str8_replace_char(Arena* arena, String8 string, char to_replace, char to_fill)
{
    string.ptr = arena_push_array_no_zero(arena, char, string.len + 1);
    for (uint64_t idx = 0; idx < string.len; idx += 1)
    {
        if (string.ptr[idx] == to_replace)
            string.ptr[idx] = to_fill;
    }
    string[string.len] = '\0';
    return string;
}

int memcmp_ci(char* a, char* b, uint64_t size)
{
    int result = 0;
    for (uint64_t i = 0; i < size; i++)
    {
        result += char_to_lower(a[i]) - char_to_lower(b[i]);
        if (result) break;
    }
    return result;
}


bool str8_equal(String8 a, String8 b, StringMatchFlags flags)
{
    bool result = false;
    if(a.len == b.len && flags == 0)
    {
        result = memcmp(a.ptr, b.ptr, b.len) == 0;
    }
    else if (a.len == b.len && flags == STRING_MATCH_CI)
    {
        result = memcmp_ci(a.ptr, b.ptr, a.len) == 0;
    }
    return result;
}

bool char_equal(char a, char b, StringMatchFlags flags)
{
    char at = a;
    char bt = b;
    if (flags & STRING_MATCH_CI) 
    {
        at = char_to_lower(at);
        bt = char_to_lower(bt);
    }
    return (at == bt);
}


bool str8_starts_with(String8 string, String8 prefix)
{
    return str8_match(str8_prefix(string, prefix.len), prefix, 0);
}

bool str8_starts_withi(String8 string, String8 prefix)
{
    return str8_match(str8_prefix(string, prefix.len), prefix, STRING_MATCH_CI);
}

uint64_t str8_find_sub(String8 string, uint64_t start_pos, String8 substr, StringMatchFlags flags)
{
    char* p = string.ptr + start_pos;
    uint64_t stop_offset = STR_MAX(string.len + 1, substr.len) - substr.len;
    char* p_stop = string.ptr + stop_offset;

    if (substr.len > 0)
    {
        char* string_opl = string.ptr + string.len;
        String8 substr = str8_skip(substr, 1);

        StringMatchFlags adjusted_flags = flags | StringMatchFlag_RightSideSloppy;
        U8 substr = substr.ptr[0];
        if (adjusted_flags & StringMatchFlag_CaseInsensitive)
        {
            needle_first_char_adjusted = upper_from_char(needle_first_char_adjusted);
        }
        if (adjusted_flags & StringMatchFlag_SlashInsensitive)
        {
            needle_first_char_adjusted = correct_slash_from_char(needle_first_char_adjusted);
        }
        for (; p < stop_p; p += 1)
        {
            U8 haystack_char_adjusted = *p;
            if(adjusted_flags & StringMatchFlag_CaseInsensitive)
            {
                haystack_char_adjusted = upper_from_char(haystack_char_adjusted);
            }
            if(adjusted_flags & StringMatchFlag_SlashInsensitive)
            {
                haystack_char_adjusted = correct_slash_from_char(haystack_char_adjusted);
            }
            if(haystack_char_adjusted == needle_first_char_adjusted)
            {
                if(str8_match(str8_range(p + 1, string_opl), needle_tail, adjusted_flags))
                {
                    break;
                }
            }
        }
    }
    U64 result = string.len;
    if(p < stop_p)
    {
    result = (U64)(p - string.ptr);
    }
    return result;
}

U64
str8_find_needle_reverse(String8 string, U64 start_pos, String8 needle, StringMatchFlags flags)
{
  U64 result = 0;
  for(S64 i = string.len - start_pos - needle.len; i >= 0; --i)
  {
    String8 haystack = str8_substr(string, rng_1u64(i, i + needle.len));
    if(str8_match(haystack, needle, flags))
    {
      result = (U64)i + needle.len;
      break;
    }
  }
  return result;
}

B32
str8_is_before(String8 a, String8 b)
{
  B32 result = 0;
  {
    U64 common_size = Min(a.len, b.len);
    for(U64 off = 0; off < common_size; off += 1)
    {
      if(a.ptr[off] < b.ptr[off])
      {
        result = 1;
        break;
      }
      else if(a.ptr[off] > b.ptr[off])
      {
        result = 0;
        break;
      }
      else if(off+1 == common_size)
      {
        result = (a.len < b.len);
      }
    }
  }
  return result;
}


String8 str8_substr(String8 str, size_t start, size_t end)
{
    start = STR_MIN(start, str.len);
    end = STR_MIN(end, str.len);
    str.ptr += range.min;
    str.len = STR_ABS(end - start);
    return str;
}

String8 str8_prefix(String8 str, size_t size)
{
    str.len = STR_MIN(size, str.len);
    return str;
}

String8
str8_skip(String8 str, U64 amt)
{
  amt = ClampTop(amt, str.len);
  str.ptr += amt;
  str.len -= amt;
  return str;
}

String8
str8_postfix(String8 str, U64 size)
{
  size = ClampTop(size, str.len);
  str.ptr = (str.ptr + str.len) - size;
  str.len = size;
  return str;
}

String8
str8_chop(String8 str, U64 amt)
{
  amt = ClampTop(amt, str.len);
  str.len -= amt;
  return str;
}

String8
str8_chop_line(String8 *str)
{
  U64     new_line_pos = str8_find_needle(*str, 0, str8_lit("\n"), 0);
  String8 line         = str8_prefix(*str, new_line_pos);
  if (str8_ends_with(line, str8_lit("\r"), 0)) {
    line = str8_chop(line, 1);
  }
  *str = str8_skip(*str, new_line_pos + 1);
  return line;
}

String8
str8_skip_chop_whitespace(String8 string)
{
  U8 *first = string.ptr;
  U8 *opl = first + string.len;
  for(;first < opl; first += 1)
  {
    if(!char_is_space(*first))
    {
      break;
    }
  }
  for(;opl > first;)
  {
    opl -= 1;
    if(!char_is_space(*opl))
    {
      opl += 1;
      break;
    }
  }
  String8 result = str8_range(first, opl);
  return result;
}

String8
str8_skip_chop_slashes(String8 string)
{
  U8 *first = string.ptr;
  U8 *opl = first + string.len;
  for(;first < opl; first += 1)
  {
    if(!char_is_slash(*first))
    {
      break;
    }
  }
  for(;opl > first;)
  {
    opl -= 1;
    if(!char_is_slash(*opl))
    {
      opl += 1;
      break;
    }
  }
  String8 result = str8_range(first, opl);
  return result;
}

////////////////////////////////
//~ rjf: String Formatting & Copying

String8
str8_cat(Arena *arena, String8 s1, String8 s2)
{
  String8 str;
  str.len = s1.len + s2.len;
  str.ptr = push_array_no_zero(arena, U8, str.len + 1);
  MemoryCopy(str.ptr, s1.ptr, s1.len);
  MemoryCopy(str.ptr + s1.len, s2.ptr, s2.len);
  str.ptr[str.len] = 0;
  return str;
}

String8
str8_copy(Arena *arena, String8 s)
{
  String8 str;
  str.len = s.len;
  str.ptr = push_array_no_zero(arena, U8, str.len + 1);
  MemoryCopy(str.ptr, s.ptr, s.len);
  str.ptr[str.len] = 0;
  return str;
}

String8
str8fv(Arena *arena, char *fmt, va_list args){
  va_list args2;
  va_copy(args2, args);
  U32 needed_bytes = raddbg_vsnprintf(0, 0, fmt, args) + 1;
  String8 result = {0};
  result.ptr = push_array_no_zero(arena, U8, needed_bytes);
  result.len = raddbg_vsnprintf((char*)result.ptr, needed_bytes, fmt, args2);
  result.ptr[result.len] = 0;
  va_end(args2);
  return result;
}

String8
str8f(Arena *arena, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  String8 result = push_str8fv(arena, fmt, args);
  va_end(args);
  return result;
}

String8
push_cstr(Arena *arena, String8 str)
{
  U64 buffer_size = str.len + 1;
  U8 *buffer = push_array_no_zero(arena, U8, buffer_size);
  MemoryCopy(buffer, str.ptr, str.len);
  buffer[str.len] = 0;
  String8 result = str8(buffer, buffer_size);
  return result;
}

////////////////////////////////
//~ rjf: String <=> Integer Conversions

//- rjf: string -> integer

S64
sign_from_str8(String8 string, String8 *string_tail)
{
  // count negative signs
  U64 neg_count = 0;
  U64 i = 0;
  for(; i < string.len; i += 1)
  {
    if (string.ptr[i] == '-'){
      neg_count += 1;
    }
    else if (string.ptr[i] != '+'){
      break;
    }
  }
  
  // output part of string after signs
  *string_tail = str8_skip(string, i);
  
  // output integer sign
  S64 sign = (neg_count & 1)?-1:+1;
  return sign;
}

B32
str8_is_integer(String8 string, U32 radix)
{
  B32 result = 0;
  if(string.len > 0)
  {
    if(1 < radix && radix <= 16)
    {
      result = 1;
      for(U64 i = 0; i < string.len; i += 1)
      {
        U8 c = string.ptr[i];
        if(!(c < 0x80) || integer_symbol_reverse[c] >= radix)
        {
          result = 0;
          break;
        }
      }
    }
  }
  return result;
}

B32
str8_is_integer_signed(String8 string, U32 radix)
{
  B32 result = 0;
  String8 sign = str8_prefix(string, 1);
  if(str8_match(sign, str8_lit("-"), 0))
  {
    result = str8_is_integer(str8_skip(string, 1), radix);
  }
  else
  {
    result = str8_is_integer(string, radix);
  }
  return result;
}

U64
u64_from_str8(String8 string, U32 radix)
{
  U64 x = 0;
  if(1 < radix && radix <= 16)
  {
    for(U64 i = 0; i < string.len; i += 1)
    {
      x *= radix;
      x += integer_symbol_reverse[string.ptr[i]&0x7F];
    }
  }
  return x;
}

S64
s64_from_str8(String8 string, U32 radix)
{
  S64 sign = sign_from_str8(string, &string);
  S64 x = (S64)u64_from_str8(string, radix) * sign;
  return x;
}

U32
u32_from_str8(String8 string, U32 radix)
{
  U64 x64 = u64_from_str8(string, radix);
  U32 x32 = safe_cast_u32(x64);
  return x32;
}

S32
s32_from_str8(String8 string, U32 radix)
{
  S64 x64 = s64_from_str8(string, radix);
  S32 x32 = safe_cast_s32(x64);
  return x32;
}

B32
try_u64_from_str8_c_rules(String8 string, U64 *x)
{
  // rjf: unpack radix / prefix size based on string prefix
  U64 radix = 0;
  U64 prefix_size = 0;
  {
    // hex
    if(str8_match(str8_prefix(string, 2), str8_lit("0x"), StringMatchFlag_CaseInsensitive))
    {
      radix = 0x10, prefix_size = 2;
    }
    // binary
    else if(str8_match(str8_prefix(string, 2), str8_lit("0b"), StringMatchFlag_CaseInsensitive))
    {
      radix = 2, prefix_size = 2;
    }
    // octal
    else if(str8_match(str8_prefix(string, 1), str8_lit("0"), StringMatchFlag_CaseInsensitive) && string.len > 1)
    {
      radix = 010, prefix_size = 1;
    }
    // decimal
    else
    {
      radix = 10, prefix_size = 0;
    }
  }
  
  // rjf: convert if we can
  String8 integer    = str8_skip(string, prefix_size);
  B32     is_integer = str8_is_integer(integer, radix);
  if(is_integer)
  {
    *x = u64_from_str8(integer, radix);
  }
  
  return is_integer;
}

B32
try_s64_from_str8_c_rules(String8 string, S64 *x)
{
  String8 string_tail = {0};
  S64 sign = sign_from_str8(string, &string_tail);
  U64 x_u64 = 0;
  B32 is_integer = try_u64_from_str8_c_rules(string_tail, &x_u64);
  *x = x_u64*sign;
  return is_integer;
}

//- rjf: integer -> string

String8
str8_from_memory_size(Arena *arena, U64 size)
{
  String8 result = {0};
  {
    if(size < KB(1))
    {
      result = push_str8f(arena, "%llu byte%s", size, size == 1 ? "" : "s");
    }
    else if(size < MB(1))
    {
      result = push_str8f(arena, "%llu.%02llu KiB", size / KB(1), ((size * 100) / KB(1)) % 100);
    }
    else if(size < GB(1))
    {
      result = push_str8f(arena, "%llu.%02llu MiB", size / MB(1), ((size * 100) / MB(1)) % 100);
    }
    else if(size < TB(1))
    {
      result = push_str8f(arena, "%llu.%02llu GiB", size / GB(1), ((size * 100) / GB(1)) % 100);
    }
    else
    {
      result = push_str8f(arena, "%llu.%02llu TiB", size / TB(1), ((size * 100) / TB(1)) % 100);
    }
  }
  return result;
}

String8
str8_from_count(Arena *arena, U64 count)
{
  String8 result = {0};
  {
    if(count < 1 * 1000)
    {
      result = push_str8f(arena, "%llu", count);
    }
    else if(count < 1000000)
    {
      U64 frac = ((count * 100) / 1000) % 100;
      if(frac > 0)
      {
        result = push_str8f(arena, "%llu.%02lluK", count / 1000, frac);
      }
      else
      {
        result = push_str8f(arena, "%lluK", count / 1000);
      }
    }
    else if(count < 1000000000)
    {
      U64 frac = ((count * 100) / 1000000) % 100;
      if(frac > 0)
      {
        result = push_str8f(arena, "%llu.%02lluM", count / 1000000, frac);
      }
      else
      {
        result = push_str8f(arena, "%lluM", count / 1000000);
      }
    }
    else
    {
      U64 frac = ((count * 100) * 1000000000) % 100;
      if(frac > 0)
      {
        result = push_str8f(arena, "%llu.%02lluB", count / 1000000000, frac);
      }
      else
      {
        result = push_str8f(arena, "%lluB", count / 1000000000, frac);
      }
    }
  }
  return result;
}

String8
str8_from_bits_u32(Arena *arena, U32 x)
{
  U8 c0 = 'a' + ((x >> 28) & 0xf);
  U8 c1 = 'a' + ((x >> 24) & 0xf);
  U8 c2 = 'a' + ((x >> 20) & 0xf);
  U8 c3 = 'a' + ((x >> 16) & 0xf);
  U8 c4 = 'a' + ((x >> 12) & 0xf);
  U8 c5 = 'a' + ((x >>  8) & 0xf);
  U8 c6 = 'a' + ((x >>  4) & 0xf);
  U8 c7 = 'a' + ((x >>  0) & 0xf);
  String8 result = push_str8f(arena, "%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7);
  return result;
}

String8
str8_from_bits_u64(Arena *arena, U64 x)
{
  U8 c0 = 'a' + ((x >> 60) & 0xf);
  U8 c1 = 'a' + ((x >> 56) & 0xf);
  U8 c2 = 'a' + ((x >> 52) & 0xf);
  U8 c3 = 'a' + ((x >> 48) & 0xf);
  U8 c4 = 'a' + ((x >> 44) & 0xf);
  U8 c5 = 'a' + ((x >> 40) & 0xf);
  U8 c6 = 'a' + ((x >> 36) & 0xf);
  U8 c7 = 'a' + ((x >> 32) & 0xf);
  U8 c8 = 'a' + ((x >> 28) & 0xf);
  U8 c9 = 'a' + ((x >> 24) & 0xf);
  U8 ca = 'a' + ((x >> 20) & 0xf);
  U8 cb = 'a' + ((x >> 16) & 0xf);
  U8 cc = 'a' + ((x >> 12) & 0xf);
  U8 cd = 'a' + ((x >>  8) & 0xf);
  U8 ce = 'a' + ((x >>  4) & 0xf);
  U8 cf = 'a' + ((x >>  0) & 0xf);
  String8 result = push_str8f(arena,
                              "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                              c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf);
  return result;
}

String8
str8_from_u64(Arena *arena, U64 u64, U32 radix, U8 min_digits, U8 digit_group_separator)
{
  String8 result = {0};
  {
    // rjf: prefix
    String8 prefix = {0};
    switch(radix)
    {
      case 16:{prefix = str8_lit("0x");}break;
      case 8: {prefix = str8_lit("0o");}break;
      case 2: {prefix = str8_lit("0b");}break;
    }
    
    // rjf: determine # of chars between separators
    U8 digit_group_size = 3;
    switch(radix)
    {
      default:break;
      case 2:
      case 8:
      case 16:
      {digit_group_size = 4;}break;
    }
    
    // rjf: prep
    U64 needed_leading_0s = 0;
    {
      U64 needed_digits = 1;
      {
        U64 u64_reduce = u64;
        for(;;)
        {
          u64_reduce /= radix;
          if(u64_reduce == 0)
          {
            break;
          }
          needed_digits += 1;
        }
      }
      needed_leading_0s = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
      U64 needed_separators = 0;
      if(digit_group_separator != 0)
      {
        needed_separators = (needed_digits+needed_leading_0s)/digit_group_size;
        if(needed_separators > 0 && (needed_digits+needed_leading_0s)%digit_group_size == 0)
        {
          needed_separators -= 1;
        }
      }
      result.len = prefix.len + needed_leading_0s + needed_separators + needed_digits;
      result.ptr = push_array_no_zero(arena, U8, result.len + 1);
      result.ptr[result.len] = 0;
    }
    
    // rjf: fill contents
    {
      U64 u64_reduce = u64;
      U64 digits_until_separator = digit_group_size;
      for(U64 idx = 0; idx < result.len; idx += 1)
      {
        if(digits_until_separator == 0 && digit_group_separator != 0)
        {
          result.ptr[result.len - idx - 1] = digit_group_separator;
          digits_until_separator = digit_group_size+1;
        }
        else
        {
          result.ptr[result.len - idx - 1] = lower_from_char(integer_symbols[u64_reduce%radix]);
          u64_reduce /= radix;
        }
        digits_until_separator -= 1;
        if(u64_reduce == 0)
        {
          break;
        }
      }
      for(U64 leading_0_idx = 0; leading_0_idx < needed_leading_0s; leading_0_idx += 1)
      {
        result.ptr[prefix.len + leading_0_idx] = '0';
      }
    }
    
    // rjf: fill prefix
    if(prefix.len != 0)
    {
      MemoryCopy(result.ptr, prefix.ptr, prefix.len);
    }
  }
  return result;
}

String8
str8_from_s64(Arena *arena, S64 s64, U32 radix, U8 min_digits, U8 digit_group_separator)
{
  String8 result = {0};
  // TODO(rjf): preeeeetty sloppy...
  if(s64 < 0)
  {
    Temp scratch = scratch_begin(&arena, 1);
    String8 numeric_part = str8_from_u64(scratch.arena, (U64)(-s64), radix, min_digits, digit_group_separator);
    result = push_str8f(arena, "-%S", numeric_part);
    scratch_end(scratch);
  }
  else
  {
    result = str8_from_u64(arena, (U64)s64, radix, min_digits, digit_group_separator);
  }
  return result;
}

////////////////////////////////
//~ rjf: String <=> Float Conversions

F64
f64_from_str8(String8 string)
{
  // TODO(rjf): crappy implementation for now that just uses atof.
  F64 result = 0;
  if(string.len > 0)
  {
    // rjf: find starting pos of numeric string, as well as sign
    F64 sign = +1.0;
    if(string.ptr[0] == '-')
    {
      sign = -1.0;
    }
    else if(string.ptr[0] == '+')
    {
      sign = 1.0;
    }
    
    // rjf: gather numerics
    U64 num_valid_chars = 0;
    char buffer[64];
    B32 exp = 0;
    for(U64 idx = 0; idx < string.len && num_valid_chars < sizeof(buffer)-1; idx += 1)
    {
      if(char_is_digit(string.ptr[idx], 10) || string.ptr[idx] == '.' || string.ptr[idx] == 'e' ||
         (exp && (string.ptr[idx] == '+' || string.ptr[idx] == '-')))
      {
        buffer[num_valid_chars] = string.ptr[idx];
        num_valid_chars += 1;
        exp = 0;
        exp = (string.ptr[idx] == 'e');
      }
    }
    
    // rjf: null-terminate (the reason for all of this!!!!!!)
    buffer[num_valid_chars] = 0;
    
    // rjf: do final conversion
    result = sign * atof(buffer);
  }
  return result;
}

////////////////////////////////
//~ rjf: String List Construction Functions

String8Node *
str8_list_push_node(String8List *list, String8Node *node)
{
  SLLQueuePush(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += node->string.len;
  return node;
}

String8Node *
str8_list_push_node_set_string(String8List *list, String8Node *node, String8 string)
{
  SLLQueuePush(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += string.len;
  node->string = string;
  return node;
}

String8Node *
str8_list_push_node_front(String8List *list, String8Node *node)
{
  SLLQueuePushFront(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += node->string.len;
  return node;
}

String8Node *
str8_list_push_node_front_set_string(String8List *list, String8Node *node, String8 string)
{
  SLLQueuePushFront(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += string.len;
  node->string = string;
  return node;
}

String8Node *
str8_list_push(Arena *arena, String8List *list, String8 string)
{
  String8Node *node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_set_string(list, node, string);
  return node;
}

String8Node *
str8_list_push_front(Arena *arena, String8List *list, String8 string)
{
  String8Node *node = push_array_no_zero(arena, String8Node, 1);
  str8_list_push_node_front_set_string(list, node, string);
  return node;
}

void
str8_list_concat_in_place(String8List *list, String8List *to_push)
{
  if(to_push->node_count != 0)
  {
    if(list->last)
    {
      list->node_count += to_push->node_count;
      list->total_size += to_push->total_size;
      list->last->next = to_push->first;
      list->last = to_push->last;
    }
    else
    {
      *list = *to_push;
    }
    MemoryZeroStruct(to_push);
  }
}

String8Node*
str8_list_push_aligner(Arena *arena, String8List *list, U64 min, U64 align)
{
  read_only local_persist U8 zeroes[64] = {0};
  Assert(IsPow2OrZero(align));
  U64 pad = Max(min, AlignPadPow2(list->total_size, align));
  if(pad < sizeof(zeroes))
  {
    return str8_list_push(arena, list, str8(zeroes, pad));
  }
  else
  {
    return str8_list_push(arena, list, str8(push_array(arena, U8, pad), pad));
  }
}

String8Node*
str8_list_pushf(Arena *arena, String8List *list, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  String8 string = push_str8fv(arena, fmt, args);
  String8Node *result = str8_list_push(arena, list, string);
  va_end(args);
  return result;
}

String8Node*
str8_list_push_frontf(Arena *arena, String8List *list, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  String8 string = push_str8fv(arena, fmt, args);
  String8Node *result = str8_list_push_front(arena, list, string);
  va_end(args);
  return result;
}

String8Node*
str8_list_pop_front(String8List *list)
{
  String8Node *node = 0;
  if(list->node_count)
  {
    node = list->first;
    Assert(list->total_size >= list->first->string.len);
    list->node_count -= 1;
    list->total_size -= list->first->string.len;
    SLLQueuePop(list->first, list->last);
  }
  return node;
}

String8List
str8_list_copy(Arena *arena, String8List *list)
{
  String8List result = {0};
  for(String8Node *node = list->first; node != 0; node = node->next)
  {
    String8Node *new_node = push_array_no_zero(arena, String8Node, 1);
    String8 new_string = push_str8_copy(arena, node->string);
    str8_list_push_node_set_string(&result, new_node, new_string);
  }
  return result;
}

String8List
str8_list_substr(Arena *arena, String8List list, Rng1U64 range)
{
  String8List result = {0};
  
  String8Node *n = list.first;
  
  U64 front_min = 0;
  {
    U64 cursor = 0;
    for (; n != 0; cursor += n->string.len, n = n->next) {
      if (cursor + n->string.len > range.min) {
        front_min = range.min - cursor;
        break;
      }
    }
  }
  
  if (front_min > 0) {
    U64 front_max = front_min + Min(dim_1u64(range), n->string.len);
    str8_list_push(arena, &result, str8_substr(n->string, r1u64(front_min, front_max)));
    n = n->next;
  }
  
  for (; n != 0; n = n->next) {
    if (result.total_size >= dim_1u64(range)) {
      break;
    }
    U64 copy_max  = dim_1u64(range) - result.total_size;
    U64 copy_size = Min(copy_max, n->string.len);
    str8_list_push(arena, &result, str8_substr(n->string, r1u64(0, copy_size)));
  }
  
  return result;
}

////////////////////////////////
//~ rjf: String Splitting & Joining

String8List
str8_split(Arena *arena, String8 string, U8 *split_chars, U64 split_char_count, StringSplitFlags flags)
{
  String8List list = {0};
  B32 keep_empties = (flags & StringSplitFlag_KeepEmpties);
  U8 *ptr = string.ptr;
  U8 *opl = string.ptr + string.len;
  for(;ptr < opl;)
  {
    U8 *first = ptr;
    for(;ptr < opl; ptr += 1)
    {
      U8 c = *ptr;
      B32 is_split = 0;
      for(U64 i = 0; i < split_char_count; i += 1)
      {
        if(split_chars[i] == c)
        {
          is_split = 1;
          break;
        }
      }
      if(is_split)
      {
        break;
      }
    }
    String8 string = str8_range(first, ptr);
    if(keep_empties || string.len > 0)
    {
      str8_list_push(arena, &list, string);
    }
    ptr += 1;
  }
  return list;
}

String8List
str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags)
{
  String8List list = str8_split(arena, string, split_chars.ptr, split_chars.len, flags);
  return list;
}

String8
str8_list_join(Arena *arena, String8List *list, StringJoin *optional_params)
{
  StringJoin join = {0};
  if(optional_params != 0)
  {
    MemoryCopyStruct(&join, optional_params);
  }
  U64 sep_count = 0;
  if(list->node_count > 0)
  {
    sep_count = list->node_count - 1;
  }
  String8 result;
  result.len = join.pre.len + join.post.len + sep_count*join.sep.len + list->total_size;
  U8 *ptr = result.ptr = push_array_no_zero(arena, U8, result.len + 1);
  MemoryCopy(ptr, join.pre.ptr, join.pre.len);
  ptr += join.pre.len;
  for(String8Node *node = list->first;
      node != 0;
      node = node->next)
  {
    MemoryCopy(ptr, node->string.ptr, node->string.len);
    ptr += node->string.len;
    if(node->next != 0)
    {
      MemoryCopy(ptr, join.sep.ptr, join.sep.len);
      ptr += join.sep.len;
    }
  }
  MemoryCopy(ptr, join.post.ptr, join.post.len);
  ptr += join.post.len;
  *ptr = 0;
  return result;
}

////////////////////////////////
//~ rjf: Basic Data Structure Stringification Helpers

String8List
numeric_str8_list_from_data(Arena *arena, U32 radix, String8 data, U64 stride)
{
  String8List strs = {0};
  U64 count = data.len/stride;
  for EachIndex(idx, count)
  {
    U64 val = 0;
    MemoryCopy(&val, data.ptr + idx*stride, stride);
    str8_list_push(arena, &strs, str8_from_u64(arena, val, radix, 0, 0));
  }
  return strs;
}

////////////////////////////////
//~ rjf; String Arrays

String8Array
str8_array_zero(void)
{
  String8Array result = {0};
  return result;
}

String8Array
str8_array_from_list(Arena *arena, String8List *list)
{
  String8Array array;
  array.count   = list->node_count;
  array.v = push_array_no_zero(arena, String8, array.count);
  U64 idx = 0;
  for(String8Node *n = list->first; n != 0; n = n->next, idx += 1)
  {
    array.v[idx] = n->string;
  }
  return array;
}

String8Array *
str8_array_from_list_arr(Arena *arena, String8List *lists, U64 count)
{
  String8Array *result = push_array(arena, String8Array, count);
  for (U64 idx = 0; idx < count; idx += 1) {
    result[idx] = str8_array_from_list(arena, &lists[idx]);
  }
  return result;
}

String8Array
str8_array_reserve(Arena *arena, U64 count)
{
  String8Array arr;
  arr.count = 0;
  arr.v = push_array(arena, String8, count);
  return arr;
}

String8Array
str8_array_copy(Arena *arena, String8Array array)
{
  String8Array result = {0};
  result.count = array.count;
  result.v = push_array(arena, String8, result.count);
  for EachIndex(idx, result.count)
  {
    result.v[idx] = push_str8_copy(arena, array.v[idx]);
  }
  return result;
}

////////////////////////////////
//~ rjf: String Version Helpers

U64
version_from_str8(String8 string)
{
  U64 result = 0;
  Temp scratch = scratch_begin(0, 0);
  U64 version_major = 0;
  U64 version_minor = 0;
  U64 version_patch = 0;
  String8List version_parts = str8_split(scratch.arena, string, (U8 *)".", 1, 0);
  if(version_parts.first &&
     version_parts.first->next &&
     version_parts.first->next->next)
  {
    try_u64_from_str8_c_rules(version_parts.first->string, &version_major);
    try_u64_from_str8_c_rules(version_parts.first->next->string, &version_minor);
    try_u64_from_str8_c_rules(version_parts.first->next->next->string, &version_patch);
    result = Version(version_major, version_minor, version_patch);
  }
  scratch_end(scratch);
  return result;
}

String8
str8_from_version(Arena *arena, U64 version)
{
  U64 version_major = MajorFromVersion(version);
  U64 version_minor = MinorFromVersion(version);
  U64 version_patch = PatchFromVersion(version);
  String8 result = str8f(arena, "%I64d.%I64d.%I64d", version_major, version_minor, version_patch);
  return result;
}

////////////////////////////////
//~ rjf: String Path Helpers

String8
program_ext_postfix_from_os(OperatingSystem os, B32 require_ext)
{
  String8 result = {0};
  switch(os)
  {
    default:{}break;
    case OperatingSystem_Windows:
    {
      result = s(".exe");
    }break;
    case OperatingSystem_Linux:
    if(require_ext)
    {
      result = s(".elf");
    }break;
    case OperatingSystem_Mac:
    if(require_ext)
    {
      result = s(".macho");
    }break;
  }
  return result;
}

String8
str8_chop_last_slash(String8 string)
{
  if(string.len > 0)
  {
    U8 *ptr = string.ptr + string.len - 1;
    for(;ptr >= string.ptr; ptr -= 1)
    {
      if(*ptr == '/' || *ptr == '\\')
      {
        break;
      }
    }
    if(ptr >= string.ptr)
    {
      string.len = (U64)(ptr - string.ptr);
    }
    else
    {
      string.len = 0;
    }
  }
  return string;
}

String8
str8_skip_last_slash(String8 string)
{
  if(string.len > 0)
  {
    U8 *ptr = string.ptr + string.len - 1;
    for(;ptr >= string.ptr; ptr -= 1)
    {
      if(*ptr == '/' || *ptr == '\\')
      {
        break;
      }
    }
    if(ptr >= string.ptr)
    {
      ptr += 1;
      string.len = (U64)(string.ptr + string.len - ptr);
      string.ptr = ptr;
    }
  }
  return string;
}

String8
str8_chop_last_dot(String8 string)
{
  String8 result = string;
  U64 p = string.len;
  for(;p > 0;)
  {
    p -= 1;
    if(string.ptr[p] == '.')
    {
      result = str8_prefix(string, p);
      break;
    }
  }
  return result;
}

String8
str8_skip_last_dot(String8 string)
{
  String8 result = string;
  U64 p = string.len;
  for(;p > 0;)
  {
    p -= 1;
    if(string.ptr[p] == '.')
    {
      result = str8_skip(string, p + 1);
      break;
    }
  }
  return result;
}

PathStyle
path_style_from_str8(String8 string)
{
  PathStyle result = PathStyle_Relative;
  if(string.len >= 1 && string.ptr[0] == '/')
  {
    result = PathStyle_UnixAbsolute;
  }
  else if(string.len >= 2 &&
          char_is_alpha(string.ptr[0]) &&
          string.ptr[1] == ':')
  {
    if(string.len == 2 || char_is_slash(string.ptr[2]))
    {
      result = PathStyle_WindowsAbsolute;
    }
  }
  return result;
}

String8List
str8_split_path(Arena *arena, String8 string)
{
  String8List result = str8_split(arena, string, (U8*)"/\\", 2, 0);
  return result;
}

void
str8_path_list_resolve_dots_in_place(String8List *path, PathStyle style)
{
  Temp scratch = scratch_begin(0, 0);
  typedef struct String8MetaNode String8MetaNode;
  struct String8MetaNode
  {
    String8MetaNode *next;
    String8Node *node;
  };
  String8MetaNode *stack = 0;
  String8MetaNode *free_meta_node = 0;
  String8Node *first = path->first;
  MemoryZeroStruct(path);
  for(String8Node *node = first, *next = 0;
      node != 0;
      node = next)
  {
    // save next now
    next = node->next;
    
    // cases:
    if(node == first && style == PathStyle_WindowsAbsolute)
    {
      goto save_without_stack;
    }
    if(node->string.len == 1 && node->string.ptr[0] == '.')
    {
      goto do_nothing;
    }
    if(node->string.len == 2 && node->string.ptr[0] == '.' && node->string.ptr[1] == '.')
    {
      if(stack != 0)
      {
        goto eliminate_stack_top;
      }
      else
      {
        goto save_without_stack;
      }
    }
    goto save_with_stack;
    
    
    // handlers:
    save_with_stack:
    {
      str8_list_push_node(path, node);
      String8MetaNode *stack_node = free_meta_node;
      if(stack_node != 0)
      {
        SLLStackPop(free_meta_node);
      }
      else
      {
        stack_node = push_array_no_zero(scratch.arena, String8MetaNode, 1);
      }
      SLLStackPush(stack, stack_node);
      stack_node->node = node;
      continue;
    }
    
    save_without_stack:
    {
      str8_list_push_node(path, node);
      continue;
    }
    
    eliminate_stack_top:
    {
      path->node_count -= 1;
      path->total_size -= stack->node->string.len;
      SLLStackPop(stack);
      if(stack == 0)
      {
        path->last = path->first;
      }
      else
      {
        path->last = stack->node;
      }
      continue;
    }
    
    do_nothing: continue;
  }
  scratch_end(scratch);
}

String8
str8_path_list_join_by_style(Arena *arena, String8List *path, PathStyle style)
{
  StringJoin params = {0};
  switch(style)
  {
    case PathStyle_Null:{}break;
    case PathStyle_Relative:
    case PathStyle_WindowsAbsolute:
    {
      params.sep = str8_lit("/");
    }break;
    
    case PathStyle_UnixAbsolute:
    {
      params.pre = str8_lit("/");
      params.sep = str8_lit("/");
    }break;
  }
  String8 result = str8_list_join(arena, path, &params);
  return result;
}

String8TxtPtPair
str8_txt_pt_pair_from_string(String8 string)
{
  String8TxtPtPair pair = {0};
  {
    String8 file_part = {0};
    String8 line_part = {0};
    String8 col_part = {0};
    
    // rjf: grab file part
    for(U64 idx = 0; idx <= string.len; idx += 1)
    {
      U8 byte = (idx < string.len) ? (string.ptr[idx]) : 0;
      U8 next_byte = ((idx+1 < string.len) ? (string.ptr[idx+1]) : 0);
      if(byte == ':' && next_byte != '/' && next_byte != '\\')
      {
        file_part = str8_prefix(string, idx);
        line_part = str8_skip(string, idx+1);
        break;
      }
      else if(byte == 0)
      {
        file_part = string;
        break;
      }
    }
    
    // rjf: grab line/column
    {
      U64 colon_pos = str8_find_needle(line_part, 0, str8_lit(":"), 0);
      if(colon_pos < line_part.len)
      {
        col_part = str8_skip(line_part, colon_pos+1);
        line_part = str8_prefix(line_part, colon_pos);
      }
    }
    
    // rjf: convert line/column strings to numerics
    U64 line = 0;
    U64 column = 0;
    try_u64_from_str8_c_rules(line_part, &line);
    try_u64_from_str8_c_rules(col_part, &column);
    
    // rjf: fill
    pair.ptring = file_part;
    pair.pt = txt_pt((S64)line, (S64)column);
    if(pair.pt.line == 0) { pair.pt.line = 1; }
    if(pair.pt.column == 0) { pair.pt.column = 1; }
  }
  return pair;
}

////////////////////////////////
//~ rjf: Relative <-> Absolute Path

String8
path_relative_dst_from_absolute_dst_src(Arena *arena, String8 dst, String8 src)
{
  Temp scratch = scratch_begin(&arena, 1);
  
  // rjf: gather path parts
  String8 dst_name = str8_skip_last_slash(dst);
  String8 src_folder = src;
  String8 dst_folder = str8_chop_last_slash(dst);
  String8List src_folders = str8_split_path(scratch.arena, src_folder);
  String8List dst_folders = str8_split_path(scratch.arena, dst_folder);
  
  // rjf: count # of backtracks to get from src -> dest
  U64 num_backtracks = src_folders.node_count;
  for(String8Node *src_n = src_folders.first, *bp_n = dst_folders.first;
      src_n != 0 && bp_n != 0;
      src_n = src_n->next, bp_n = bp_n->next)
  {
    if(str8_match(src_n->string, bp_n->string, path_match_flags_from_os(OperatingSystem_CURRENT)))
    {
      num_backtracks -= 1;
    }
    else
    {
      break;
    }
  }
  
  // rjf: only build relative string if # of backtracks is not the entire `src`.
  // if getting to `dst` from `src` requires erasing the entire `src`, then the
  // only possible way to get to `dst` from `src` is via absolute path.
  String8 dst_path = {0};
  if(num_backtracks >= src_folders.node_count)
  {
    dst_path = dst;
  }
  else
  {
    // rjf: build backtrack parts
    String8List dst_path_strs = {0};
    for(U64 idx = 0; idx < num_backtracks; idx += 1)
    {
      str8_list_push(scratch.arena, &dst_path_strs, str8_lit(".."));
    }
    
    // rjf: build parts of dst which are unique from src
    {
      B32 unique_from_src = 0;
      for(String8Node *src_n = src_folders.first, *bp_n = dst_folders.first;
          bp_n != 0;
          bp_n = bp_n->next)
      {
        if(!unique_from_src && (src_n == 0 || !str8_match(src_n->string, bp_n->string, path_match_flags_from_os(OperatingSystem_CURRENT))))
        {
          unique_from_src = 1;
        }
        if(unique_from_src)
        {
          str8_list_push(scratch.arena, &dst_path_strs, bp_n->string);
        }
        if(src_n != 0)
        {
          src_n = src_n->next;
        }
      }
    }
    
    // rjf: build file name
    str8_list_push(scratch.arena, &dst_path_strs, dst_name);
    
    // rjf: join
    StringJoin join = {0};
    {
      join.sep = str8_lit("/");
    }
    dst_path = str8_list_join(arena, &dst_path_strs, &join);
  }
  scratch_end(scratch);
  return dst_path;
}

String8
path_absolute_dst_from_relative_dst_src(Arena *arena, String8 dst, String8 src)
{
  String8 result = dst;
  PathStyle dst_style = path_style_from_str8(dst);
  if(dst.len != 0 && dst_style == PathStyle_Relative)
  {
    Temp scratch = scratch_begin(&arena, 1);
    String8 dst_from_src_absolute = str8f(scratch.arena, "%S/%S", src, dst);
    String8List dst_from_src_absolute_parts = str8_split_path(scratch.arena, dst_from_src_absolute);
    PathStyle dst_from_src_absolute_style = path_style_from_str8(src);
    str8_path_list_resolve_dots_in_place(&dst_from_src_absolute_parts, dst_from_src_absolute_style);
    result = str8_path_list_join_by_style(arena, &dst_from_src_absolute_parts, dst_from_src_absolute_style);
    scratch_end(scratch);
  }
  return result;
}

////////////////////////////////
//~ rjf: Path Normalization

String8List
path_normalized_list_from_string(Arena *arena, String8 path_string, PathStyle *style_out)
{
  // rjf: analyze path
  PathStyle path_style = path_style_from_str8(path_string);
  String8List path = str8_split_path(arena, path_string);
  
  // rjf: resolve dots
  str8_path_list_resolve_dots_in_place(&path, path_style);
  
  // rjf: return
  if(style_out != 0)
  {
    *style_out = path_style;
  }
  return path;
}

String8
path_normalized_from_string(Arena *arena, String8 path_string)
{
  Temp scratch = scratch_begin(&arena, 1);
  PathStyle style = PathStyle_Relative;
  String8List path = path_normalized_list_from_string(scratch.arena, path_string, &style);
  String8 result = str8_path_list_join_by_style(arena, &path, style);
  scratch_end(scratch);
  return result;
}

B32
path_match_normalized(String8 left, String8 right)
{
  Temp scratch = scratch_begin(0, 0);
  String8 left_normalized = path_normalized_from_string(scratch.arena, left);
  String8 right_normalized = path_normalized_from_string(scratch.arena, right);
  B32 result = str8_match(left_normalized, right_normalized, StringMatchFlag_CaseInsensitive);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Misc. Path Helpers

PathStyle
path_style_from_string(String8 string)
{
  for (U64 i = 0; i < ArrayCount(g_path_style_map); ++i)
  {
    if(str8_match(g_path_style_map[i].ptring, string, StringMatchFlag_CaseInsensitive))
    {
      return g_path_style_map[i].path_style;
    }
  }
  return PathStyle_Null;
}

String8
string_from_path_style(PathStyle style)
{
  Assert(style < ArrayCount(g_path_style_map));
  return g_path_style_map[style].ptring;
}

String8
path_separator_string_from_style(PathStyle style)
{
  String8 result = str8_zero();
  switch (style)
  {
    case PathStyle_Null:     break;
    case PathStyle_Relative: break;
    case PathStyle_WindowsAbsolute: result = str8_lit("\\"); break;
    case PathStyle_UnixAbsolute:    result = str8_lit("/");  break;
  }
  return result;
}

StringMatchFlags
path_match_flags_from_os(OperatingSystem os)
{
  StringMatchFlags flags = StringMatchFlag_SlashInsensitive;
  switch(os)
  {
    default:{}break;
    case OperatingSystem_Windows:
    {
      flags |= StringMatchFlag_CaseInsensitive;
    }break;
    case OperatingSystem_Linux:
    case OperatingSystem_Mac:
    {
      // NOTE(rjf): no-op
    }break;
  }
  return flags;
}

String8
path_convert_slashes(Arena *arena, String8 path, PathStyle path_style)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = str8_split_path(scratch.arena, path);
  StringJoin join = {0};
  join.sep = path_separator_string_from_style(path_style);
  String8 result = str8_list_join(arena, &list, &join);
  scratch_end(scratch);
  return result;
}

String8
path_replace_file_extension(Arena *arena, String8 file_name, String8 ext)
{
  String8 file_name_no_ext = str8_chop_last_dot(file_name);
  String8 result           = str8f(arena, "%S.%S", file_name_no_ext, ext);
  return result;
}

////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding

read_only global U8 utf8_class[32] =
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

UnicodeDecode
utf8_decode(U8 *str, U64 max)
{
  UnicodeDecode result = {1, max_U32};
  U8 byte = str[0];
  U8 byte_class = utf8_class[byte >> 3];
  switch(byte_class)
  {
    case 1:
    {
      result.codepoint = byte;
    }break;
    case 2:
    {
      if(1 < max)
      {
        U8 cont_byte = str[1];
        if(utf8_class[cont_byte >> 3] == 0)
        {
          result.codepoint = (byte & bitmask5) << 6;
          result.codepoint |=  (cont_byte & bitmask6);
          result.inc = 2;
        }
      }
    }break;
    case 3:
    {
      if(2 < max)
      {
        U8 cont_byte[2] = {str[1], str[2]};
        if(utf8_class[cont_byte[0] >> 3] == 0 &&
           utf8_class[cont_byte[1] >> 3] == 0)
        {
          result.codepoint = (byte & bitmask4) << 12;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
          result.codepoint |=  (cont_byte[1] & bitmask6);
          result.inc = 3;
        }
      }
    }break;
    case 4:
    {
      if(3 < max)
      {
        U8 cont_byte[3] = {str[1], str[2], str[3]};
        if(utf8_class[cont_byte[0] >> 3] == 0 &&
           utf8_class[cont_byte[1] >> 3] == 0 &&
           utf8_class[cont_byte[2] >> 3] == 0)
        {
          result.codepoint = (byte & bitmask3) << 18;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
          result.codepoint |= ((cont_byte[1] & bitmask6) <<  6);
          result.codepoint |=  (cont_byte[2] & bitmask6);
          result.inc = 4;
        }
      }
    }
  }
  return result;
}

UnicodeDecode
utf16_decode(U16 *str, U64 max)
{
  UnicodeDecode result = {1, max_U32};
  result.codepoint = str[0];
  result.inc = 1;
  if(max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000)
  {
    result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
    result.inc = 2;
  }
  return result;
}

U32
utf8_encode(U8 *str, U32 codepoint)
{
  U32 inc = 0;
  if(codepoint <= 0x7F)
  {
    str[0] = (U8)codepoint;
    inc = 1;
  }
  else if(codepoint <= 0x7FF)
  {
    str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
    str[1] = bit8 | (codepoint & bitmask6);
    inc = 2;
  }
  else if(codepoint <= 0xFFFF)
  {
    str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
    str[1] = bit8 | ((codepoint >> 6) & bitmask6);
    str[2] = bit8 | ( codepoint       & bitmask6);
    inc = 3;
  }
  else if(codepoint <= 0x10FFFF)
  {
    str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
    str[1] = bit8 | ((codepoint >> 12) & bitmask6);
    str[2] = bit8 | ((codepoint >>  6) & bitmask6);
    str[3] = bit8 | ( codepoint        & bitmask6);
    inc = 4;
  }
  else
  {
    str[0] = '?';
    inc = 1;
  }
  return inc;
}

U32
utf16_encode(U16 *str, U32 codepoint)
{
  U32 inc = 1;
  if(codepoint == max_U32)
  {
    str[0] = (U16)'?';
  }
  else if(codepoint < 0x10000)
  {
    str[0] = (U16)codepoint;
  }
  else
  {
    U32 v = codepoint - 0x10000;
    str[0] = safe_cast_u16(0xD800 + (v >> 10));
    str[1] = safe_cast_u16(0xDC00 + (v & bitmask10));
    inc = 2;
  }
  return inc;
}

////////////////////////////////
//~ rjf: Unicode String Conversions

String8
str8_from_16(Arena *arena, String16 in)
{
  String8 result = str8_zero();
  if(in.len)
  {
    U64 cap = in.len*3;
    U8 *str = push_array_no_zero(arena, U8, cap + 1);
    U16 *ptr = in.ptr;
    U16 *opl = ptr + in.len;
    U64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf16_decode(ptr, opl - ptr);
      size += utf8_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    result = str8(str, size);
  }
  return result;
}

String16
str16_from_8(Arena *arena, String8 in)
{
  String16 result = str16_zero();
  if(in.len)
  {
    U64 cap = in.len*2;
    U16 *str = push_array_no_zero(arena, U16, cap + 1);
    U8 *ptr = in.ptr;
    U8 *opl = ptr + in.len;
    U64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf8_decode(ptr, opl - ptr);
      size += utf16_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size)*2);
    result = str16(str, size);
  }
  return result;
}

String8
str8_from_32(Arena *arena, String32 in)
{
  String8 result = str8_zero();
  if(in.len)
  {
    U64 cap = in.len*4;
    U8 *str = push_array_no_zero(arena, U8, cap + 1);
    U32 *ptr = in.ptr;
    U32 *opl = ptr + in.len;
    U64 size = 0;
    for(;ptr < opl; ptr += 1)
    {
      size += utf8_encode(str + size, *ptr);
    }
    str[size] = 0;
    arena_pop(arena, (cap - size));
    result = str8(str, size);
  }
  return result;
}

String32
str32_from_8(Arena *arena, String8 in)
{
  String32 result = str32_zero(); 
  if(in.len)
  {
    U64 cap = in.len;
    U32 *str = push_array_no_zero(arena, U32, cap + 1);
    U8 *ptr = in.ptr;
    U8 *opl = ptr + in.len;
    U64 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc)
    {
      consume = utf8_decode(ptr, opl - ptr);
      str[size] = consume.codepoint;
      size += 1;
    }
    str[size] = 0;
    arena_pop(arena, (cap - size)*4);
    result = str32(str, size);
  }
  return result;
}

////////////////////////////////
//~ rjf: Basic Types & Space Enum -> String Conversions

read_only global struct
{
  String8         string;
  OperatingSystem os;
} g_os_enum_map[] =
{
  { str8_lit_comp(""),        OperatingSystem_Null     },
  { str8_lit_comp("Windows"), OperatingSystem_Windows, },
  { str8_lit_comp("Linux"),   OperatingSystem_Linux,   },
  { str8_lit_comp("Mac"),     OperatingSystem_Mac,     },
};
StaticAssert(ArrayCount(g_os_enum_map) == OperatingSystem_COUNT, g_os_enum_map_count_check);

OperatingSystem
operating_system_from_string(String8 string)
{
  for EachElement(idx, g_os_enum_map)
  {
    if(str8_match(g_os_enum_map[idx].ptring, string, StringMatchFlag_CaseInsensitive))
    {
      return g_os_enum_map[idx].os;
    }
  }
  return OperatingSystem_Null;
}

String8
string_from_dimension(Dimension dimension)
{
  read_only local_persist String8 strings[] =
  {
    str8_lit_comp("X"),
    str8_lit_comp("Y"),
    str8_lit_comp("Z"),
    str8_lit_comp("W"),
  };
  String8 result = str8_lit("error");
  if((U32)dimension < 4)
  {
    result = strings[dimension];
  }
  return result;
}

String8
string_from_side(Side side)
{
  local_persist String8 strings[] =
  {
    str8_lit_comp("Min"),
    str8_lit_comp("Max"),
  };
  String8 result = str8_lit("error");
  if((U32)side < 2)
  {
    result = strings[side];
  }
  return result;
}

String8
string_from_operating_system(OperatingSystem os)
{
  String8 result = g_os_enum_map[OperatingSystem_Null].ptring;
  if(os < ArrayCount(g_os_enum_map))
  {
    result = g_os_enum_map[os].ptring;
  }
  return result;
}

String8
string_from_arch(Arch arch)
{
  String8 result = {0};
  switch(arch)
  {
    case Arch_Null:  {result = str8_lit("Null");}break;
    case Arch_x64:   {result = str8_lit("x64");}break;
    case Arch_x86:   {result = str8_lit("x86");}break;
    case Arch_arm64: {result = str8_lit("arm64");}break;
    case Arch_arm32: {result = str8_lit("arm32");}break;
    case Arch_COUNT:
    {result = str8_lit("Invalid");}break;
  }
  return result;
}

String8
string_from_week_day(WeekDay week_day)
{
  read_only local_persist String8 strings[] =
  {
    str8_lit_comp("Sun"),
    str8_lit_comp("Mon"),
    str8_lit_comp("Tue"),
    str8_lit_comp("Wed"),
    str8_lit_comp("Thu"),
    str8_lit_comp("Fri"),
    str8_lit_comp("Sat"),
  };
  String8 result = str8_lit("Err");
  if((U32)week_day < WeekDay_COUNT)
  {
    result = strings[week_day];
  }
  return result;
}

String8
string_from_month(Month month)
{
  read_only local_persist String8 strings[] =
  {
    str8_lit_comp("Jan"),
    str8_lit_comp("Feb"),
    str8_lit_comp("Mar"),
    str8_lit_comp("Apr"),
    str8_lit_comp("May"),
    str8_lit_comp("Jun"),
    str8_lit_comp("Jul"),
    str8_lit_comp("Aug"),
    str8_lit_comp("Sep"),
    str8_lit_comp("Oct"),
    str8_lit_comp("Nov"),
    str8_lit_comp("Dec"),
  };
  String8 result = str8_lit("Err");
  if((U32)month < Month_COUNT)
  {
    result = strings[month];
  }
  return result;
}

String8
string_from_date_time(Arena *arena, DateTime *date_time)
{
  char *mon_str = (char*)string_from_month(date_time->month).ptr;
  U32 adjusted_hour = date_time->hour%12;
  if(adjusted_hour == 0)
  {
    adjusted_hour = 12;
  }
  char *ampm = "am";
  if(date_time->hour >= 12)
  {
    ampm = "pm";
  }
  String8 result = push_str8f(arena, "%d %s %d, %02d:%02d:%02d %s",
                              date_time->day, mon_str, date_time->year,
                              adjusted_hour, date_time->min, date_time->sec, ampm);
  return result;
}

String8
string_from_date_time__file_name(Arena *arena, DateTime *date_time)
{
  char *mon_str = (char*)string_from_month(date_time->month).ptr;
  String8 result = str8f(arena, "%d-%s-%0d--%02d-%02d-%02d",
                         date_time->year, mon_str, date_time->day,
                         date_time->hour, date_time->min, date_time->sec);
  return result;
}

String8
string_from_elapsed_time(Arena *arena, DateTime dt)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List list = {0};
  if (dt.year) {
    str8_list_pushf(scratch.arena, &list, "%dy", dt.year);
    str8_list_pushf(scratch.arena, &list, "%um", dt.mon);
    str8_list_pushf(scratch.arena, &list, "%ud", dt.day);
  } else if (dt.mon) {
    str8_list_pushf(scratch.arena, &list, "%um", dt.mon);
    str8_list_pushf(scratch.arena, &list, "%ud", dt.day);
  } else if (dt.day) {
    str8_list_pushf(scratch.arena, &list, "%ud", dt.day);
  }
  
  if (dt.hour) {
    str8_list_pushf(scratch.arena, &list, "%uh %um %u.%us", dt.hour, dt.min, dt.sec, dt.msec);
  } else if (dt.min) {
    str8_list_pushf(scratch.arena, &list, "%um %u.%us", dt.min, dt.sec, dt.msec);
  } else if (dt.sec) {
    str8_list_pushf(scratch.arena, &list, "%u.%us", dt.sec, dt.msec);
  } else if (dt.msec) {
    str8_list_pushf(scratch.arena, &list, "%ums", dt.msec);
  } else if (dt.micro_sec) {
    str8_list_pushf(scratch.arena, &list, "%uus", dt.micro_sec);
  }
  
  if (list.node_count == 0) {
    str8_list_pushf(scratch.arena, &list, "0");
  }
  
  String8 result = str8_list_join(arena, &list, &(StringJoin){ str8_lit_comp(""), str8_lit_comp(" "), str8_lit_comp("") });
  
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: String <-> Globally Unique IDs

String8
string_from_guid(Arena *arena, Guid guid)
{
  String8 result = str8f(arena, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                         guid.data1,
                         guid.data2,
                         guid.data3,
                         guid.data4[0],
                         guid.data4[1],
                         guid.data4[2],
                         guid.data4[3],
                         guid.data4[4],
                         guid.data4[5],
                         guid.data4[6],
                         guid.data4[7]);
  return result;
}

B32
try_guid_from_string(String8 string, Guid *guid_out)
{
  Temp scratch = scratch_begin(0,0);
  B32 is_parsed = 0;
  String8List list = str8_split_by_string_chars(scratch.arena, string, str8_lit("-"), StringSplitFlag_KeepEmpties);
  if(list.node_count == 5)
  {
    String8 data1_str    = list.first->string;
    String8 data2_str    = list.first->next->string;
    String8 data3_str    = list.first->next->next->string;
    String8 data4_hi_str = list.first->next->next->next->string;
    String8 data4_lo_str = list.first->next->next->next->next->string;
    if(str8_is_integer(data1_str, 16) && 
       str8_is_integer(data2_str, 16) &&
       str8_is_integer(data3_str, 16) &&
       str8_is_integer(data4_hi_str, 16) &&
       str8_is_integer(data4_lo_str, 16))
    {
      U64 data1    = u64_from_str8(data1_str, 16);
      U64 data2    = u64_from_str8(data2_str, 16);
      U64 data3    = u64_from_str8(data3_str, 16);
      U64 data4_hi = u64_from_str8(data4_hi_str, 16);
      U64 data4_lo = u64_from_str8(data4_lo_str, 16);
      if(data1 <= max_U32 &&
         data2 <= max_U16 &&
         data3 <= max_U16 &&
         data4_hi <= max_U16 &&
         data4_lo <= 0xffffffffffff)
      {
        guid_out->data1 = (U32)data1;
        guid_out->data2 = (U16)data2;
        guid_out->data3 = (U16)data3;
        U64 data4 = (data4_hi << 48) | data4_lo;
        MemoryCopy(&guid_out->data4[0], &data4, sizeof(data4));
        is_parsed = 1;
      }
    }
  }
  scratch_end(scratch);
  return is_parsed;
}

Guid
guid_from_string(String8 string)
{
  Guid guid = {0};
  try_guid_from_string(string, &guid);
  return guid;
}

////////////////////////////////
//~ rjf: Basic Text Indentation

String8
indented_from_string(Arena *arena, String8 string)
{
  Temp scratch = scratch_begin(&arena, 1);
  read_only local_persist U8 indentation_bytes[] = "                                                                                                                                ";
  String8List indented_strings = {0};
  S64 depth = 0;
  S64 next_depth = 0;
  U64 line_begin_off = 0;
  for(U64 off = 0; off <= string.len; off += 1)
  {
    U8 byte = off<string.len ? string.ptr[off] : 0;
    switch(byte)
    {
      default:{}break;
      case '{':case '[':case '(':{next_depth += 1; next_depth = Max(0, next_depth);}break;
      case '}':case ']':case ')':{next_depth -= 1; next_depth = Max(0, next_depth); depth = next_depth;}break;
      case '\n':
      case 0:
      {
        String8 line = str8_skip_chop_whitespace(str8_substr(string, r1u64(line_begin_off, off)));
        if(line.len != 0)
        {
          str8_list_pushf(scratch.arena, &indented_strings, "%.*s%S\n", (int)depth*2, indentation_bytes, line);
        }
        if(line.len == 0 && indented_strings.node_count != 0 && off < string.len)
        {
          str8_list_pushf(scratch.arena, &indented_strings, "\n");
        }
        line_begin_off = off+1;
        depth = next_depth;
      }break;
    }
  }
  String8 result = str8_list_join(arena, &indented_strings, 0);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Text Escaping

String8
escaped_from_raw_str8(Arena *arena, String8 string)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List parts = {0};
  U64 start_split_idx = 0;
  for(U64 idx = 0; idx <= string.len; idx += 1)
  {
    U8 byte = (idx < string.len) ? string.ptr[idx] : 0;
    B32 split = 1;
    String8 separator_replace = {0};
    switch(byte)
    {
      default:{split = 0;}break;
      case 0:    {}break;
      case '\a': {separator_replace = str8_lit("\\a");}break;
      case '\b': {separator_replace = str8_lit("\\b");}break;
      case '\f': {separator_replace = str8_lit("\\f");}break;
      case '\n': {separator_replace = str8_lit("\\n");}break;
      case '\r': {separator_replace = str8_lit("\\r");}break;
      case '\t': {separator_replace = str8_lit("\\t");}break;
      case '\v': {separator_replace = str8_lit("\\v");}break;
      case '\\': {separator_replace = str8_lit("\\\\");}break;
      case '"':  {separator_replace = str8_lit("\\\"");}break;
    }
    if(split)
    {
      String8 substr = str8_substr(string, r1u64(start_split_idx, idx));
      start_split_idx = idx+1;
      str8_list_push(scratch.arena, &parts, substr);
      if(separator_replace.len != 0)
      {
        str8_list_push(scratch.arena, &parts, separator_replace);
      }
    }
  }
  StringJoin join = {0};
  String8 result = str8_list_join(arena, &parts, &join);
  scratch_end(scratch);
  return result;
}

String8
raw_from_escaped_str8(Arena *arena, String8 string)
{
  Temp scratch = scratch_begin(&arena, 1);
  String8List strs = {0};
  U64 start = 0;
  for(U64 idx = 0; idx <= string.len; idx += 1)
  {
    if(idx == string.len || string.ptr[idx] == '\\' || string.ptr[idx] == '\r')
    {
      String8 str = str8_substr(string, r1u64(start, idx));
      if(str.len != 0)
      {
        str8_list_push(scratch.arena, &strs, str);
      }
      start = idx+1;
    }
    if(idx < string.len && string.ptr[idx] == '\\')
    {
      U8 next_char = string.ptr[idx+1];
      U8 replace_byte = 0;
      switch(next_char)
      {
        default:{}break;
        case 'a': replace_byte = 0x07; break;
        case 'b': replace_byte = 0x08; break;
        case 'e': replace_byte = 0x1b; break;
        case 'f': replace_byte = 0x0c; break;
        case 'n': replace_byte = 0x0a; break;
        case 'r': replace_byte = 0x0d; break;
        case 't': replace_byte = 0x09; break;
        case 'v': replace_byte = 0x0b; break;
        case '\\':replace_byte = '\\'; break;
        case '\'':replace_byte = '\''; break;
        case '"': replace_byte = '"';  break;
        case '?': replace_byte = '?';  break;
      }
      String8 replace_string = push_str8_copy(scratch.arena, str8(&replace_byte, 1));
      str8_list_push(scratch.arena, &strs, replace_string);
      idx += 1;
      start += 1;
    }
  }
  String8 result = str8_list_join(arena, &strs, 0);
  scratch_end(scratch);
  return result;
}

////////////////////////////////
//~ rjf: Text Wrapping

String8List
wrapped_lines_from_string(Arena *arena, String8 string, U64 first_line_max_width, U64 max_width, U64 wrap_indent)
{
  String8List list = {0};
  Rng1U64 line_range = r1u64(0, 0);
  U64 wrapped_indent_level = 0;
  static char *spaces = "                                                                ";
  for (U64 idx = 0; idx <= string.len; idx += 1){
    U8 chr = idx < string.len ? string.ptr[idx] : 0;
    if (chr == '\n'){
      Rng1U64 candidate_line_range = line_range;
      candidate_line_range.max = idx;
      // NOTE(nick): when wrapping is interrupted with \n we emit a string without including \n
      // because later tool_fprint_list inserts separator after each node
      // except for last node, so don't strip last \n.
      if (idx + 1 == string.len){
        candidate_line_range.max += 1;
      }
      String8 substr = str8_substr(string, candidate_line_range);
      str8_list_push(arena, &list, substr);
      line_range = r1u64(idx+1,idx+1);
    }
    else
      if (char_is_space(chr) || chr == 0){
      Rng1U64 candidate_line_range = line_range;
      candidate_line_range.max = idx;
      String8 substr = str8_substr(string, candidate_line_range);
      U64 width_this_line = max_width-wrapped_indent_level;
      if (list.node_count == 0){
        width_this_line = first_line_max_width;
      }
      if (substr.len > width_this_line){
        String8 line = str8_substr(string, line_range);
        if (wrapped_indent_level > 0){
          line = push_str8f(arena, "%.*s%S", wrapped_indent_level, spaces, line);
        }
        str8_list_push(arena, &list, line);
        line_range = r1u64(line_range.max+1, candidate_line_range.max);
        wrapped_indent_level = ClampTop(64, wrap_indent);
      }
      else{
        line_range = candidate_line_range;
      }
    }
  }
  if (line_range.min < string.len && line_range.max > line_range.min){
    String8 line = str8_substr(string, line_range);
    if (wrapped_indent_level > 0){
      line = push_str8f(arena, "%.*s%S", wrapped_indent_level, spaces, line);
    }
    str8_list_push(arena, &list, line);
  }
  return list;
}

////////////////////////////////
//~ rjf: String <-> Color

String8
hex_string_from_rgba_4f32(Arena *arena, Vec4F32 rgba)
{
  String8 hex_string = str8f(arena, "%02x%02x%02x%02x", (U8)(rgba.x*255.f), (U8)(rgba.y*255.f), (U8)(rgba.z*255.f), (U8)(rgba.w*255.f));
  return hex_string;
}

Vec4F32
rgba_from_hex_string_4f32(String8 hex_string)
{
  U8 byte_text[8] = {0};
  U64 byte_text_idx = 0;
  for(U64 idx = 0; idx < hex_string.len && byte_text_idx < ArrayCount(byte_text); idx += 1)
  {
    if(char_is_digit(hex_string.ptr[idx], 16))
    {
      byte_text[byte_text_idx] = lower_from_char(hex_string.ptr[idx]);
      byte_text_idx += 1;
    }
  }
  U8 byte_vals[4] = {0};
  for(U64 idx = 0; idx < 4; idx += 1)
  {
    byte_vals[idx] = (U8)u64_from_str8(str8(&byte_text[idx*2], 2), 16);
  }
  Vec4F32 rgba = v4f32(byte_vals[0]/255.f, byte_vals[1]/255.f, byte_vals[2]/255.f, byte_vals[3]/255.f);
  return rgba;
}

////////////////////////////////
//~ rjf: String Fuzzy Matching

FuzzyMatchRangeList
fuzzy_match_find(Arena *arena, String8 needle, String8 haystack)
{
  FuzzyMatchRangeList result = {0};
  Temp scratch = scratch_begin(&arena, 1);
  String8List needles = str8_split(scratch.arena, needle, (U8*)" ", 1, 0);
  result.needle_part_count = needles.node_count;
  for(String8Node *needle_n = needles.first; needle_n != 0; needle_n = needle_n->next)
  {
    U64 find_pos = 0;
    for(;find_pos < haystack.len;)
    {
      find_pos = str8_find_needle(haystack, find_pos, needle_n->string, StringMatchFlag_CaseInsensitive|StringMatchFlag_SlashInsensitive);
      B32 is_in_gathered_ranges = 0;
      for(FuzzyMatchRangeNode *n = result.first; n != 0; n = n->next)
      {
        if(n->range.min <= find_pos && find_pos < n->range.max)
        {
          is_in_gathered_ranges = 1;
          find_pos = n->range.max;
          break;
        }
      }
      if(!is_in_gathered_ranges)
      {
        break;
      }
    }
    if(find_pos < haystack.len)
    {
      Rng1U64 range = r1u64(find_pos, find_pos+needle_n->string.len);
      FuzzyMatchRangeNode *n = push_array(arena, FuzzyMatchRangeNode, 1);
      n->range = range;
      SLLQueuePush(result.first, result.last, n);
      result.count += 1;
      result.total_dim += dim_1u64(range);
    }
  }
  scratch_end(scratch);
  return result;
}

FuzzyMatchRangeList
fuzzy_match_range_list_copy(Arena *arena, FuzzyMatchRangeList *src)
{
  FuzzyMatchRangeList dst = {0};
  for(FuzzyMatchRangeNode *src_n = src->first; src_n != 0; src_n = src_n->next)
  {
    FuzzyMatchRangeNode *dst_n = push_array(arena, FuzzyMatchRangeNode, 1);
    SLLQueuePush(dst.first, dst.last, dst_n);
    dst_n->range = src_n->range;
  }
  dst.count = src->count;
  dst.needle_part_count = src->needle_part_count;
  dst.total_dim = src->total_dim;
  return dst;
}

////////////////////////////////
//~ NOTE(allen): Serialization Helpers

void
str8_serial_begin(Arena *arena, String8List *srl)
{
  String8Node *node = push_array(arena, String8Node, 1);
  node->string.ptr = push_array_no_zero(arena, U8, 0);
  srl->first      = srl->last = node;
  srl->node_count = 1;
  srl->total_size = 0;
}

String8
str8_serial_end(Arena *arena, String8List *srl)
{
  U64 size = srl->total_size;
  U8 *out = push_array_no_zero(arena, U8, size);
  str8_serial_write_to_dst(srl, out);
  String8 result = str8(out, size);
  return result;
}

void
str8_serial_write_to_dst(String8List *srl, void *out)
{
  U8 *ptr = (U8*)out;
  for EachNode(n, String8Node, srl->first)
  {
    U64 size = n->string.len;
    MemoryCopy(ptr, n->string.ptr, size);
    ptr += size;
  }
}

U64
str8_serial_push_align(Arena *arena, String8List *srl, U64 align)
{
  Assert(IsPow2(align));
  
  U64 pos = srl->total_size;
  U64 new_pos = AlignPow2(pos, align);
  U64 size = (new_pos - pos);
  
  if(size != 0)
  {
    U8 *buf = push_array(arena, U8, size);
    
    String8 *str = &srl->last->string;
    if(str->str + str->size == buf)
    {
      srl->last->string.len += size;
      srl->total_size += size;
    }
    else
    {
      str8_list_push(arena, srl, str8(buf, size));
    }
  }
  return size;
}

void *
str8_serial_push_size(Arena *arena, String8List *srl, U64 size)
{
  void *result = 0;
  if(size != 0)
  {
    U8 *buf = push_array(arena, U8, size);
    String8 *str = &srl->last->string;
    if(str->str + str->size == buf)
    {
      srl->last->string.len += size;
      srl->total_size += size;
    }
    else
    {
      str8_list_push(arena, srl, str8(buf, size));
    }
    result = buf;
  }
  return result;
}

void *
str8_serial_push_data(Arena *arena, String8List *srl, void *data, U64 size)
{
  void *result = str8_serial_push_size(arena, srl, size);
  if(result != 0)
  {
    MemoryCopy(result, data, size);
  }
  return result;
}

void
str8_serial_push_data_list(Arena *arena, String8List *srl, String8Node *first)
{
  for EachNode(n, String8Node, first)
  {
    str8_serial_push_data(arena, srl, n->string.ptr, n->string.len);
  }
}

void *
str8_serial_push_u64(Arena *arena, String8List *srl, U64 x)
{
  return str8_serial_push_data(arena, srl, &x, sizeof(x));
}

void *
str8_serial_push_u32(Arena *arena, String8List *srl, U32 x)
{
  return str8_serial_push_data(arena, srl, &x, sizeof(x));
}

void *
str8_serial_push_u16(Arena *arena, String8List *srl, U16 x)
{
  return str8_serial_push_data(arena, srl, &x, sizeof(x));
}

void *
str8_serial_push_u8(Arena *arena, String8List *srl, U8 x)
{
  return str8_serial_push_data(arena, srl, &x, sizeof(x));
}

void *
str8_serial_push_cstr(Arena *arena, String8List *srl, String8 str)
{
  void *ptr = str8_serial_push_data(arena, srl, str.ptr, str.len);
  str8_serial_push_u8(arena, srl, 0);
  return ptr;
}

void *
str8_serial_push_string(Arena *arena, String8List *srl, String8 str)
{
  return str8_serial_push_data(arena, srl, str.ptr, str.len);
}

////////////////////////////////
//~ rjf: Deserialization Helpers

U64
str8_deserial_read(String8 string, U64 off, void *read_dst, U64 read_size, U64 granularity)
{
  U64 bytes_left = string.len-Min(off, string.len);
  U64 actually_readable_size = Min(bytes_left, read_size);
  U64 legally_readable_size = actually_readable_size - actually_readable_size%granularity;
  if(legally_readable_size > 0)
  {
    MemoryCopy(read_dst, string.ptr+off, legally_readable_size);
  }
  return legally_readable_size;
}

U64
str8_deserial_find_first_match(String8 string, U64 off, U16 scan_val)
{
  U64 cursor = off;
  for (;;) {
    U16 val = 0;
    str8_deserial_read_struct(string, cursor, &val);
    if (val == scan_val) {
      break;
    }
    cursor += sizeof(val);
  }
  return cursor;
}

void *
str8_deserial_get_raw_ptr(String8 string, U64 off, U64 size)
{
  void *raw_ptr = 0;
  if (off + size <= string.len) {
    raw_ptr = string.ptr + off;
  }
  return raw_ptr;
}

U64
str8_deserial_read_cstr(String8 string, U64 off, String8 *cstr_out)
{
  U64 cstr_size = 0;
  if (off < string.len) {
    U8 *ptr = string.ptr + off;
    U8 *cap = string.ptr + string.len;
    *cstr_out = str8_cstring_capped(ptr, cap);
    cstr_size = (cstr_out->size + 1);
  }
  return cstr_size;
}

U64
str8_deserial_read_windows_utf16_string16(String8 string, U64 off, String16 *str_out)
{
  U64 null_off = str8_deserial_find_first_match(string, off, 0);
  U64 size = null_off - off;
  U16 *str = (U16 *)str8_deserial_get_raw_ptr(string, off, size);
  U64 count = size / sizeof(*str);
  *str_out = str16(str, count);
  
  U64 read_size_with_null = size + sizeof(*str);
  return read_size_with_null;
}

U64
str8_deserial_read_block(String8 string, U64 off, U64 size, String8 *block_out)
{
  Rng1U64 range = rng_1u64(off, off + size);
  *block_out = str8_substr(string, range);
  return block_out->size;
}

U64
str8_deserial_read_uleb128(String8 string, U64 off, U64 *value_out)
{
  U64 value  = 0;
  U64 shift  = 0;
  U64 cursor = off;
  for(;;)
  {
    U8  byte       = 0;
    U64 bytes_read = str8_deserial_read_struct(string, cursor, &byte);
    if(bytes_read != sizeof(byte))
    {
      break;
    }
    U8 val = byte & 0x7fu;
    value |= ((U64)val) << shift;
    cursor += bytes_read;
    shift += 7u;
    if((byte & 0x80u) == 0)
    {
      break;
    }
  }
  if(value_out != 0)
  {
    *value_out = value;
  }
  U64 bytes_read = cursor - off;
  return bytes_read;
}

U64
str8_deserial_read_sleb128(String8 string, U64 off, S64 *value_out)
{
  U64 value  = 0;
  U64 shift  = 0;
  U64 cursor = off;
  for(;;)
  {
    U8 byte;
    U64 bytes_read = str8_deserial_read_struct(string, cursor, &byte);
    if(bytes_read != sizeof(byte))
    {
      break;
    }
    U8 val = byte & 0x7fu;
    value |= ((U64)val) << shift;
    cursor += bytes_read;
    shift += 7u;
    if((byte & 0x80u) == 0)
    {
      if(shift < sizeof(value) * 8 && (byte & 0x40u) != 0)
      {
        value |= -(S64)(1ull << shift);
      }
      break;
    }
  }
  if(value_out != 0)
  {
    *value_out = value;
  }
  U64 bytes_read = cursor - off;
  return bytes_read;
}

////////////////////////////////

force_inline int
str8_compar(String8 a, String8 b, B32 ignore_case)
{
  U64 size = Min(a.len, b.len);
  int cmp = ignore_case ? MemCompareI(a.ptr, b.ptr, size) : MemCompare(a.ptr, b.ptr, size);
  
  // normalize compar result
  cmp = cmp > 0 ? 1 : cmp < 0 ? -1 : 0;
  
  // shorter prefix must precede longer prefixes
  if (cmp == 0)
  {
    cmp = a.len < b.len ? -1 :
    a.len > b.len ? +1 : 0;
  }
  
  return cmp;
}

force_inline int
str8_compar_ignore_case(const void *a, const void *b)
{
  return str8_compar(*(String8*)a, *(String8*)b, 1);
}

force_inline int
str8_compar_case_sensitive(const void *a, const void *b)
{
  return str8_compar(*(String8*)a, *(String8*)b, 0);
}

force_inline int
str8_is_before_case_sensitive(const void *a, const void *b)
{
  int cmp = str8_compar_case_sensitive(a, b);
  return cmp < 0;
}

////////////////////////////////
// string buffer

B32
str8_buffer_skip(String8Node *buf, U64 *pos, U64 skip)
{
  S64 to_skip;
  for (to_skip = skip; to_skip > 0;) {
    if (buf == 0) { break; }
    
    U64 left = Min(to_skip, buf->string.len -  *pos);
    *pos += left;
    
    if (*pos == buf->string.len) {
      if (buf->next) { *buf = *buf->next;                }
      else           { *buf = (String8Node){0}; buf = 0; }
      *pos = 0;
    }
    
    to_skip -= (S64)left;
  }
  Assert(to_skip == 0);
  return (to_skip == 0);
}

U64
str8_buffer_read(String8Node *buf, U64 *pos, U64 read_size, void *out)
{
  U64 cursor = 0;
  for (; cursor < read_size ;) {
    if (buf == 0) { break; }
    
    U64   copy_size = Min(read_size - cursor, (buf->string.len - *pos));
    void *dst       = (U8 *)out + cursor;
    void *src       = buf->string.ptr + *pos;
    MemoryCopy(dst, src, copy_size);
    
    *pos   += copy_size;
    cursor += copy_size;
    
    if (*pos >= buf->string.len) {
      if (buf->next) { *buf = *buf->next;                }
      else           { *buf = (String8Node){0}; buf = 0; }
      *pos = 0;
    }
  }
  return cursor;
}

U64
str8_buffer_peek(String8Node *buf, U64 *pos, U64 read_size, void *out)
{
  String8Node buf_copy = *buf;
  U64         pos_copy = *pos;
  return str8_buffer_read(&buf_copy, &pos_copy, read_size, out);
}

U64
str8_buffer_write(String8Node *buf, U64 *pos, String8 data)
{
  U64 copy_size = 0;
  if (buf) {
    for (copy_size = 0; copy_size < data.len; ) {
      U64 data_size = data.len - copy_size;
      
      U64 available_size = buf->string.len - *pos;
      U64 to_copy        = Min(available_size, data_size);
      if (data.ptr == 0) {
        MemorySet(buf->string.ptr + *pos, 0, to_copy);
      } else {
        U8 *data_ptr = data.ptr + copy_size;
        MemoryCopy(buf->string.ptr + *pos, data_ptr, to_copy);
      }
      *pos      += to_copy;
      copy_size += to_copy;
      
      if (*pos >= buf->string.len) {
        if (buf->next) {
          *buf = *buf->next;
          *pos = 0;
        } else {
          break;
        }
      }
    }
    Assert(copy_size == data.len);
  } else {
    copy_size = data.len;
  }
  return copy_size;
}

U64
str8_buffer_write_u16(String8Node *buf, U64 *pos, U16 v)
{
  return str8_buffer_write(buf, pos, str8_struct(&v));
}

U64
str8_buffer_write_u32(String8Node *buf, U64 *pos, U32 v)
{
  return str8_buffer_write(buf, pos, str8_struct(&v));
}

U64
str8_buffer_write_zeroes(String8Node *buf, U64 *pos, U64 size)
{
  return str8_buffer_write(buf, pos, str8(0, size));
}

U64
str8_buffer_write_string_list(String8Node *buf, U64 *pos, String8List list)
{
  U64 copy_size = 0;
  for EachNode(n, String8Node, list.first) { copy_size += str8_buffer_write(buf, pos, n->string); }
  return copy_size;
}

