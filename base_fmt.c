
#include <stdarg.h>

#if OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "base_fmt.h"

// NOTE(bcall): without NOUNALIGNED define, zig cc debug programs will panic for known reason. 
#define STB_SPRINTF_NOUNALIGNED
#define STB_SPRINTF_IMPLEMENTATION
#include "third_party/stb_sprintf.h"

internal char* 
print_cb(const char *buf, void* file, int len)
{
    File out = *(File*)file;
    file_write(out, (u8*)buf, len);
    return (char *)buf;
}

int fmt_vprint_file(File file, const char* fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_cb, &file, buffer, fmt, ap);
}

int fmt_print_file(File file, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = fmt_vprint_file(file, fmt, ap);
    va_end(ap);
    return ret;
}

int fmt_print(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    File out = file_stdout();
    int ret = fmt_vprint_file(out, fmt, ap);
    va_end(ap);
    return ret;
}

int fmt_print_err(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    File err = file_stderr();
    int ret = fmt_vprint_file(err, fmt, ap);
    va_end(ap);
    return ret;
}

int fmt_vprint_buf(char* buf, const char *fmt, va_list ap)
{
    return stbsp_vsprintf(buf, fmt, ap);
}

int fmt_print_buf(char* buf, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = fmt_vprint_buf(buf, fmt, ap);
    va_end(ap);
    return ret;
}

int fmt_vprint_buf_n(char* buf, int n, const char *fmt, va_list ap)
{
    return stbsp_vsnprintf(buf, n, fmt, ap);
}

int fmt_print_buf_n(char* buf, int n, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = fmt_vprint_buf_n(buf, n, fmt, ap);
    va_end(ap);
    return ret;
}

int fmt_count(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int result = stbsp_vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);
    return result;
}
