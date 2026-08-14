/**
 * Copyright (C) 2012-2015 Yecheng Fu <cofyc.jackson at gmail dot com>
 * All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be found
 * in the LICENSE file.
 */
#ifndef ARGPARSE_H
#define ARGPARSE_H

/* For c++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct arg_parser_t arg_parser_t;
typedef struct arg_t arg_t;
typedef int argparse_callback(arg_parser_t *parser, arg_t* arg);

enum argparse_flag {
    ARGPARSE_STOP_AT_UNKNOWN_ARG  = 1 << 0,
    ARGPARSE_IGNORE_UNKNOWN_ARGS  = 1 << 1,
};

enum argparse_arg_type {
    /* special */
    ARGPARSE_TYPE_END,
    ARGPARSE_TYPE_GROUP,
    /* options with no arguments */
    ARGPARSE_TYPE_BOOL,
    /* options with arguments (optional or required) */
    ARGPARSE_TYPE_INT,
    ARGPARSE_TYPE_FLOAT,
    ARGPARSE_TYPE_STRING,
};

struct arg_t
{
    enum argparse_arg_type arg_type;
    void* arg_ptr;

    char short_name;
    char* long_name;
    char* help;

    argparse_callback* callback;
    intptr_t cb_data;

    char is_set;
};

struct arg_parser_t 
{
    arg_t* args;      
    int flags;        
                      
    char* prog_name;  
    int argc;         
    char** argv;      
    char** out;
    int cpidx;
    char* current_arg;

    int pos_arg_idx;
};

int argparse_help_cb(arg_parser_t* self, arg_t* arg);
int argparse_help_cb_no_exit(arg_parser_t* self, arg_t* arg);

// built-in option macros
#define ARG_BOOL(...)    { ARGPARSE_TYPE_BOOL, __VA_ARGS__, 0 }
#define ARG_INT(...)     { ARGPARSE_TYPE_INT, __VA_ARGS__, 0 }
#define ARG_FLOAT(...)   { ARGPARSE_TYPE_FLOAT, __VA_ARGS__, 0 }
#define ARG_STRING(...)  { ARGPARSE_TYPE_STRING, __VA_ARGS__, 0 }
#define ARG_GROUP(h)     { ARGPARSE_TYPE_GROUP, NULL, '\0' NULL, h, NULL, 0, NULL, 0}
#define ARG_HELP()       ARG_BOOL(NULL, 'h', "help", "show this help message and exit", argparse_help_cb, 0)
#define ARG_END(...)     { ARGPARSE_TYPE_END, NULL, '\0', NULL, NULL, NULL, 0, 0 }

void argparse_init         (arg_parser_t* self, arg_t* args, int flags);
void argparse_parse        (arg_parser_t* self, int argc, char* argv[]);
void argparse_print_usage  (arg_parser_t* self);
void argparse_print_help   (arg_parser_t* self);

#ifdef __cplusplus
}
#endif

#endif
