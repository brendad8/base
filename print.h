
#ifndef PRINT_H
#define PRINT_H

// TODO(bcall): remove stdio dependency
//
// int   fprint     (FILE*, const char*, ...);
// int   fprintln   (FILE*, const char*, ...);

#include <stdarg.h>

int   print      (const char *fmt, ...);
int   println    (const char *fmt, ...);

int   vprint     (const char*, va_list);
int   vprintln   (const char*, va_list);

int   vfprint    (FILE*, const char*, va_list);
int   vfprintln  (FILE*, const char*, va_list);

#endif // PRINT_H
      
#ifdef PRINT_IMPLEMENTATION

// TODO(bcall): remove <stdio> dependency
#include <stdio.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

static char* print_cb(const char *buf, void* fout, int len)
{
    fwrite(buf, 1, len, (FILE*)fout);
    return (char *)buf;
}
static char* println_cb(const char *buf, void* fout, int len)
{
    fwrite(buf, 1, len, (FILE*)fout);
    char* res = (char *)buf;
    if (!res)
        fputc('\n', (FILE*)fout);
    return res;
}

int vfprint(FILE* fout, const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(print_cb, (void*)stdout, buffer, fmt, ap);
}

int vprint(const char *fmt, va_list ap)
{
    return vfprint(stdout, fmt, ap);
}

int vfprintln(FILE* fout, const char *fmt, va_list ap)
{
    char buffer[STB_SPRINTF_MIN];
    return stbsp_vsprintfcb(println_cb, (void*)stdout, buffer, fmt, ap);
}

int vprintln(const char *fmt, va_list ap)
{
    return vfprintln(stdout, fmt, ap);
}

int print(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vprint(fmt, ap);
    va_end(ap);
    return ret;
}

int println(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vprintln(fmt, ap);
    va_end(ap);
    return ret;
}

#endif // PRINT_IMPLEMENTATION
