
#ifndef FILE_H
#define FILE_H

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stdint.h>

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef struct 
{
   uint64_t  fd;

} File;

typedef int FileAccessFlags;
enum
{
    FILE_ACCESS_READ        = 1 << 0,
    FILE_ACCESS_WRITE       = 1 << 1,
    FILE_ACCESS_EXECUTE     = 1 << 2,
    FILE_ACCESS_APPEND      = 1 << 3,
    FILE_ACCESS_SHARE_READ  = 1 << 4,
    FILE_ACCESS_SHARE_WRITE = 1 << 5
};


/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

File   file_stdin    (void);
File   file_stdout   (void);
File   file_stderr   (void);

File   file_open     (char*, FileAccessFlags);
void   file_close    (File);

int    file_write    (File, const char*, int);

#endif // FILE_H

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef FILE_IMPLEMENTATION

#ifdef _WIN32
#include "windows.h"
#endif

File file_stdin(void)
{
    File result = {0};
    result.fd = (uint64_t)GetStdHandle(STD_INPUT_HANDLE);
    return result;
}

File file_stdout(void)
{
    File result = {0};
    result.fd = (uint64_t)GetStdHandle(STD_OUTPUT_HANDLE);
    return result;
}

File file_stderr(void)
{
    File result = {0};
    result.fd = (uint64_t)GetStdHandle(STD_ERROR_HANDLE);
    return result;
}

File file_open(char* file_name, FileAccessFlags flags)
{
    File result = {0};

    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;

    if(flags & FILE_ACCESS_READ)        {access_flags |= GENERIC_READ;}
    if(flags & FILE_ACCESS_WRITE)       {access_flags |= GENERIC_WRITE;}
    if(flags & FILE_ACCESS_EXECUTE)     {access_flags |= GENERIC_EXECUTE;}
    if(flags & FILE_ACCESS_SHARE_READ)  {share_mode |= FILE_SHARE_READ;}
    if(flags & FILE_ACCESS_SHARE_WRITE) {share_mode |= FILE_SHARE_WRITE|FILE_SHARE_DELETE;}
    if(flags & FILE_ACCESS_WRITE)       {creation_disposition = CREATE_ALWAYS;}
    if(flags & FILE_ACCESS_APPEND)      {creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA; }

    HANDLE file = CreateFileA(file_name, access_flags, share_mode, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if(file != INVALID_HANDLE_VALUE)
    {
        result.fd = (uint64_t)file;
    }
    else
    {
        DWORD err = GetLastError();
        (void)err;
    }
    return result;
}

void file_close(File file)
{
    HANDLE handle = (HANDLE)file.fd;
    BOOL result = CloseHandle(handle);
    (void)result;
}

int file_write(File file, const char* buf, int len)
{
    DWORD written;
    WriteFile((HANDLE)(file.fd), (void*)buf, (DWORD)len, &written, NULL);
    return written;
}


#endif // FILE_IMPLEMENTATION
