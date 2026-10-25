
#include <stdarg.h>

#if OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "base_format.h"

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

int fmt_vprint_buf_max(char* buf, int max, const char *fmt, va_list ap)
{
    return stbsp_vsnprintf(buf, max, fmt, ap);
}

int fmt_print_buf_max(char* buf, int max, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = fmt_vprint_buf_max(buf, max, fmt, ap);
    va_end(ap);
    return ret;
}

int fmt_vcount(const char *fmt, va_list ap)
{
    return stbsp_vsnprintf(NULL, 0, fmt, ap);
}

int fmt_count(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int result = fmt_vcount(fmt, ap);
    va_end(ap);
    return result;
}

String8 fmt_print_alloc(Allocator allocator, const char* fmt, ...)
{
    String8 result = {0};
    va_list ap;
    va_start(ap, fmt);

    // NOTE(bcall): plus one for null termination
    u64 size_needed = fmt_vcount(fmt, ap) + 1;
    u8* buf = mem_alloc_array(allocator, u8, size_needed);

    if (buf)
    {
        fmt_vprint_buf_max((char*)buf, size_needed, fmt, ap);
        buf[size_needed] = '\0';

        result.ptr = buf;
        result.len = size_needed - 1;
    }

    va_end(ap);
    return result;
}
