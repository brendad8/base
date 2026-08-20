
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"

enum {
    ARGPARSE_UNKNOWN_ARG = -2
};

#define ARGPARSE_LONG_NAME   (1 << 1)
#define ARGPARSE_POSITIONAL  (1 << 2)

void argparse_init(arg_parser_t* self, arg_t* args, int flags)
{
    memset(self, 0, sizeof(*self));
    self->args = args;
    self->flags = flags;
}

static void argparse_error(arg_t* arg, char* reason, int flags)
{
    if (flags & ARGPARSE_LONG_NAME) 
        fprintf(stderr, "error: option `--%s` %s\n", arg->long_name, reason);
    else 
        fprintf(stderr, "error: option `-%c` %s\n", arg->short_name, reason);

    exit(EXIT_FAILURE);
}


static char* prefix_skip(char* str, char* prefix)
{
    size_t len = strlen(prefix);
    return strncmp(str, prefix, len) ? NULL : str + len;
}

static int prefix_cmp(char *str, char *prefix)
{
    for (;; str++, prefix++)
    {
        if (!*prefix)
            return 0;
        else if (*str != *prefix)
            return (unsigned char)*prefix - (unsigned char)*str;
    }
}

static int argparse_get_value(arg_parser_t* self, arg_t* arg, int flags)
{
    char *end_ptr = NULL;

    if (!arg->arg_ptr)
        goto skipped;

    switch (arg->arg_type) 
    {
        case ARGPARSE_TYPE_BOOL:
        {
            *(bool *)arg->arg_ptr = true;
            arg->is_set = true;
            break;
        }

        case ARGPARSE_TYPE_STRING:
        {
            if (self->current_arg)
            {
                *(char **)arg->arg_ptr = self->current_arg;
                arg->is_set = true;
                self->current_arg = NULL;
            } 
            else if (self->argc > 1) 
            {
                self->argc--;
                arg->is_set = true;
                *(char **)arg->arg_ptr = *++self->argv;
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
            if (self->current_arg) 
            {
                *(int *)arg->arg_ptr = strtol(self->current_arg, (char **)&end_ptr, 0);
                arg->is_set = true;
                self->current_arg = NULL;
            } 
            else if (self->argc > 1) 
            {
                self->argc--;
                *(int *)arg->arg_ptr = strtol(*++self->argv, (char **)&end_ptr, 0);
                arg->is_set = true;
            } 
            else 
            {
                argparse_error(arg, "requires a value", flags);
            }

            if (errno == ERANGE)
                argparse_error(arg, "numerical result out of range", flags);

            if (end_ptr[0] != '\0') // no digits or contains invalid characters
                argparse_error(arg, "expects an integer value", flags);

            break;
        }

        case ARGPARSE_TYPE_FLOAT:
        {
            errno = 0;
            if (self->current_arg) 
            {
                *(float *)arg->arg_ptr = strtof(self->current_arg, (char **)&end_ptr);
                self->current_arg = NULL;
                arg->is_set = true;
            } 
            else if (self->argc > 1) 
            {
                self->argc--;
                *(float *)arg->arg_ptr = strtof(*++self->argv, (char **)&end_ptr);
                arg->is_set = true;
            } 
            else 
            {
                argparse_error(arg, "requires a value", flags);
            }

            if (errno == ERANGE)
                argparse_error(arg, "numerical result out of range", flags);

            if (end_ptr[0] != '\0') // no digits or contains invalid characters
                argparse_error(arg, "expects a numerical value", flags);

            break;
        }

        default:
            assert(0);
    }

skipped:
    if (arg->callback)
        return arg->callback(self, arg);

    return 0;
}

static int argparse_parse_short_arg(arg_parser_t* self)
{
    for (arg_t* args = self->args; args->arg_type != ARGPARSE_TYPE_END; args++) 
    {
        if (args->short_name == *self->current_arg) 
        {
            return argparse_get_value(self, args, 0);
        }
    }
    return ARGPARSE_UNKNOWN_ARG;
}

static bool argparse_is_pos_arg(arg_t* arg)
{
    return arg->short_name == '\0' && arg->long_name == NULL;
}

static int argparse_parse_long_arg(arg_parser_t* self)
{
    for (arg_t* args = self->args; args->arg_type != ARGPARSE_TYPE_END; args++) 
    {
        char *rest;

        if (!args->long_name)
            continue;
      
        rest = prefix_skip(self->argv[0] + 2, args->long_name);
        if (rest != NULL) 
        {
            if (*rest == '=')
                self->current_arg = rest + 1;
            else if (*rest == '\0')
                self->current_arg = NULL;

            return argparse_get_value(self, args, ARGPARSE_LONG_NAME);
        }

    }
    return ARGPARSE_UNKNOWN_ARG;
}

static int argparse_parse_positional_arg(arg_parser_t* self)
{
    for (arg_t* arg = self->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (argparse_is_pos_arg(arg) && !arg->is_set)
        {
            return argparse_get_value(self, arg, self->flags);
        }
    }
    return ARGPARSE_UNKNOWN_ARG;
}

void argparse_parse(arg_parser_t* self, int argc, char* argv[])
{
    self->prog_name = argv[0];
    self->argc = argc - 1;
    self->argv = argv + 1;

    bool exit_at_unknown_arg = !(self->flags & ARGPARSE_IGNORE_UNKNOWN_ARGS);
    
    for (; self->argc > 0; self->argc--, self->argv++)
    {
        int status = 0;
        char* arg = self->argv[0];
        size_t arg_len = strlen(arg);

        if (arg_len > 1 && strncmp("--", arg, 2) == 0)
        {
            self->current_arg = arg + 2;
            status = argparse_parse_long_arg(self);
            if (status == ARGPARSE_UNKNOWN_ARG)
            {
                fprintf(stderr, "error: unknown option `%s`\n", self->argv[0]);
                if (exit_at_unknown_arg)
                    goto unknown;
            }
            self->current_arg=NULL;
        }
        else if (arg_len > 0 && *arg == '-')
        {
            self->current_arg = arg + 1;
            while (*self->current_arg != '\0')
            {
                status = argparse_parse_short_arg(self);
                if (status == ARGPARSE_UNKNOWN_ARG)
                {
                    fprintf(stderr, "error: unknown option `%c` in `%s`\n", *self->current_arg, self->argv[0]);
                    if (exit_at_unknown_arg)
                        goto unknown;
                }
                self->current_arg++;
            }
            self->current_arg=NULL;
        }
        else if (arg_len > 0)
        {
            self->current_arg = arg;
            status = argparse_parse_positional_arg(self);
            if (status == ARGPARSE_UNKNOWN_ARG)
            {
                fprintf(stderr, "error: unknown positional argument `%s`\n", self->argv[0]);
                if (exit_at_unknown_arg)
                    goto unknown;
            }
            self->current_arg=NULL;
        }

    unknown:
        if (status == ARGPARSE_UNKNOWN_ARG && exit_at_unknown_arg) 
        {
            argparse_print_usage(self);
            fputc('\n', stdout);
            argparse_print_help(self);
            exit(EXIT_FAILURE);
        }
    }

    for (arg_t* arg = self->args; arg->arg_type != ARGPARSE_TYPE_END; arg++)
    {
        if (argparse_is_pos_arg(arg) && !arg->is_set)
        {
            argparse_print_usage(self);
            fprintf(stderr, "error: the following argument is required: %s\n\n", arg->help);
            exit(EXIT_FAILURE);
        }
    }
}

void argparse_print_usage(arg_parser_t* self)
{
    fputc('\n', stdout);
    fprintf(stdout, "Usage: %s [-", self->prog_name);
    for (arg_t* arg = self->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (arg->short_name)
            fputc(arg->short_name, stdout);
    }
    fputs("]", stdout);

    for (arg_t* arg = self->args; arg->arg_type != ARGPARSE_TYPE_END; arg++) 
    {
        if (argparse_is_pos_arg(arg))
            if (arg->help)
                fprintf(stdout, " %s", arg->help);
    }
    fputc('\n', stdout);
}

void argparse_print_help(arg_parser_t* self)
{
    arg_t* args;

    // figure out best width
    size_t usage_opts_width = 0;
    size_t len;
    args = self->args;
    for (; args->arg_type != ARGPARSE_TYPE_END; args++) 
    {
        len = 0;
        if ((args)->short_name) 
        {
            len += 2;
        }
        if ((args)->short_name && (args)->long_name) 
        {
            len += 2;
        }
        if ((args)->long_name) 
        {
            len += strlen((args)->long_name) + 2;
        }
        if (args->arg_type == ARGPARSE_TYPE_INT) 
        {
            len += strlen("=<int>");
        }
        if (args->arg_type == ARGPARSE_TYPE_FLOAT) 
        {
            len += strlen("=<flt>");
        } 
        else if (args->arg_type == ARGPARSE_TYPE_STRING) 
        {
            len += strlen("=<str>");
        }

        len = (len + 3) - ((len + 3) & 3);
        if (usage_opts_width < len) 
        {
            usage_opts_width = len;
        }
    }
    usage_opts_width += 4;      // 4 spaces prefix

    args = self->args;
    for (; args->arg_type != ARGPARSE_TYPE_END; args++) 
    {
        size_t pos = 0;
        size_t pad = 0;
        if (args->arg_type == ARGPARSE_TYPE_GROUP) 
        {
            fputc('\n', stdout);
            fprintf(stdout, "%s", args->help);
            fputc('\n', stdout);
            continue;
        }

        if (argparse_is_pos_arg(args))
            continue;

        pos = fprintf(stdout, "    ");
        if (args->short_name) 
        {
            pos += fprintf(stdout, "-%c", args->short_name);
        }
        if (args->long_name && args->short_name) 
        {
            pos += fprintf(stdout, ", ");
        }
        if (args->long_name) 
        {
            pos += fprintf(stdout, "--%s", args->long_name);
        }

        if (args->arg_type == ARGPARSE_TYPE_INT) 
        {
            pos += fprintf(stdout, "=<int>");
        } 
        else if (args->arg_type == ARGPARSE_TYPE_FLOAT) 
        {
            pos += fprintf(stdout, "=<flt>");
        } 
        else if (args->arg_type == ARGPARSE_TYPE_STRING) 
        {
            pos += fprintf(stdout, "=<str>");
        }

        if (pos <= usage_opts_width) 
        {
            pad = usage_opts_width - pos;
        } 
        else 
        {
            fputc('\n', stdout);
            pad = usage_opts_width;
        }
        fprintf(stdout, "%*s%s\n", (int)pad + 2, "", args->help ? args->help : "");
    }

}

int argparse_help_cb_no_exit(arg_parser_t* self, arg_t* option)
{
    (void)option;
    argparse_print_usage(self);
    fputc('\n', stdout);
    argparse_print_help(self);
    return 0;
}

int argparse_help_cb(arg_parser_t* self, arg_t* option)
{
    argparse_help_cb_no_exit(self, option);
    exit(EXIT_SUCCESS);
}
