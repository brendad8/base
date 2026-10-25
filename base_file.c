
#if OS_WINDOWS
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
#endif

#include "base_file.h"

File
file_stdin(void)
{
    File result = {0};
#if OS_WINDOWS
    result.fd = (u64)GetStdHandle(STD_INPUT_HANDLE);
#else
    result.fd = (u64)STDIN_FILENO;
#endif
    return result;
}

File 
file_stdout(void)
{
    File result = {0};
#if OS_WINDOWS
    result.fd = (u64)GetStdHandle(STD_OUTPUT_HANDLE);
#else
    result.fd = (u64)STDOUT_FILENO;
#endif
    return result;
}

File 
file_stderr(void)
{
    File result = {0};
#if OS_WINDOWS
    result.fd = (u64)GetStdHandle(STD_ERROR_HANDLE);
#else
    result.fd = (u64)STDERR_FILENO;
#endif
    return result;
}

File 
file_open(const u8* path, FileAccessFlags flags)
{
    File result = {0};
#if OS_WINDOWS
    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;
    if(flags & FileAccess_Read)        {access_flags |= GENERIC_READ;}
    if(flags & FileAccess_Write)       {access_flags |= GENERIC_WRITE;}
    if(flags & FileAccess_Execute)     {access_flags |= GENERIC_EXECUTE;}
    if(flags & FileAccess_ShareRead)   {share_mode |= FILE_SHARE_READ;}
    if(flags & FileAccess_ShareWrite)  {share_mode |= FILE_SHARE_WRITE|FILE_SHARE_DELETE;}
    if(flags & FileAccess_Write)       {creation_disposition = CREATE_ALWAYS;}
    if(flags & FileAccess_Append)      {creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA; }
    HANDLE handle = CreateFileA(path, access_flags, share_mode, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if(handle != INVALID_HANDLE_VALUE)
    {
        result.fd = (uint64)handle;
    }
    else
    {
        DWORD err = GetLastError();
        (void)err;
    }
#else
    int open_flags = 0;
    if ((flags & FileAccess_Read) && (flags & FileAccess_Write))
        open_flags = O_RDWR;
    else if (flags & FileAccess_Write)
        open_flags = O_WRONLY;
    else if (flags & FileAccess_Read)
        open_flags = O_RDONLY;
    if (flags & FileAccess_Append)
        open_flags |= O_APPEND;
    if (flags & (FileAccess_Write | FileAccess_Append))
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

File 
file_open_str8(String8 path, FileAccessFlags flags)
{
    File result = {0};
    ArenaTemp temp = tctx_scratch_begin(0, 0);
    u8* path_temp = arena_push_array(temp.arena, u8, path.len + 1); 
    mem_copy(path_temp, path.ptr, path.len);
    path_temp[path.len] = '\0';
    result = file_open(path_temp, flags);
    tctx_scratch_end(temp);
    return result;
}

void 
file_close(File file)
{
#if OS_WINDOWS
    BOOL result = CloseHandle((HANDLE)file.fd);
    (void)result;
#else
    close(file.fd);
#endif
    file.fd = 0;
}

int 
file_write(File file, const u8* buf, int len)
{
    int result;
#if OS_WINDOWS
    WriteFile((HANDLE)(file.fd), (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)write((int)file.fd, (unsigned char*)buf, (size_t)len);
#endif
    return result;
}

int 
file_read(File file, u8* buf, int len)
{
    int result;
#if OS_WINDOWS
    ReadFile((HANDLE)(file.fd), (void*)buf, (DWORD)len, (DWORD*)&result, NULL);
#else
    result = (int)read((int)file.fd, (void*)buf, (size_t)len);
#endif
    return result;
}

u64
file_get_size(File file)
{
	i64 result = 0;
#if OS_WINDOWS
    LARGE_INTEGER file_size = {0};
    BOOL success = GetFileSizeEx((HANDLE)file.fd, &file_size);
    if (success)
	{
        result = (u64)file_size.QuadPart;
	}
#else
    struct stat file_stats = {0};
    int status = fstat(file.fd, &file_stats);
	if (status != -1)
	{
        result = (u64)file_stats.st_size;
	}
#endif
	return result;
}

String8 file_read_all(Allocator a, File file)
{
    String8 result = {0};
    u64 file_size = file_get_size(file);
    if (file_size)
    {
        u8* buffer = mem_alloc(a, file_size + 1, alignof(u8));
        if (buffer)
        {
            int bytes_read = file_read(file, buffer, file_size);
            (void)bytes_read;
            buffer[file_size] = '\0';

            result.ptr = buffer;
            result.len = file_size;
        }
    }
    return result;
}


u64 file_get_read_pos(File file)
{
    i64 result = 0;
#if OS_WINDOWS
    LARGE_INTEGER zero = {0};
    LARGE_INTEGER position;
    if (!SetFilePointerEx((HANDLE)file.fd, zero, &position, FILE_CURRENT))
        result = -1;
    else
        result =(u64)position.QuadPart;
#else
    off_t pos = lseek((int)file.fd, 0, SEEK_CUR);
    if (pos == (off_t)-1)
        result = -1;
    else 
        result = (u64)pos;
#endif
    return result;
}

b32 file_set_read_pos(File file, u64 pos)
{
    b32 result = 0;
#if OS_WINDOWS
    LARGE_INTEGER offset;
    offset.QuadPart = pos;
    if (SetFilePointerEx((HANDLE)file.fd, offset, NULL, FILE_BEGIN))
    {
        result = 1;
    }
#else
    off_t status = lseek((int)file.fd, pos, SEEK_SET);
    if (status != (off_t)-1)
    {
        result = 1;
    }
#endif
    return result;
}

// bool file_iter_start(FileIter* iter, u8* path, FileIterFlags flags)
// bool file_iter_start(ArenaTemp* tmp, u8* path, FileIterFlags flags)
// {
// }
// bool file_iter_next(FileIter* iter, FileInfo* info)
// {
// }
// void file_iter_end(FileIter* iter)
// {
// }
