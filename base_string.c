
#include "base_string.h"

String8 str8_new(u8* ptr, u64 len)
{
    String8 result = {
        .ptr = ptr,
        .len = len
    };
    return result;
}
