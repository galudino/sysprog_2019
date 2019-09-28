/**
 *  @file       check.c
 *  @brief      Client source file for Asst0
 *
 *  @author     Gemuele Aludino
 *  @date       19 Sep 2019
 *  @copyright  Copyright © 2019 Gemuele Aludino
 */
/**
 *  Copyright © 2019 Gemuele Aludino
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *  THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#else
#define false 0
#define true 1
typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
#endif /* __STD_VERSION__ >= 19990L */

#define BUFFER_SIZE 256
#define MAXIMUM_STACK_BUFFER_SIZE 16384

#define KNRM "\x1B[0;0m"   /**< reset to standard color/weight */
#define KNRM_b "\x1B[0;1m" /**< standard color bold */

#define KGRY "\x1B[0;2m" /**< dark grey */

#define KBNK "\x1B[0;5m" /**< blink every second */

#define KCYN "\x1B[0;36m" /**< cyan */
#define KWHT "\x1B[0;37m" /**< white */

#define KRED_b "\x1B[1;31m" /**< red bold */
#define KGRN_b "\x1B[1;32m" /**< green bold */
#define KYEL_b "\x1B[1;33m" /**< yellow bold */
#define KMAG_b "\x1B[1;35m" /**< magenta bold */
#define KCYN_b "\x1B[1;36m" /**< cyan bold */
#define KWHT_b "\x1B[1;37m" /**< white bold */

/**< utils: debugging */
int ulog(FILE *dest,
         const char *level, /**< meant for "LOG" or "ERROR" */
         const char *file,  /**< meant for use with the __FILE__ macro */
         const char *func,  /**< meant for use with the __func__ macro */
         long double line,  /**< meant for use with the __LINE__ macro */
         const char *fmt,   /**< user's custom message */
         ...);

bool ulog_attrs_disable[] = { false, false, false, false, false, false, false };

/**
 *  Unless you would like to create a customized
 *  debugging message, please use the following preprocessor directives.
 *
 *  LOG is a general-purpose messaging tool.
 *  ERROR is used for displaying error messages (i.e. something failed, etc)
 *
 *  The first argument in LOG and ERROR is FILEMACRO,
 *  which refers to the following macro:
 *          __FILE__
 *  This is just a string, so if you are building a custom ulog message,
 *  with ERROR or WARNING, and would like to put a different
 *  string in place of __FILE__, you may do so.
 */
/**
 *  @def        LOG
 *  @brief      Shorthand macro for ulog to create messages for a program
 *
 *  Use the preprocessor directive
 *      #define ULOG_DISABLE_BUG
 *  before the inclusion of utils.h (or before these directives)
 *  to disable the LOG macro.
 */
#if __STDC_VERSION__ >= 199901L
#ifndef ULOG_DISABLE_LOG
#define LOG(FILEMACRO, ...)                                                    \
    ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__, __VA_ARGS__)
#else
#define LOG(FILEMACRO, ...)
#endif /* ULOG_DISABLE_LOG */
#else
#ifndef ULOG_DISABLE_LOG
#define LOG(FILEMACRO, MSG)                                                    \
    ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__, MSG)
#else
#define LOG(FILEMACRO, MSG)
#endif /* ULOG_DISABLE_LOG */
#endif /* __STDC_VERSION__ >= 199901L */

/**
 *  @def        ERROR
 *  @brief      Shorthand macro for ulog to display errors for a program
 *
 *  Use the preprocessor directive
 *      #define ULOG_DISABLE_ERROR
 *  before the inclusion of utils.h (or before these directives)
 *  to disable the ERROR macro.
 */
#if __STDC_VERSION__ >= 199901L
#ifndef ULOG_DISABLE_ERROR
#define ERROR(FILEMACRO, ...)                                                  \
    ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__, (long int)__LINE__, __VA_ARGS__)
#endif /* ULOG_DISABLE_ERROR */
#else
#ifndef ULOG_DISABLE_ERROR
#define ERROR(FILEMACRO, MSG)                                                  \
    ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__, (long int)__LINE__, MSG)
#else
#define ERROR(FILEMACRO, MSG)
#endif /* ULOG_DISABLE_ERROR */
#endif /* __STDC_VERSION__ >= 199901L */

/**
 *  @def        WARNING
 *  @brief      Shorthand macro for ulog to display warning for a program
 */
#if __STDC_VERSION__ >= 199901L
#ifndef ULOG_DISABLE_WARNING
#define WARNING(FILEMACRO, ...)                                                \
    ulog(ULOG_STREAM_WARNING, "[WARNING]", FILEMACRO, __func__, (long int)__LINE__, __VA_ARGS__)
#endif /* ULOG_DISABLE_WARNING */
#else
#ifndef ULOG_DISABLE_WARNING
#define WARNING(FILEMACRO, MSG)                                                \
    ulog(ULOG_STREAM_WARNING, "[WARNING]", FILEMACRO, __func__, (long int)__LINE__, MSG)
#else
#define WARNING(FILEMACRO, MSG)
#endif /* ULOG_DISABLE_WARNING */
#endif /* __STDC_VERSION__ >= 199901L */

/**
 *  @def        ULOG_DISABLE_ALL
 *  @brief      Shorthand macro to disable the following preprocessor macros:
 *              LOG, ERROR
 *
 *  Use the preprocessor directive
 *      #define ULOG_DISABLE_ALL
 *  before the inclusion of utils.h (or before any of the directives below)
 *  to disable the macros ERROR and WARNING all at once.
 *
 *  You may also use any combination of these macros to keep some
 *  of the ulog macro types active.
 *
 *  ulog has the following format:
 *  (level is what appears in LEVEL, usually [ERROR] or [WARNING]
 *  if using the ulog macros. Using the ulog function allows you to customize
 *  your own error message)
 *  MMM dd yyyy HH:mm:ss LEVEL [filepath/filename:linenumber] function_name
 *message
 */
#ifdef ULOG_DISABLE_ALL
#define ULOG_DISABLE_LOG
#define ULOG_DISABLE_ERROR
#define ULOG_DISABLE_WARNING
#endif

#define UTILS_LOG_ATTRS_COUNT 7
enum ULOG_ATTRS {
    DATE,
    TIME,
    LEVEL,
    FILENAME,
    LINE,
    FUNCTION,
    MESSAGE
};

bool ulog_attrs_disable[UTILS_LOG_ATTRS_COUNT];

#define ULOG_TOGGLE_ATTR(ULOG_ATTR)                                            \
    ulog_attrs_disable[ULOG_ATTR] = (ulog_attrs_disable[ULOG_ATTR]) ? (false) : (true)

/** Turn off ulog attributes by invoking one or more of these in a function.
ULOG_TOGGLE_ATTR(DATE);
ULOG_TOGGLE_ATTR(TIME);
ULOG_TOGGLE_ATTR(LEVEL);
ULOG_TOGGLE_ATTR(FILENAME);
ULOG_TOGGLE_ATTR(LINE);
ULOG_TOGGLE_ATTR(FUNCTION);
ULOG_TOGGLE_ATTR(MESSAGE);
*/

/**< Designated default streams for LOG, and ERROR  */
#define ULOG_STREAM_LOG stdout
#define ULOG_STREAM_ERROR stderr
#define ULOG_STREAM_WARNING stderr

/**
 *  Custom assert function with message string -
 *  message prints to stderr, just like the assert macro in assert.h,
 *  and the message string is printed using the ulog function --
 *  wrapped with the ERROR macro defined in this header file.
 *  Then, just like the assert macro, abort() is invoked
 *  and the program ends.
 *
 *  Unlike the original assert macro,
 *  NDEBUG will not disable massert - massert
 *  will persist whether you are in debug mode, or release mode.
 *
 *  massert is most useful when a program is no longer fit
 *  to continue, given a particular condition --
 *  a description message of your choice can be provided.
 *
 *  If no message is preferred, you may provide an empty string.
 */
#define massert(CONDITION, MESSAGE)                                                              \
    do {                                                                                         \
        if (((CONDITION) == (false))) {                                                          \
            fprintf(stderr, "Assertion failed: (%s)\n", #CONDITION);                             \
            ulog(ULOG_STREAM_ERROR, "[ERROR]", __FILE__, __func__, (long int)__LINE__, MESSAGE); \
            abort();                                                                             \
        }                                                                                        \
    } while (0)

#define massert_ptr(PTR)                                                       \
    ;                                                                          \
    massert(PTR,                                                               \
            "['" #PTR "' was found to be NULL - '" #PTR                        \
            "' must be nonnull to continue.]");

#define massert_malloc(PTR)                                                    \
    ;                                                                          \
    massert(PTR,                                                               \
            "[Request for heap storage allocation failed (malloc returned "    \
            "NULL and was assigned to '" #PTR "')]");

#define massert_calloc(PTR)                                                    \
    ;                                                                          \
    massert(PTR,                                                               \
            "[Request for heap storage allocation failed (calloc returned "    \
            "NULL and was assigned to '" #PTR "')]");

#define massert_realloc(PTR)                                                   \
    ;                                                                          \
    massert(PTR,                                                               \
            "[Request for heap storage reallocation failed (realloc returned " \
            "NULL and was assigned to '" #PTR "')]");

#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
/**< gcs: string utilities */
char *gcs__strcpy(char *dst, const char *src);
size_t gcs__strlen(const char *src);
int gcs__strcmp(const char *c1, const char *c2);
int gcs__strncmp(const char *c1, const char *c2, size_t n);
char *gcs__strtok(char *src, const char *delim);

void *gcs__memcpy(void *dst, const void *src, size_t width);
void *gcs__memmove(void *dst, const void *src, size_t width);
void *gcs__memset(void *dst, int ch, size_t n);

#else

#define gcs__strcpy strcpy
#define gcs__strlen strlen
#define gcs__strcmp strcmp
#define gcs__strncmp strncmp
#define gcs__strtok strtok
#define gcs__strtok_r strtok_r
#define gcs__memcpy memcpy
#define gcs__memmove memmove
#define gcs__memset memset

#endif /* !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_) */

#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
#define streql(s1, s2) gcs__strcmp(s1, s2) == 0
#define strneql(s1, s2, n) gcs__strncmp(s1, s2, n) == 0

#else

#define streql(s1, s2) strcmp(s1, s2) == 0
#define strneql(s1, s2, n) strncmp(s1, s2, n) == 0
#endif

#define gcs__strdup(dst, src)                                                  \
    do {                                                                       \
        dst = malloc(gcs__strlen(src) + 1);                                    \
        massert_malloc(dst);                                                   \
        gcs__strcpy(dst, src);                                                 \
    } while (0)

/**< check: client functions - logging/errors */
int check__fexpr_log(FILE *dest, uint32_t ct_expr, uint32_t ct_logical, uint32_t ct_arithmetic);
int check__fexpr_err(FILE *dest,
                     const char *err_type,
                     const char *desc,
                     const char *expr_fragmt,
                     uint32_t ct_expr,
                     int index);
void check__arg_check(int argc, const char *argv[]);

#define check__expr_log(ct_expr, ct_logical, ct_arithmetic)                    \
    fexpr_log(stdout, ct_expr, ct_logical, ct_arithmetic)

#define check__expr_err(err_type, desc, expr_fragmt, ct_expr, index)           \
    fexpr_err(stderr, err_type, desc, expr_fragmt, ct_expr, index)

#define check__fexpr_ok(dest) fprintf(dest, "%s%s%s", KGRN_b, "OK.", KNRM)
#define check__expr_ok() fexpr_ok(stdout)

/**< check: client functions - tokenize */
void check__expr_populate(char **expr, char *input_string, const char *delimiter);

/**< check: client functions - assess */
bool check__expr_assess(const char *expr,
                        const char *operands[],
                        const char *operators[],
                        const char *delimiter);

#define CHECK__OPERANDS                                                        \
    "false", "true", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"

#define CHECK__OPERATORS "+", "-", "*", "/", "AND", "OR", "NOT"

#define CHECK__USAGE "USAGE: ./check [input string]"

enum check__operand_type {
    CHECK_false,
    CHECK_true,
    CHECK_0,
    CHECK_1,
    CHECK_2,
    CHECK_3,
    CHECK_4,
    CHECK_5,
    CHECK_6,
    CHECK_7,
    CHECK_8,
    CHECK_9
};

enum check__operator_type {
    CHECK__plus,
    CHECK__minus,
    CHECK__multiply,
    CHECK__divide,
    CHECK__AND,
    CHECK__OR,
    CHECK__NOT
};

struct gcs__vstr {
    struct gcs__vstr_base {
        char **start;
        char **finish;
        char **end_of_storage;
    } impl;
};

typedef struct gcs__vstr gcs__vstr;
#define GCS__VSTR_INITIAL_SIZE 4

/*#define GCS__VSTR_DEEP_COPY*/

void gcs__vstr_init(gcs__vstr *v, size_t capacity);
void gcs__vstr_deinit(gcs__vstr *v);

size_t gcs__vstr_size(gcs__vstr *v);
size_t gcs__vstr_capacity(gcs__vstr *v);
bool gcs__vstr_empty(gcs__vstr *v);

char **gcs__vstr_at(gcs__vstr *v, size_t index);
char **gcs__vstr_front(gcs__vstr *v);
char **gcs__vstr_back(gcs__vstr *v);

void gcs__vstr_resize(gcs__vstr *v, size_t n);

void gcs__vstr_pushb(gcs__vstr *v, char **straddr);
void gcs__vstr_popb(gcs__vstr *v);
void gcs__vstr_clear(gcs__vstr *v);

void gcs__vstr_puts(gcs__vstr *v);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    Argument count (sizeof(argv))
 *  @param[in]  argv    Command line arguments
 *
 *  @return     0 on success, else failure
 */
int main(int argc, const char *argv[]) {
    const char *operands[] = { CHECK__OPERANDS };
    const char *operators[] = { CHECK__OPERATORS };

    const char *input_string = argv[1];
    const char *delimiter = NULL;

    char *buff = NULL;

    size_t size = 0;
    size_t i = 0;
    char *curr = NULL;

    gcs__vstr vec_str = { { NULL, NULL, NULL } };
    gcs__vstr *v = &vec_str;

    check__arg_check(argc, argv);
    gcs__strdup(buff, input_string);

    gcs__vstr_init(v, GCS__VSTR_INITIAL_SIZE);

    delimiter = ";";
    curr = gcs__strtok(buff, delimiter);

    gcs__vstr_pushb(v, &curr);

    while ((curr = gcs__strtok(NULL, delimiter)) != NULL) {
        gcs__vstr_pushb(v, &curr);
    }

    size = gcs__vstr_size(v);
    delimiter = " ";

    ulog(stdout, "" KWHT_b "[BEG]" KNRM "", __FILE__, __func__, __LINE__, "%s", "======");

    for (i = 0; i < size; i++) {
        char *curr = NULL;
        char *expr = NULL;
        char *token = NULL;

        curr = *(gcs__vstr_at(v, i));

        ulog(stdout, "" KYEL_b "[EXP]" KNRM "", __FILE__, __func__, __LINE__, "%s", curr);
        ulog(stdout, "[beg]", __FILE__, __func__, __LINE__, "%s", "------");
        token = gcs__strtok(curr, delimiter);

        ulog(stdout, "" KCYN_b "[TOK]" KNRM "", __FILE__, __func__, __LINE__, "%s", token);

        while ((token = gcs__strtok(NULL, curr)) != NULL) {
            ulog(stdout, "" KCYN_b "[TOK]" KNRM "", __FILE__, __func__, __LINE__, "%s", token);
        }

        ulog(stdout, "[end]", __FILE__, __func__, __LINE__, "%s", "------");
    }

    ulog(stdout, "" KWHT_b "[END]" KNRM "", __FILE__, __func__, __LINE__, "%s", "======");

    gcs__vstr_deinit(v);

    free(buff);
    buff = NULL;

    return EXIT_SUCCESS;
}

bool check__expr_assess(const char *expr,
                        const char *operands[],
                        const char *operators[],
                        const char *delim) {
    char str[256];
    sprintf(str, "expr: %s", expr);
    LOG(__FILE__, str);
    return false;
}

int check__fexpr_log(FILE *dest, uint32_t ct_expr, uint32_t ct_logical, uint32_t ct_arithmetic) {
    char buffer[BUFFER_SIZE];
    int j = 0;

    j = sprintf(buffer + j,
                "Found %s%d%s expressions: %s%d%s logical and %s%d%s "
                "arithmetic.",
                KWHT_b,
                ct_expr,
                KNRM,
                KWHT_b,
                ct_logical,
                KNRM,
                KWHT_b,
                ct_arithmetic,
                KNRM);

    return fprintf(dest, "%s\n", buffer);
}

int check__fexpr_err(FILE *dest,
                     const char *err_type,
                     const char *desc,
                     const char *expr_fragmt,
                     uint32_t ct_expr,
                     int index) {
    char buffer[BUFFER_SIZE];
    char spaces[BUFFER_SIZE];
    int i = 0;
    int j = 0;

    gcs__strcpy(spaces, "\t");

    if (index > -1) {
        for (i = 1; i <= index + 1; i++) {
            *(spaces + i) = ' ';
        }
    }

    j += sprintf(buffer + j,
                 "%s: " KWHT_b "%s" KNRM " in " KMAG_b "expression %d" KNRM
                 ": %s in\n\t\"%s\"\n",
                 KRED_b "Error" KNRM,
                 err_type,
                 ct_expr,
                 desc,
                 expr_fragmt);

    if (index > -1) {
        j += sprintf(buffer + j, "%s%s", spaces, KYEL_b "^" KNRM);
    }

    return fprintf(dest, "%s\n", buffer);
}

void check__arg_check(int argc, const char *argv[]) {
    uint32_t i = 0;
    char msg[4096];
    size_t input_len = 0;

    i += sprintf(msg + i,
                 KNRM "\n\n%s\n",
                 "USAGE:\n./check" KWHT_b " [input string]" KNRM);

    i += sprintf(msg + i,
                 "\n%s\n%s\n\n",
                 "Argument count (argc) must be 2; argument "
                 "values are \"./check\" and [input "
                 "string].",
                 "[input_string] must have a length greater than 0.");

    i += sprintf(msg + i,
                 "%s\n\n",
                 "The " KWHT_b "[input string]" KNRM
                 " consists of one or more " KWHT_b "expressions." KNRM);

    i += sprintf(msg + i,
                 "%s\n",
                 "An " KWHT_b "expression" KNRM
                 " is comprised of multiple " KWHT_b "tokens" KNRM " --");
    i += sprintf(msg + i,
                 "%s\n\n",
                 "which will denote an " KWHT_b "operand" KNRM "," KWHT_b
                 " operator" KNRM ", or " KWHT_b "delimiter." KNRM);

    i += sprintf(msg + i,
                 "%s\n",
                 "Possible operands: { \"false\", \"true\", "
                 "\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", "
                 "\"6\", \"7\", \"8\", \"9\" }");
    i += sprintf(msg + i,
                 "%s\n",
                 "Possible operators: { \"AND\", \"OR\", "
                 "\"NOT\", \"+\", \"-\", \"*\", \"/\" }" KNRM);
    i += sprintf(msg + i, "%s\n\n", "Possible delimiters: { \" \", \";\" }");

    i += sprintf(msg + i,
                 "%s\n",
                 "The delimiter \" \" (one character of "
                 "whitespace) must appear:");
    i += sprintf(msg + i, "\t%s\n", "after every operand and operator,");
    i += sprintf(msg + i,
                 "\t\t%s\n",
                 "except for the last operand that ends the input string.");
    i += sprintf(msg + i,
                 "%s\n",
                 "The delimiter \";\" must appear after each expression,");
    i += sprintf(msg + i,
                 "\t%s\n",
                 "except for the last expression of an input string --");
    i += sprintf(msg + i,
                 "\t\t%s\n\n",
                 "if and only if the input string has at least 2 expressions.");

    i += sprintf(msg + i, "%s\n", "Both \"false\" and \"true\"");
    i +=
        sprintf(msg + i, "\t%s\n\n", "are " KWHT_b "logical" KNRM " operands.");
    i += sprintf(msg + i,
                 "%s\n",
                 "\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", "
                 "\"6\", \"7\", \"8\", and \"9\"");
    i += sprintf(msg + i,
                 "\t%s\n\n",
                 "are " KWHT_b "arithmetic" KNRM " operands.");

    i += sprintf(msg + i,
                 "%s\n",
                 "NOT is a " KWHT_b "unary logical" KNRM " operator.");
    i += sprintf(msg + i,
                 "%s\n",
                 "An legal expression consisting of a unary logical operator");
    i += sprintf(msg + i,
                 "%s\n",
                 "has one logical operand, which appears to "
                 "the right of the operator, like this:");
    i += sprintf(msg + i, "\t%s\n\n", "\"NOT true\"");

    i += sprintf(msg + i, "%s\n", "AND and OR are " KWHT_b "binary logical" KNRM " operators.");
    i += sprintf(msg + i,
                 "%s\n",
                 "Legal expressions consisting of a binary logical operator");
    i += sprintf(msg + i,
                 "%s\n",
                 "have two logical operands, which appear on "
                 "each side of the operator; they can look "
                 "like this:");
    i += sprintf(msg + i, "\t%s\n", "\"true OR false\"");
    i += sprintf(msg + i, "\t%s\n", "\"false AND true\"");
    i += sprintf(msg + i, "\t%s\n\n", "\"true OR false; false AND true\"");

    i += sprintf(msg + i,
                 "%s\n",
                 "+, -, *, and / are " KWHT_b "binary arithmetic" KNRM
                 " operators.");
    i +=
        sprintf(msg + i,
                "%s\n",
                "Legal expressions consisting of a binary arithmetic operator");
    i += sprintf(msg + i,
                 "%s\n",
                 "have two arithmetic operands, which appear "
                 "on each side of the operator; they can look "
                 "like this:");
    i += sprintf(msg + i, "\t%s\n", "\"2 + 2\"");
    i += sprintf(msg + i, "\t%s\n", "\"9 - 5\"");
    i += sprintf(msg + i, "\t%s\n\n", "\"2 * 2; 9 / 5\"");

    i += sprintf(msg + i,
                 "%s\n",
                 "There is no limit as to how many "
                 "expressions can go in an input string,");
    i += sprintf(msg + i,
                 "%s\n",
                 "and logical/arithmetic expressions may "
                 "coexist within the same input string.");
    i += sprintf(msg + i,
                 "\t%s\n",
                 "However, mixing logical operands with arithmetic operators,");
    i += sprintf(msg + i,
                 "\t%s\n\n",
                 "or arithmetic operands with logical operators is illegal.");

    i += sprintf(msg + i,
                 "%s\n",
                 "Sample usage cases (program invocation and input string):");
    i += sprintf(msg + i, "\t%s\n", "./check \"2 + 2\"");
    i += sprintf(msg + i, "\t%s\n", "./check \"2 + 2; true OR false\"");
    i += sprintf(msg + i,
                 "\t%s\n",
                 "./check \"false AND false; NOT true; 9 / 5\"");

    input_len = argc >= 2 ? gcs__strlen(argv[1]) : input_len;
    massert((argc == 2) && (input_len > 0), msg);
}

void gcs__vstr_init(gcs__vstr *v, size_t capacity) {
    v->impl.start = calloc(capacity, sizeof *v->impl.start);
    massert_calloc(v->impl.start);

    v->impl.finish = v->impl.start;
    v->impl.end_of_storage = v->impl.start + capacity;
}

void gcs__vstr_deinit(gcs__vstr *v) {
    gcs__vstr_clear(v);

    free(v->impl.start);
    v->impl.start = v->impl.finish = v->impl.end_of_storage = NULL;
}

size_t gcs__vstr_size(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.finish - v->impl.start;
}

size_t gcs__vstr_capacity(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.end_of_storage - v->impl.start;
}

bool gcs__vstr_empty(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start == v->impl.finish;
}

char **gcs__vstr_at(gcs__vstr *v, size_t index) {
    massert_ptr(v);
    return index < gcs__vstr_size(v) ? (v->impl.start + index) : (NULL);
}

char **gcs__vstr_front(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start != v->impl.finish ? (v->impl.start) : NULL;
}

char **gcs__vstr_back(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start != v->impl.finish ? (v->impl.finish - 1) : NULL;
}

void gcs__vstr_resize(gcs__vstr *v, size_t n) {
    char **new_start = NULL;
    size_t size = gcs__vstr_size(v);
    size_t old_capacity = gcs__vstr_capacity(v);

    #ifdef GCS__VSTR_DEEP_COPY

    if (n < size) {
        char **sentinel = v->impl.start + n;
        while (--v->impl.finish != sentinel) {
            free(*(v->impl.finish));
            *(v->impl.finish) = NULL;
        }
    }
    
    #endif

    new_start = realloc(v->impl.start, sizeof *new_start * n);
    massert_realloc(new_start);

    size = n < old_capacity ? n : size;

    v->impl.start = new_start;
    v->impl.finish = v->impl.start + size;
    v->impl.end_of_storage = v->impl.start + n;
}

void gcs__vstr_pushb(gcs__vstr *v, char **straddr) {
    massert_ptr(v);

    if (v->impl.finish == v->impl.end_of_storage) {
        size_t capacity = gcs__vstr_capacity(v);
        gcs__vstr_resize(v, capacity * 2);
    }

    #ifdef GCS__VSTR_DEEP_COPY

    *(v->impl.finish) = malloc(gcs__strlen((*straddr)) + 1);
    massert_malloc(*(v->impl.finish));
    gcs__strcpy(*(v->impl.finish++), (*straddr));

    #else

    *(v->impl.finish++) = (*straddr);

    #endif
}

void gcs__vstr_popb(gcs__vstr *v) {
    #ifdef GCS__VSTR_DEEP_COPY

    if (gcs__vstr_empty(v) == false) {
        free(*(v->impl.finish));
        *(v->impl.finish) = NULL;

        --v->impl.finish;
    }

    #else

    v->impl.finish -= gcs__vstr_empty(v) == false ? 1 : 0;

    #endif
}

void gcs__vstr_clear(gcs__vstr *v) {
    massert_ptr(v);

    #ifdef GCS__VSTR_DEEP_COPY    
    
    while (--v->impl.finish != v->impl.start) {
        free(*(v->impl.finish));
        *(v->impl.finish) = NULL;
    }
    
    free(*(v->impl.start));
    *(v->impl.start) = NULL;

    #else

    v->impl.finish = v->impl.start;

    #endif
}

void gcs__vstr_puts(gcs__vstr *v) {
    char **start = v->impl.start;
    char **pos = start;
    while ((pos = (start++)) != v->impl.finish) {
        LOG(__FILE__, *(pos));
    }
}


#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)

#define GCS__STRING_UTILITIES_DEFINED

char *gcs__strcpy(char *dst, const char *src) {
    char *pos = dst;
    while ((*(pos++) = (*(src++))) != '\0') {
    }
    return dst;
}

size_t gcs__strlen(const char *src) {
    size_t len = 0;
    while ((*(src++)) != '\0') {
        ++len;
    }

    return len;
}

int gcs__strcmp(const char *c1, const char *c2) {
    const uint8_t *pos1 = (const uint8_t *)(c1);
    const uint8_t *pos2 = (const uint8_t *)(c2);

    uint8_t ch1 = 0;
    uint8_t ch2 = 0;

    /**
     *  Iterate from left to right until the first pair
     *  of unlike characters between c1 and c2 are found.
     *
     *  if c1[n] == c2[n], we continue until c1[n] != c2[n],
     *  or either character reaches the null terminator.
     */
    do {
        ch1 = *(pos1++);
        ch2 = *(pos2++);

        if (ch1 == '\0') {
            break;
        }
    } while (ch1 == ch2);

    /**
     *  Delta is taken between the first pair
     *  of unlike characters between c1 and c2.
     */
    return ch1 - ch2;
}

int gcs__strncmp(const char *c1, const char *c2, size_t n) {
    const uint8_t *pos1 = (const uint8_t *)(c1);
    const uint8_t *pos2 = (const uint8_t *)(c2);

    uint8_t ch1 = 0;
    uint8_t ch2 = 0;

    size_t i = 0;

    /**
     *  Iterate from left to right until the first pair
     *  of unlike characters between c1 and c2 are found.
     *
     *  if c1[n] == c2[n], we continue until c1[n] != c2[n],
     *  or either character reaches the null terminator...
     *  or i reaches the character limit, n.
     */
    do {
        ch1 = *(pos1++);
        ch2 = *(pos2++);

        if (i == n) {
            break;
        }

        if (ch1 == '\0') {
            break;
        }
    } while (ch1 == ch2);

    /**
     *  Delta is taken between the first pair
     *  of unlike characters between c1 and c2.
     */
    return ch1 - ch2;
}

char *gcs__strtok(char *src, const char *delim) {
    static char *s = NULL;
    static int pos = 0;

    int i = 0;
    int j = 0;
    int start = pos;

    if (src != NULL) {
        s = src;
    }

    while (s[pos] != '\0') {
        j = 0;

        while (delim[j] != '\0') {
            if (s[pos + 1] == delim[j]) {
                s[pos + 1] = '\0';
                pos += 2;

                if (s[start] != '\0') {
                    return s + start;
                } else {
                    start = pos;
                    break;
                }
            }

            ++j;
        }

        ++pos;
    }

    s[pos] = '\0';

    if (s[start] == '\0') {
        pos = 0;
        return NULL;
    } else {
        return (s + start);
    }
}

void *gcs__memcpy(void *dst, const void *src, size_t width) {
    char *dest = dst;
    const char *source = src;
    size_t i = 0;

    for (i = 0; i < width; i++) {
        *(dest + i) = *(source + i);
    }

    return dst;
}

void *gcs__memmove(void *dst, const void *src, size_t width) {
    char *dest = dst;
    const char *source = src;
    size_t i = 0;

    char *temp = malloc(width);
    massert_malloc(temp);

    for (i = 0; i < width; i++) {
        *(temp + i) = *(source + i);
    }

    for (i = 0; i < width; i++) {
        *(dest + i) = *(temp + i);
    }

    free(temp);
    temp = NULL;

    return dst;
}

void *gcs__memset(void *dst, int ch, size_t n) {
    char *dest = dst;
    size_t i = 0;

    for (i = 0; i < n; i++) {
        *(dest + i) = ch;
    }

    return dst;
}

#else
/* do nothing */
#endif /* !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_) */

/**
 *  Utility function for debugging/error messages
 *
 *  @param[in]      dest        stream destination
 *  @param[in]      level       literals "LOG" or "ERROR"
 *  @param[in]      file        macro __FILE__ is to be used here (by client)
 *  @param[in]      func        macro __func__ is to be used here
 *  @param[in]      line        macro __LINE__ is to be used here
 *  @param[in]      fmt         formatting to be used
 *
 *  @return         character count of buffer (from fprintf)
 */
int ulog(FILE *dest,
         const char *level,
         const char *file,
         const char *func,
         long double line,
         const char *fmt,
         ...) {

    char buffer[MAXIMUM_STACK_BUFFER_SIZE];
    char temp[BUFFER_SIZE];

    const char *color = KNRM;
    const char *blink = "";

    bool found = false;
    bool is_integer = false;
    bool is_currency = *file == '$';

    int j = 0;

    if (found == false) {
        if (streql(level, "[LOG]")) {
            color = KCYN_b;
            found = true;
        }
    }

    if (found == false) {
        if (streql(level, "[ERROR]")) {
            color = KRED_b;
            blink = KBNK;
            found = true;
        }
    }

    sprintf(temp, "%Lf", line);

/* char digit = strchr(temp, '.'); */

#if __STD_VERSION__ >= 199901L
    is_integer = line / (long long int)(line) == 1.000000 || line == 0.00000;
#else
    is_integer = line / (long int)(line) == 1.000000 || line == 0.00000;
#endif

    is_integer = is_currency ? false : is_integer;

    if (ulog_attrs_disable[DATE] == false) {
        char date[1024];
        sprintf(date, "%s%s%s ", KGRY, __DATE__, KNRM);

        j += sprintf(buffer + j, "%s", date);
    }

    if (ulog_attrs_disable[TIME] == false) {
        char time[1024];
        sprintf(time, "%s%s%s ", KGRY, __TIME__, KNRM);

        j += sprintf(buffer + j, "%s", time);
    }

    if (ulog_attrs_disable[LEVEL] == false) {
        char leveltype[1024];
        sprintf(leveltype, "%s%s%s%s ", blink, color, level, KNRM);

        j += sprintf(buffer + j, "%s", leveltype);
    }

    if (ulog_attrs_disable[FILENAME] == false && ulog_attrs_disable[LINE]) {
        char filename[1024];
        sprintf(filename, "[%s] ", file);

        j += sprintf(buffer + j, "%s", filename);
    } else if (ulog_attrs_disable[FILENAME] && ulog_attrs_disable[LINE] == false) {
        char linenumber[1024];

        if (is_integer) {
#if __STD_VERSION__ >= 199901L
            sprintf(linenumber, "[%lli] ", (long long int)(line));
#else
            sprintf(linenumber, "[%li] ", (long int)(line));
#endif
        } else {
            if (is_currency) {
                sprintf(linenumber, "[%0.2Lf] ", line);
            } else {
                sprintf(linenumber, "[%Lf] ", line);
            }
        }

        j += sprintf(buffer + j, "%s", linenumber);
    } else if (ulog_attrs_disable[FILENAME] == false && ulog_attrs_disable[LINE] == false) {
        char fileline[1024];

        if (is_integer) {
#if __STD_VERSION__ >= 199901L
            sprintf(fileline, "[%s:%lli] ", file, (long long int)(line));
#else
            sprintf(fileline, "[%s:%li] ", file, (long int)(line));
#endif
        } else {
            if (is_currency) {
                sprintf(fileline, "[%s%0.2Lf] ", file, line);
            } else {
                sprintf(fileline, "[%s:%Lf] ", file, line);
            }
        }

        j += sprintf(buffer + j, "%s", fileline);
    }

    if (ulog_attrs_disable[FUNCTION] == false) {
        char function[1024];
        sprintf(function, "%s%s", KCYN, func);

        j += sprintf(buffer + j, "%s", function);
    }

    if (ulog_attrs_disable[FUNCTION] == false && ulog_attrs_disable[MESSAGE] == false) {
        j += sprintf(buffer + j, "%s", " ");
    }

    if (ulog_attrs_disable[MESSAGE] == false) {
        char message[4096];
        va_list args;

        va_start(args, fmt);
        vsprintf(message, fmt, args);
        va_end(args);

        j += sprintf(buffer + j, "%s%s%s", KNRM_b, message, KNRM);
    }

    return fprintf(dest, "%s\n", buffer);
}
