
/* print.h - print functions

   To use this library, do this in *one* C file:
      #define PRINT_IMPLEMENTATION
      #include "base/rand.h"

   Dependencies:
       stb_sprintf - v1.10 - public domain snprintf() implementation
*/

// TODO(bcall): println not working...

#ifndef PRINT_H
#define PRINT_H

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stdarg.h>

#include "file.h"

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

int   print      (const char *fmt, ...);
int   println    (const char *fmt, ...);

int   fprint     (File*, const char* fmt, ...);
int   fprintln   (File*, const char* fmt, ...);

int   vprint     (const char* fmt, va_list);
int   vprintln   (const char* fmt, va_list);

int   vfprint    (File*, const char* fmt, va_list);
int   vfprintln  (File*, const char* fmt, va_list);

#endif // PRINT_H
      
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
    #define NEWLINE "\r\n"
    #define NEWLINE_LEN 2
#else
    #include <unistd.h>
    #define NEWLINE "\n"
    #define NEWLINE_LEN 1
#endif


static File get_stdout(void)
{
    File result = {0};
#ifdef _WIN32
    result.fd = (uint64_t)GetStdHandle(STD_OUTPUT_HANDLE);
#else
    result.fd = (uint64_t)STDOUT_FILENO;
#endif
    return result;
}

static int print_file_write(File file, const char* buf, int len)
{
    int result;
#ifdef _WIN32
    WriteFile((HANDLE)(file.fd), (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)write((int)file.fd, buf, (size_t)len);
#endif
    return result;
}

static char* print_cb(const char *buf, void* file, int len)
{
    File out = *(File*)file;
    print_file_write(out, buf, len);
    return (char *)buf;
}

int vfprint(File* file, const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_cb, (void*)file, buffer, fmt, ap);
}

int vprint(const char *fmt, va_list ap)
{
    File std_out = get_stdout();
    return vfprint(&std_out, fmt, ap);
}

int vfprintln(File* file, const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    int result = stbsp_vsprintfcb(print_cb, (void*)file, buffer, fmt, ap);

    char* newline = NEWLINE;
    int newline_len = NEWLINE_LEN;
    result += print_file_write(*file, newline, newline_len);

    return result;
}

int vprintln(const char *fmt, va_list ap)
{
    File std_out = get_stdout();
    return vfprintln(&std_out, fmt, ap);
}

int fprint(File* file, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprint(file, fmt, ap);
    va_end(ap);
    return ret;
}

int fprintln(File* file, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintln(file, fmt, ap);
    va_end(ap);
    return ret;
}

int print(const char* fmt, ...)
{
    File std_out = get_stdout();
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprint(&std_out, fmt, ap);
    va_end(ap);
    return ret;
}

int println(const char* fmt, ...)
{
    File std_out = get_stdout();
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintln(&std_out, fmt, ap);
    va_end(ap);
    return ret;
}

#endif // PRINT_IMPLEMENTATION
