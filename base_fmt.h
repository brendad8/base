
#pragma once

#include <stdarg.h>

#include "base_core.h"
#include "base_file.h"

#if COMPILER_CLANG || COMPILER_GCC
#define printf_format(fmt_index, first_arg) __attribute__((format(printf, fmt_index, first_arg)))
#elif defined(COMPILER_MSVC)
#define printf_format(fmt_index, first_arg)
#else
#define printf_format(fmt_index, first_arg)
#endif

int   fmt_count          (const char* fmt, ...) printf_format(1, 2);

int   fmt_print          (const char* fmt, ...) printf_format(1, 2);
int   fmt_print_err      (const char* fmt, ...) printf_format(1, 2);
int   fmt_print_file     (File file, const char* fmt, ...) printf_format(2, 3);

int   fmt_print_buf      (char* buf, const char* fmt, ...) printf_format(2, 3);
int   fmt_print_buf_max  (char* buf, int max, const char* fmt, ...) printf_format(3, 4);

int   fmt_vprint_file    (File file, const char* fmt, va_list ap);
int   fmt_vprint_buf     (char* buf, const char* fmt, va_list ap);
int   fmt_vprint_buf_max (char* buf, int max, const char* fmt, va_list ap);

// TODO(bcall):
// FmtErr fmt_parse_i32(str8 str, int base, i32* out);
// FmtErr fmt_parse_u32(str8 str, int base, u32* out);
// FmtErr fmt_parse_f32(str8 str, int base, f32* out);
