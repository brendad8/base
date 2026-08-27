
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
#include "file.h" // for File struct

/***************************************************************************
 *          MACROS
 ***************************************************************************/

#define prints(str)              print("%s\n", str)
#define println(fmt, ...)        print(fmt "\n", __VA_ARGS__)
#define eprintln(fmt, ...)       eprint(fmt "\n", __VA_ARGS__)
#define fprintln(file, fmt, ...) fprint(file, fmt "\n", __VA_ARGS__)

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

int   print      (const char* fmt, ...);
int   eprint     (const char* fmt, ...);
int   fprint     (File file, const char* fmt, ...);

int   bprint     (char* buf, const char* fmt, ...);
int   bnprint    (char* buf, int n, const char* fmt, ...);

int   vfprint    (File file, const char* fmt, va_list);
int   vbprint    (char* buf, const char* fmt, va_list);
int   vbnprint   (char* buf, int n, const char* fmt, va_list);

#ifdef __cplusplus
}
#endif

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef PRINT_IMPLEMENTATION

// NOTE(bcall): without NOUNALIGNED define, zig cc debug programs will panic for known reason. 
#define STB_SPRINTF_NOUNALIGNED
#define STB_SPRINTF_IMPLEMENTATION
#include "third_party/stb_sprintf.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

File print_get_stdout(void)
{
    File result = {0};
#ifdef _WIN32
    result.fd = (uint64_t)GetStdHandle(STD_OUTPUT_HANDLE);
#else
    result.fd = (uint64_t)STDOUT_FILENO;
#endif
    return result;
}

File print_get_stderr(void)
{
    File result = {0};
#ifdef _WIN32
    result.fd = (uint64_t)GetStdHandle(STD_ERROR_HANDLE);
#else
    result.fd = (uint64_t)STDERR_FILENO;
#endif
    return result;
}

static int print_write_file(File file, const char* buf, int len)
{
    int result = 0;
#ifdef _WIN32
    result = WriteFile((HANDLE)file.fd, (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)write((int)file.fd, buf, (size_t)len);
#endif
    return result;
}

static char* print_cb(const char *buf, void* file, int len)
{
    File out = *(File*)file;
    print_write_file(out, buf, len);
    return (char *)buf;
}

int vfprint(File file, const char* fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_cb, &file, buffer, fmt, ap);
}

int fprint(File file, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprint(file, fmt, ap);
    va_end(ap);
    return ret;
}

int print(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    File out = print_get_stdout();
    int ret = vfprint(out, fmt, ap);
    va_end(ap);
    return ret;
}

int eprint(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    File err = print_get_stderr();
    int ret = vfprint(err, fmt, ap);
    va_end(ap);
    return ret;
}

int vbprint(char* buf, const char *fmt, va_list ap)
{
    return stbsp_vsprintf(buf, fmt, ap);
}

int bprint(char* buf, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vbprint(buf, fmt, ap);
    va_end(ap);
    return ret;
}

int vbnprint(char* buf, int n, const char *fmt, va_list ap)
{
    return stbsp_vsnprintf(buf, n, fmt, ap);
}

int bnprint(char* buf, int n, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vbnprint(buf, n, fmt, ap);
    va_end(ap);
    return ret;
}

    
#undef NEWLINE
#undef NEWLINE_LEN

#endif // PRINT_IMPLEMENTATION
      
#endif // PRINT_H
