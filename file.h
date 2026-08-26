
/* file.h - file and filesystem functions

   To use this library, do this in *one* C file:
      #define DATETIME_IMPLEMENTATION
      #include "base/datetime.h"
*/

#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stdint.h>
#include <stdbool.h>

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

typedef int FileIterFlags;
enum
{
  FILE_ITER_SKIP_DIRS    = (1 << 0),
  FILE_ITER_SKIP_FILES   = (1 << 1),
  FILE_ITER_SKIP_HIDDEN  = (1 << 2),
  FILE_ITER_DONE         = (1 << 31)
};

typedef struct
{
  FileIterFlags flags;
  char memory[800];

} FileIter;

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

File      file_stdin        (void);
File      file_stdout       (void);
File      file_stderr       (void);

File      file_open         (char*, FileAccessFlags);
void      file_close        (File);

int       file_write        (File, const char*, int);
int       file_read         (File, char*, int);

int64_t   file_get_size     (File file);
int64_t   file_get_pos      (File file);
int       file_set_pos      (File file, int64_t pos);

// bool      file_iter_start   (FileIter* iter, char* path, FileIterFlags flags);
// bool      file_iter_next    (FileIter* iter, FileInfo* info);
// void      file_iter_end     (FileIter* iter);

#ifdef __cplusplus
}
#endif

#endif // FILE_H

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef FILE_IMPLEMENTATION

#ifdef _WIN32
    #include <windows.h>
    #define NEWLINE "\r\n"
    #define NEWLINE_LEN 2
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #define NEWLINE "\n"
    #define NEWLINE_LEN 1
#endif

File file_stdin(void)
{
    File result = {0};
#ifdef _WIN32
    result.fd = (uint64_t)GetStdHandle(STD_INPUT_HANDLE);
#else
    result.fd = (uint64_t)STDIN_FILENO;
#endif
    return result;
}

File file_stdout(void)
{
    File result = {0};
#ifdef _WIN32
    result.fd = (uint64_t)GetStdHandle(STD_OUTPUT_HANDLE);
#else
    result.fd = (uint64_t)STDOUT_FILENO;
#endif
    return result;
}

File file_stderr(void)
{
    File result = {0};
#ifdef _WIN32
    result.fd = (uint64_t)GetStdHandle(STD_ERROR_HANDLE);
#else
    result.fd = (uint64_t)STDERR_FILENO;
#endif
    return result;
}

File file_open(char* path, FileAccessFlags flags)
{
    File result = {0};
#ifdef _WIN32
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

    HANDLE handle = CreateFileA(path, access_flags, share_mode, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if(handle != INVALID_HANDLE_VALUE)
    {
        result.fd = (uint64_t)handle;
    }
    else
    {
        DWORD err = GetLastError();
        (void)err;
    }
#else
    int open_flags = 0;
    if ((flags & FILE_ACCESS_READ) && (flags & FILE_ACCESS_WRITE))
        open_flags = O_RDWR;
    else if (flags & FILE_ACCESS_WRITE)
        open_flags = O_WRONLY;
    else if (flags & FILE_ACCESS_READ)
        open_flags = O_RDONLY;

    if (flags & FILE_ACCESS_APPEND)
        open_flags |= O_APPEND;

    if (flags & (FILE_ACCESS_WRITE | FILE_ACCESS_APPEND))
        open_flags |= O_CREAT;

    open_flags |= O_CLOEXEC;

    int fd = open((char *)path, open_flags, 0755);
    if(fd != -1)
    {
        result.fd = fd;
    }
#endif
    return result;
}

void file_close(File file)
{
#ifdef _WIN32
    BOOL result = CloseHandle((HANDLE)file.fd);
    (void)result;
#else
    close(file.fd);
#endif
    file.fd = 0;
}

int file_write(File file, const char* buf, int len)
{
    int result;
#ifdef _WIN32
    WriteFile((HANDLE)(file.fd), (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)write((int)file.fd, buf, (size_t)len);
#endif
    return result;
}

int file_read(File file, char* buf, int len)
{
    int result;
#ifdef _WIN32
    ReadFile((HANDLE)(file.fd), (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)read((int)file.fd, (void*)buf, (size_t)len);
#endif
    return result;
}

int64_t file_get_size(File file)
{
#ifdef _WIN32
    LARGE_INTEGER file_size = {0};
    BOOL success = GetFileSizeEx((HANDLE)file.fd, &file_size);
    if (success)
        return (int64_t)file_size.QuadPart;
    else
        return 0;
#else
    struct stat file_stats = {0};
    fstat(file.fd, &file_stats);
    return (int64_t)file_stats.st_size;
#endif
}

int64_t file_get_pos(File file)
{
    int64_t result = 0;
#ifdef _WIN32
    LARGE_INTEGER zero = {0};
    LARGE_INTEGER position;

    if (!SetFilePointerEx((HANDLE)file.fd, zero, &position, FILE_CURRENT))
        result = -1;
    else
        result =(int64_t)position.QuadPart;
#else
    off_t pos = lseek(fd, 0, SEEK_CUR);
    if (pos == (off_t)-1)
        result = -1;
    else 
        result = (int64_t)pos;
#endif
    return result;
}

int file_set_pos(File file, int64_t pos)
{
#ifdef _WIN32
    LARGE_INTEGER offset;
    offset.QuadPart = pos;
    if (!SetFilePointerEx((HANDLE)file.fd, offset, NULL, FILE_BEGIN))
        return 0;
#else
    off_t result = lseek((int)file.fd, pos, SEEK_SET);
    if (result == (off_t)-1)
        return 0;
#endif
    return 1;
}

#endif // FILE_IMPLEMENTATION

