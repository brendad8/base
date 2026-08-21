
/* print.h - print functions

   To use this library, do this in *one* C file:
      #define PRINT_IMPLEMENTATION
      #include "base/rand.h"

   Dependencies:
       stb_sprintf - v1.10 - public domain snprintf() implementation
*/

#ifndef PRINT_H
#define PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stdarg.h>

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

int   print      (const char *fmt, ...);
int   println    (const char *fmt, ...);

int   eprint     (const char *fmt, ...);
int   eprintln   (const char *fmt, ...);

int   vprint     (const char* fmt, va_list);
int   vprintln   (const char* fmt, va_list);

int   veprint    (const char* fmt, va_list);
int   veprintln  (const char* fmt, va_list);

#ifdef __cplusplus
}
#endif

#endif // PRINT_H

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef PRINT_IMPLEMENTATION

// NOTE(bcall): without NOUNALIGNED define, zig cc debug programs will panic for known reason. 

#ifndef STB_SPRINTF_H_INCLUDE
    #ifndef STB_SPRINTF_IMPLEMENTATION
        #define STB_SPRINTF_NOUNALIGNED
        #define STB_SPRINTF_IMPLEMENTATION
        #include "third_party/stb_sprintf.h"
    #endif
#endif

#ifdef _WIN32
    #include <windows.h>
    #define NEWLINE "\r\n"
    #define NEWLINE_LEN 2
#else
    #include <unistd.h>
    #define NEWLINE "\n"
    #define NEWLINE_LEN 1
#endif


static int print_write_stdout(const char* buf, int len)
{
    int result;
#ifdef _WIN32
    HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(std_out, (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)write(STDOUT_FILENO, buf, (size_t)len);
#endif
    return result;
}

static int print_write_stderr(const char* buf, int len)
{
    int result;
#ifdef _WIN32
    HANDLE std_out = GetStdHandle(STD_ERROR_HANDLE);
    WriteFile(std_out, (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)write(STDERR_FILENO, buf, (size_t)len);
#endif
    return result;
}

static char* print_stdout_cb(const char *buf, void* file, int len)
{
    print_write_stdout(buf, len);
    return (char *)buf;
}

static char* print_stderr_cb(const char *buf, void* file, int len)
{
    print_write_stderr(buf, len);
    return (char *)buf;
}

int vprint(const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_stdout_cb, NULL, buffer, fmt, ap);
}

int vprintln(const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    int result = stbsp_vsprintfcb(print_stdout_cb, NULL, buffer, fmt, ap);

    char* newline = NEWLINE;
    int newline_len = NEWLINE_LEN;
    result += print_write_stdout(newline, newline_len);

    return result;
}

int veprint(const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_stderr_cb, NULL, buffer, fmt, ap);
}

int veprintln(const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    int result = stbsp_vsprintfcb(print_stderr_cb, NULL, buffer, fmt, ap);

    char* newline = NEWLINE;
    int newline_len = NEWLINE_LEN;
    result += print_write_stderr(newline, newline_len);

    return result;
}

int print(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vprint(fmt, ap);
    va_end(ap);
    return ret;
}

int println(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vprintln(fmt, ap);
    va_end(ap);
    return ret;
}

int eprint(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = veprint(fmt, ap);
    va_end(ap);
    return ret;
}

int eprintln(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = veprintln(fmt, ap);
    va_end(ap);
    return ret;
}

#endif // PRINT_IMPLEMENTATION
      
