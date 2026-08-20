
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

int   fprint     (File, const char*, ...);
int   fprintln   (File, const char*, ...);

int   vprint     (const char*, va_list);
int   vprintln   (const char*, va_list);

int   vfprint    (File*, const char*, va_list);
int   vfprintln  (File*, const char*, va_list);

#endif // PRINT_H
      
/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef PRINT_IMPLEMENTATION

// NOTE(bcall): without NOUNALIGNED define, zig cc debug programs will panic for known reason. 
#define STB_SPRINTF_NOUNALIGNED
#define STB_SPRINTF_IMPLEMENTATION
#include "third_party/stb_sprintf.h"

#define FILE_IMPLEMENTATION
#include "file.h"

static char* print_cb(const char *buf, void* file, int len)
{
    File out = *(File*)file;
    file_write(out, buf, len);
    return (char *)buf;
}

// static char* println_cb(const char *buf, void* fout, int len)
// {
//     fwrite(buf, 1, len, (FILE*)fout);
//     char* res = (char *)buf;
//     if (!res)
//         fputc('\n', (FILE*)fout);
//     return res;
// }

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

// int vfprintln(FILE* fout, const char *fmt, va_list ap)
// {
//     char buffer[STB_SPRINTF_MIN];
//     return stbsp_vsprintfcb(println_cb, (void*)stdout, buffer, fmt, ap);
// }

// int vprintln(const char *fmt, va_list ap)
// {
//     return vfprintln(stdout, fmt, ap);
// }

int print(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vprint(fmt, ap);
    va_end(ap);
    return ret;
}

// int println(const char *fmt, ...)
// {
//     va_list ap;
//     va_start(ap, fmt);
//     int ret = vprintln(fmt, ap);
//     va_end(ap);
//     return ret;
// }

#endif // PRINT_IMPLEMENTATION
