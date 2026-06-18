@echo off

zig cc arena_test.c -o arena_test.exe -s
zig cc ds_test.c -o ds_test.exe -s
zig cc string_test.c -o string_test.exe -s

arena_test.exe
ds_test.exe
string_test.exe

