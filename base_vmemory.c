
#include "base_vmemory.h"

/*********************************************************************************/
#if defined(PLATFORM_WINDOWS)
#include <windows.h>

/*********************************************************************************/
static VirtualMemoryInfo vm_get_info(void)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    VirtualMemoryInfo info = {0};
    info.page_size = (uint64_t)sysinfo.dwPageSize;
    info.allocation_granularity = (uint64_t)sysinfo.dwAllocationGranularity;
    return info;
}

/*********************************************************************************/
static void* vm_reserve(uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ALIGN_UP_POW2(size, info.allocation_granularity);
    void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    return ptr;
}

/*********************************************************************************/
static bool32_t vm_commit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ALIGN_UP_POW2(size, info.page_size);

    void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);

    return (result != NULL);
}

/*********************************************************************************/
static bool32_t vm_decommit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ALIGN_UP_POW2(size, info.page_size);
 
    BOOL result = VirtualFree(ptr, size, MEM_DECOMMIT);

    return (result != 0);
}

/*********************************************************************************/
static void vm_release(void* ptr, uint64_t size)
{
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}

/*********************************************************************************/
#elif defined(PLATFORM_LINUX)

#include <sys/mman.h>
#include <unistd.h>

/*********************************************************************************/
static VirtualMemoryInfo vm_get_info(void)
{
    uint64_t page_size = (uint64_t)sysconf(_SC_PAGESIZE);
    VirtualMemoryInfo info = {0};
    info.page_size = page_size;
    info.allocation_granularity = page_size;

    return info;
}

/*********************************************************************************/
static void* vm_reserve(uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ALIGN_UP_POW2(size, info.allocation_granularity);

    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED)
        return NULL;

    return ptr;
}

/*********************************************************************************/
static bool32 vm_commit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ALIGN_UP_POW2(size, info.page_size);

    int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);

    return (result == 0);
}

/*********************************************************************************/
static bool32 vm_decommit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();

    size = ALIGN_UP_POW2(size, info.page_size);

    // Return pages to OS. Memory remains mapped.
    int result = madvise(ptr, size, MADV_DONTNEED);

    // Make inaccessible again to emulate reserve state.
    mprotect(ptr, size, PROT_NONE);

    return (result == 0);
}

/*********************************************************************************/
static void vm_release(void* ptr, uint64_t size)
{
    munmap(ptr, size);
}

#endif
