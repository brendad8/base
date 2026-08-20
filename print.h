
/* print.h - print functions

   To use this library, do this in *one* C file:
      #define PRINT_IMPLEMENTATION
      #include "base/rand.h"

   Dependencies:
       stb_sprintf - v1.10 - public domain snprintf() implementation
*/

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
    #define NEWLINE "\r\n"
    #define NEWLINE_LEN 2
#else
    #define NEWLINE "\n"
    #define NEWLINE_LEN 1
#endif

#define FILE_IMPLEMENTATION
#include "file.h"

static char* print_cb(const char *buf, void* file, int len)
{
    File out = *(File*)file;
    file_write(out, buf, len);
    return (char *)buf;
}

static char* println_cb(const char *buf, void* file, int len)
{
    File out = *(File*)file;
    file_write(out, buf, len);
    char* res = (char *)buf;
    if (!res)
        file_write(out, NEWLINE, NEWLINE_LEN);

    return res;
}

int vfprint(File* file, const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_cb, (void*)file, buffer, fmt, ap);
}

int vprint(const char *fmt, va_list ap)
{
    File std_out = file_stdout();
    return vfprint(&std_out, fmt, ap);
}

int vfprintln(File* file, const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(println_cb, (void*)file, buffer, fmt, ap);
}

int vprintln(const char *fmt, va_list ap)
{
    File std_out = file_stdout();
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
    File std_out = file_stdout();
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprint(&std_out, fmt, ap);
    va_end(ap);
    return ret;
}

int println(const char* fmt, ...)
{
    File std_out = file_stdout();
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintln(&std_out, fmt, ap);
    va_end(ap);
    return ret;
}

#endif // PRINT_IMPLEMENTATION
