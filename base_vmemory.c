
#include "base_vmemory.h"

/*********************************************************************************/
#if defined(PLATFORM_WINDOWS)
#include <windows.h>

/*********************************************************************************/
static VirtualMemoryInfo VM_GetInfo(void)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    VirtualMemoryInfo info = {0};
    info.page_size = (uint64)sysinfo.dwPageSize;
    info.allocation_granularity = (uint64)sysinfo.dwAllocationGranularity;
    return info;
}

/*********************************************************************************/
static void* VM_Reserve(uint64 size)
{
    VirtualMemoryInfo info = VM_GetInfo();
    size = ALIGN_UP_POW2(size, info.allocation_granularity);
    void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    return ptr;
}

/*********************************************************************************/
static bool32 VM_Commit(void* ptr, uint64 size)
{
    VirtualMemoryInfo info = VM_GetInfo();
    size = ALIGN_UP_POW2(size, info.page_size);

    void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);

    return (result != NULL);
}

/*********************************************************************************/
static bool32 VM_Decommit(void* ptr, uint64 size)
{
    VirtualMemoryInfo info = VM_GetInfo();
    size = ALIGN_UP_POW2(size, info.page_size);
 
    BOOL result = VirtualFree(ptr, size, MEM_DECOMMIT);

    return (result != 0);
}

/*********************************************************************************/
static void VM_Release(void* ptr, uint64 size)
{
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}

/*********************************************************************************/
#elif defined(PLATFORM_LINUX)

#include <sys/mman.h>
#include <unistd.h>

/*********************************************************************************/
static VirtualMemoryInfo VM_GetInfo(void)
{
    uint64 page_size = (uint64)sysconf(_SC_PAGESIZE);
    VirtualMemoryInfo info = {0};
    info.page_size = page_size;
    info.allocation_granularity = page_size;

    return info;
}

/*********************************************************************************/
static void* VM_Reserve(uint64 size)
{
    VirtualMemoryInfo info = VM_GetInfo();
    size = ALIGN_UP_POW2(size, info.allocation_granularity);

    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED)
        return NULL;

    return ptr;
}

/*********************************************************************************/
static bool32 VM_Commit(void* ptr, uint64 size)
{
    VirtualMemoryInfo info = VM_GetInfo();
    size = ALIGN_UP_POW2(size, info.page_size);

    int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);

    return (result == 0);
}

/*********************************************************************************/
static bool32 VM_Decommit(void* ptr, uint64 size)
{
    VirtualMemoryInfo info = VM_GetInfo();

    size = ALIGN_UP_POW2(size, info.page_size);

    // Return pages to OS. Memory remains mapped.
    int result = madvise(ptr, size, MADV_DONTNEED);

    // Make inaccessible again to emulate reserve state.
    mprotect(ptr, size, PROT_NONE);

    return (result == 0);
}

/*********************************************************************************/
static void VM_Release(void* ptr, uint64 size)
{
    munmap(ptr, size);
}

#endif
