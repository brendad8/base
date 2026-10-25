
#include "base_include.h"

int main(void)
{
    tctx_alloc();

    fmt_print("hi mom!\n");

    int x = 1000;
    char buf[1024];
    fmt_print_buf(buf, "int = %d\n", x);
    fmt_print("%s\n", buf);

    int needed = fmt_count("%s\n", buf);
    fmt_print("needed = %d\n", needed);

    File file = file_open((const u8*)"test.c", FileAccess_Read);
    String8 contents = file_read_all(c_allocator, file);
    fmt_print(str8_fmt "\n", str8_varg(contents));

    Arena* arena = arena_alloc((ArenaParams){.commit_size = 4 * 1024, .reserve_size = 4 * 1024 });

    void* mem = arena_push(arena, KB(2));
    assert(mem);
    mem = arena_push(arena, KB(2));
    assert(mem);

    Allocator arena_allocator = arena_make_allocator(arena);
    file_set_read_pos(file, 0);
    String8 contents2 = file_read_all(arena_allocator, file);
    fmt_print(str8_fmt "\n", str8_varg(contents2));

    file_close(file);

    arena_release(arena);

    ArenaTemp temp1 = tctx_scratch_begin(0, 0);
    assert(temp1.arena);
    
    Allocator temp_arena_allocator = arena_make_allocator(temp1.arena);
    String8 msg = fmt_print_alloc(temp_arena_allocator, "hi mom my favorite number is %d", 8);
    fmt_print(str8_fmt "\n", str8_varg(msg));

    ArenaTemp temp2 = tctx_scratch_begin(&temp1.arena, 1);
    assert(temp2.arena);
    assert(temp1.arena != temp2.arena);

    tctx_scratch_end(temp2);
    tctx_scratch_end(temp1);
    
    String8 path = str8_lit("test.c");
    file = file_open_str8(path, FileAccess_Read);
    assert(file.fd != 0);
    file_close(file);

    return 0;
}
