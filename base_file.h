
#pragma once

#include "base_core.h"
#include "base_thread_context.h"
#include "base_allocator.h"
#include "base_string.h"

typedef struct 
{
   u64  fd;

} File;

typedef int FileAccessFlags;
enum
{
    FileAccess_Read       = 1 << 0,
    FileAccess_Write      = 1 << 1,
    FileAccess_Execute    = 1 << 2,
    FileAccess_Append     = 1 << 3,
    FileAccess_ShareRead  = 1 << 4,
    FileAccess_ShareWrite = 1 << 5
};

// typedef int FileIterFlags;
// enum
// {
//   FILE_ITER_SKIP_DIRS    = (1 << 0),
//   FILE_ITER_SKIP_FILES   = (1 << 1),
//   FILE_ITER_SKIP_HIDDEN  = (1 << 2),
//   FILE_ITER_DONE         = (1 << 31)
// };
//
// typedef struct
// {
//   FileIterFlags flags;
//   u8 memory[800];
//
// } FileIter;
//
//
// typedef int FilePropertyFlags;
// enum
// {
//     FILE_PROPERTY_DIR = (1 << 0)
// };
//
// typedef struct
// {
//     int64 file_size;
//     DenseTime created; 
//     DenseTime modified;
//     FilePropertyFlags flags;
//
// } FileProperties;
//
// typedef struct 
// {
//     string name;
//     FileProperties properties;
//
// } FileInfo;

File   file_stdin       (void);
File   file_stdout      (void);
File   file_stderr      (void);

File   file_open        (const u8* path, FileAccessFlags flags);
File   file_open_str8   (String8 path, FileAccessFlags flags);

void   file_close       (File file);

int    file_write       (File file, const u8* buf, int size);
int    file_write_str8  (File file, String8 str);

int    file_read        (File file, u8* buf, int size);
int    file_read_str8   (File file, String8* str);

String8   file_read_all                   (Allocator allocator, File file);
// String8   file_read_all_from_path         (Allocator allocator, const u8* path);
// String8   file_read_all_from_path_str8    (Allocator allocator, String8 path);

u64    file_get_size    (File file);

u64    file_get_read_pos     (File file);
b32    file_set_read_pos     (File file, u64 pos);

// bool   file_iter_start  (FileIter* iter, u8* path, FileIterFlags flags);
// bool   file_iter_next   (FileIter* iter, FileInfo* info);
// void   file_iter_end    (FileIter* iter);
