
Base C Library:

    arena.h    - virtual memory backed arena allocator

    argparse.h - command line argument parsing

    darray.h   - dynamic array macros

    datetime.h - date and time types and functions

    file.h     - file and filesystem functions

    linklist.h - intrusive linked list data structure macros

    map.h      - hash map and string map macros

    print.h    - basic print functions


Dependency Chain:

stb_sprintf.h -> print.h -> argparse.h
             \
              -> file.h  


