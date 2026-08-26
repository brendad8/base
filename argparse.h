
/* argparse.h - command line argument parsing

   To use this library, do this in *one* C file:
      #define ARGPARSE_IMPLEMENTATION
      #include "base/argparse.h"
*/

#ifndef ARGPARSE_H
#define ARGPARSE_H

// TODO(bcall): 
//      add positonal arguments to help
//      add color option for text

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

/***************************************************************************
 *          MACROS
 ***************************************************************************/

#define ARG_FLAG(...)    { ARGPARSE_TYPE_FLAG, __VA_ARGS__ }
#define ARG_INT(...)     { ARGPARSE_TYPE_INT, __VA_ARGS__ }
#define ARG_FLOAT(...)   { ARGPARSE_TYPE_FLOAT, __VA_ARGS__ }
#define ARG_STRING(...)  { ARGPARSE_TYPE_STRING, __VA_ARGS__ }
#define ARG_HELP()       ARG_FLAG(NULL, 'h', "help", "show this help message and exit", argparse_help_cb, NULL, 0)
#define ARG_END(...)     { ARGPARSE_TYPE_END, NULL, '\0', NULL, NULL, NULL, NULL, 0 }

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef int ArgParseFlags;
enum 
{
    ARGPARSE_STOP_AT_UNKNOWN = 1 << 0,
    ARGPARSE_IGNORE_UNKNOWN  = 1 << 1,
};

typedef int ArgParseType;
enum 
{
    ARGPARSE_TYPE_END,
    ARGPARSE_TYPE_FLAG,
    ARGPARSE_TYPE_INT,
    ARGPARSE_TYPE_FLOAT,
    ARGPARSE_TYPE_STRING,
};

typedef struct Arg Arg;
typedef struct ArgParser ArgParser;
typedef int ArgCallback(ArgParser* parser, Arg* arg);

typedef int ArgFlags;
enum
{
    ARG_POSITIONAL = 1 << 0,
    ARG_REQUIRED   = 1 << 1,
    ARG_HIDDEN     = 1 << 2,
    ARG_IS_SET     = 1 << 30
};

struct Arg
{
    ArgParseType arg_type;
    void* arg_ptr;

    char short_name;
    char* long_name;
    char* help;

    ArgCallback* callback;
    void* cb_data;
    
    ArgFlags flags;
};

struct ArgParser
{
    Arg* args;      
    ArgParseFlags flags; 
    char* prog_name;
    char* usage;
    int argc;         
    char** argv;      
    char* current_arg;
};

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

void   argparse_init              (ArgParser* parser, Arg* args, char* prog_name, char* usage, int flags);
void   argparse_parse             (ArgParser* parser, int argc, char* argv[]);
void   argparse_print_usage       (ArgParser* parser);
void   argparse_print_help        (ArgParser* parser);

int    argparse_help_cb           (ArgParser* parser, Arg* arg);
int    argparse_help_cb_no_exit   (ArgParser* parser, Arg* arg);

#ifdef __cplusplus
}
#endif

#endif // ARGPARSE_H

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef ARGPARSE_IMPLEMENTATION

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifdef PRINT_IMPLEMENTATION
    #undef PRINT_IMPLEMENTATION
#endif
#include "print.h"

#define ARGPARSE_UNKNOWN_ARG -1

#define ARGPARSE_LONG_NAME  1 << 0
#define ARGPARSE_POSITIONAL 1 << 1

#ifdef ARGPARSE_NO_COLOR
    #define ANSI_RESET          ""
    #define ANSI_BLACK          ""
    #define ANSI_RED            ""
    #define ANSI_GREEN          ""
    #define ANSI_YELLOW         ""
    #define ANSI_BLUE           ""
    #define ANSI_MAGENTA        ""
    #define ANSI_CYAN           ""
    #define ANSI_WHITE          ""
    #define ANSI_BRIGHT_BLACK   ""
    #define ANSI_BRIGHT_RED     ""
    #define ANSI_BRIGHT_GREEN   ""
    #define ANSI_BRIGHT_YELLOW  ""
    #define ANSI_BRIGHT_BLUE    ""
    #define ANSI_BRIGHT_MAGENTA ""
    #define ANSI_BRIGHT_CYAN    ""
    #define ANSI_BRIGHT_WHITE   ""
#else
    #define ANSI_RESET          "\x1b[0m"
    #define ANSI_BLACK          "\x1b[30m"
    #define ANSI_RED            "\x1b[31m"
    #define ANSI_GREEN          "\x1b[32m"
    #define ANSI_YELLOW         "\x1b[33m"
    #define ANSI_BLUE           "\x1b[34m"
    #define ANSI_MAGENTA        "\x1b[35m"
    #define ANSI_CYAN           "\x1b[36m"
    #define ANSI_WHITE          "\x1b[37m"
    #define ANSI_BRIGHT_BLACK   "\x1b[90m"
    #define ANSI_BRIGHT_RED     "\x1b[91m"
    #define ANSI_BRIGHT_GREEN   "\x1b[92m"
    #define ANSI_BRIGHT_YELLOW  "\x1b[93m"
    #define ANSI_BRIGHT_BLUE    "\x1b[94m"
    #define ANSI_BRIGHT_MAGENTA "\x1b[95m"
    #define ANSI_BRIGHT_CYAN    "\x1b[96m"
    #define ANSI_BRIGHT_WHITE   "\x1b[97m"
#endif

void argparse_init(ArgParser* parser, Arg* args, char* prog_name, char* usage, int flags)
{
    memset(parser, 0, sizeof(ArgParser));
    parser->args = args;
    parser->prog_name = prog_name;
    parser->usage = usage;
    parser->flags = flags;

    Arg* arg;
    for (arg = args; arg->arg_type != ARGPARSE_TYPE_END; arg++)
    {
        if (arg->flags & ARG_POSITIONAL)
        {
            assert(arg->help && "help field is required for positional args");
            assert(!arg->short_name && "short name field should be `\\0` for positional args");
            assert(!arg->long_name && "long name field should be `NULL` for positional args");
        }
        else
        {
            assert((arg->short_name || arg->long_name) && "one of short name, long name fields are required for non-positional args");
        }
    }
}

static void argparse_error(Arg* arg, char* reason, int flags)
{
    if (flags & ARGPARSE_LONG_NAME) 
        eprintln(ANSI_RED "error: option `--%s` %s" ANSI_RESET, arg->long_name, reason);
    else if (flags & ARGPARSE_POSITIONAL)
        eprintln(ANSI_RED "error: option `--%s` %s" ANSI_RESET, arg->help, reason);
    else 
        eprintln(ANSI_RED "error: option `-%c` %s" ANSI_RESET, arg->short_name, reason);

    exit(EXIT_FAILURE);
}

static int argparse_get_value(ArgParser* parser, Arg* arg, int flags)
{
    char *end_ptr = NULL;

    if (arg && arg->arg_ptr)
    {
        switch (arg->arg_type) 
        {
            case ARGPARSE_TYPE_FLAG:
            {
                *(int*)arg->arg_ptr = true;
                arg->flags |= ARG_IS_SET;
                break;
            }

            case ARGPARSE_TYPE_STRING:
            {
                if (parser->current_arg)                 
                {
                    *(char**)arg->arg_ptr = parser->current_arg;
                    arg->flags |= ARG_IS_SET;
                    parser->current_arg = NULL;
                } 
                else if (parser->argc > 1) 
                {
                    parser->argc--;
                    parser->argv++;
                    *(char **)arg->arg_ptr = *parser->argv;
                    arg->flags |= ARG_IS_SET;
                } 
                else 
                {
                    argparse_error(arg, "requires a value", flags);
                }

                break;
            }

            case ARGPARSE_TYPE_INT:
                {
                    errno = 0;
                    if (parser->current_arg) 
                    {
                        *(int *)arg->arg_ptr = strtol(parser->current_arg, (char **)&end_ptr, 0);
                        arg->flags |= ARG_IS_SET;
                        parser->current_arg = NULL;
                    } 
                    else if (parser->argc > 1) 
                    {
                        parser->argc--;
                        parser->argv++;
                        *(int*)arg->arg_ptr = strtol(*parser->argv, (char**)&end_ptr, 0);
                        arg->flags |= ARG_IS_SET;
                    } 
                    else 
                    {
                        argparse_error(arg, "requires a value", flags);
                    }

                    if (errno == ERANGE)
                        argparse_error(arg, "numerical result out of range", flags);

                    if (end_ptr[0] != '\0') 
                        argparse_error(arg, "expects an integer value", flags);

                    break;
                }

            case ARGPARSE_TYPE_FLOAT:
                {
                    errno = 0;
                    if (parser->current_arg) 
                    {
                        *(float *)arg->arg_ptr = strtof(parser->current_arg, (char **)&end_ptr);
                        arg->flags |= ARG_IS_SET;
                        parser->current_arg = NULL;
                    } 
                    else if (parser->argc > 1) 
                    {
                        parser->argc--;
                        parser->argv++;
                        *(float *)arg->arg_ptr = strtof(*parser->argv, (char **)&end_ptr);
                        arg->flags |= ARG_IS_SET;
                    } 
                    else 
                    {
                        argparse_error(arg, "requires a value", flags);
                    }

                    if (errno == ERANGE)
                        argparse_error(arg, "numerical result out of range", flags);

                    if (end_ptr[0] != '\0')
                        argparse_error(arg, "expects a numerical value", flags);

                    break;
                }

            default:
                assert(0);
        }
    }

    if (arg && arg->callback)
        return arg->callback(parser, arg);

    return 0;
}

static int argparse_parse_short(ArgParser* parser, char short_name)
{
    Arg* arg = NULL;
    for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (arg->short_name == short_name) 
        {
            return argparse_get_value(parser, arg, 0);
        }
    }
    return ARGPARSE_UNKNOWN_ARG;
}

static int argparse_parse_long(ArgParser* parser, char* long_name)
{
    Arg* arg = NULL;
    for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (arg->long_name)
        {
            int len = strlen(arg->long_name);
            if (strncmp(arg->long_name, long_name, len) == 0)
            {
                if (!long_name[len])
                {
                    parser->current_arg = NULL;
                }
                else if (long_name[len] == '=')
                {
                    parser->current_arg = &long_name[len+1];
                }
                else if (long_name[len] != '\0')
                {
                    continue;
                }
                return argparse_get_value(parser, arg, ARGPARSE_LONG_NAME);
            }
        }
    }
    return ARGPARSE_UNKNOWN_ARG;
}

static int argparse_parse_positional(ArgParser* parser)
{
    Arg* arg = NULL;
    for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (arg->flags & ARG_POSITIONAL && !(arg->flags & ARG_IS_SET))
        {
            return argparse_get_value(parser, arg, ARGPARSE_POSITIONAL);
        }
    }

    return ARGPARSE_UNKNOWN_ARG;

}

void argparse_parse(ArgParser* parser, int argc, char* argv[])
{
    int status = 0;

    if (!parser->prog_name)
    {
        parser->prog_name = argv[0];
    }

    parser->argc = argc - 1;
    parser->argv = argv + 1;

    while (parser->argc)
    {
        char* arg_str = parser->argv[0];

        if (arg_str[0] == '-')
        {
            if (arg_str[1] == '-')
            {
                parser->current_arg = arg_str + 2;
                status = argparse_parse_long(parser, arg_str + 2);
                if (status == ARGPARSE_UNKNOWN_ARG && parser->flags & ARGPARSE_STOP_AT_UNKNOWN)
                {
                    eprint(ANSI_RED "error: unknown option `%s`\n" ANSI_RESET, arg_str);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                // NOTE(bcall): while loop for short names to handle -abc like -a -b -c
                char* short_name = arg_str + 1;
                while (*short_name)
                {
                    status = argparse_parse_short(parser, *short_name);
                    if (status == ARGPARSE_UNKNOWN_ARG && parser->flags & ARGPARSE_STOP_AT_UNKNOWN)
                    {
                        eprintln(ANSI_RED "error: unknown option `%c`" ANSI_RESET, *short_name);
                        exit(EXIT_FAILURE);
                    }
                    short_name++;
                }
            }
        }
        else
        {
            parser->current_arg = arg_str;
            status = argparse_parse_positional(parser);
            
            if (parser->flags & ARGPARSE_STOP_AT_UNKNOWN)
            {
                eprintln(ANSI_RED "error: unknown option `%s`" ANSI_RESET, arg_str);
                exit(EXIT_FAILURE);
            }
        }

        parser->argc--;
        parser->argv++;
    }

    Arg* arg;
    int missing_arg_count = 0;
    for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (arg->flags & ARG_REQUIRED && !(arg->flags & ARG_IS_SET))
        {
            missing_arg_count++;
        }
    }
    if (missing_arg_count)
    {
        eprint(ANSI_RED "error: the following arguments are required:" ANSI_RESET);
        int missing_arg_idx = 0;
        for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
        {
            if (arg->flags & ARG_REQUIRED && !(arg->flags & ARG_IS_SET))
            {
                if (arg->flags & ARG_POSITIONAL)
                {
                    eprint(" %s", arg->help);
                }
                else
                {
                    if (arg->long_name && arg->long_name)
                        eprint(" --%s/-%c", arg->long_name, arg->short_name);
                    else if (arg->long_name)
                        eprint(" --%s", arg->long_name);
                    else if (arg->short_name)
                        eprint(" -%c", arg->short_name);
                }
                missing_arg_idx++;
                if (missing_arg_idx != missing_arg_count)
                    eprint(",");
                else
                    eprint("\n");
            }
        }
        exit(EXIT_FAILURE);
    }
}


void argparse_print_usage(ArgParser* parser)
{
    if (parser->usage)
    {
        print("Usage: %s\n", parser->usage);
    }
    else if (parser->prog_name)
    {
        print("Usage: %s", parser->prog_name);

        Arg* arg;
        for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
        {
            if (arg->flags & ARG_POSITIONAL)
            {
                if (arg->help)
                    print(" %s", arg->help);
            }
        }
        print(" [arguments]\n");
    }
}


void argparse_print_help(ArgParser* parser)
{
    Arg* arg;

    size_t usage_opts_width = 0;
    size_t len;

    for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if ((arg->flags & ARG_POSITIONAL) || (arg->flags & ARG_HIDDEN))
            continue;

        len = 0;
        if ((arg)->short_name) 
        {
            len += 2;
        }
        if ((arg)->short_name && (arg)->long_name) 
        {
            len += 2;
        }
        if ((arg)->long_name) 
        {
            len += strlen((arg)->long_name) + 2;
        }

        len = (len + 3) - ((len + 3) & 3);
        if (usage_opts_width < len) 
        {
            usage_opts_width = len;
        }
    }
    usage_opts_width += 4;      // 4 spaces prefix

    for (arg = parser->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if ((arg->flags & ARG_POSITIONAL) || (arg->flags & ARG_HIDDEN))
            continue;

        size_t pos = 0;
        size_t pad = 0;

        pos = eprint("    ");
        if (arg->short_name) 
        {
            pos += eprint("-%c", arg->short_name);
        }
        if (arg->long_name && arg->short_name) 
        {
            pos += eprint(", ");
        }
        if (arg->long_name) 
        {
            pos += eprint("--%s", arg->long_name);
        }

        if (pos <= usage_opts_width) 
        {
            pad = usage_opts_width - pos;
        } 
        else 
        {
            print("\n");
            pad = usage_opts_width;
        }
        print("%*s%s\n", (int)pad + 2, "", arg->help ? arg->help : "");
    }
}

int argparse_help_cb_no_exit(ArgParser* parser, Arg* arg)
{
    (void)arg;
    print("\n");
    argparse_print_usage(parser);
    print("\n");
    argparse_print_help(parser);
    return 0;
}

int argparse_help_cb(ArgParser* parser, Arg* arg)
{
    argparse_help_cb_no_exit(parser, arg);
    exit(EXIT_SUCCESS);
}

// int argparse_console_supports_color(void)
// {
// #ifdef _WIN32
//     HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
//
//     if (h == INVALID_HANDLE_VALUE)
//         return 0;
//
//     DWORD mode;
//     if (!GetConsoleMode(h, &mode))
//         return 0;  // Not a console, e.g. redirected output
//
//     if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
//         return 1;
//
//     // Try enabling VT processing.
//     if (SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
//         return 1;
//
//     return 0;
// #else
//     if (!isatty(STDOUT_FILENO))
//         return 0;
//
//     const char *term = getenv("TERM");
//
//     if (!term || strcmp(term, "dumb") == 0)
//         return 0;
//
//     return 1;
// #endif
// }

#endif // ARGPARSE_IMPLEMENTATION
