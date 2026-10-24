
#include "base_str.h"

str8 str8_new(u8* ptr, u64 len)
{
    str8 result = {
        .ptr = ptr,
        .len = len
    };
    return result;
}
