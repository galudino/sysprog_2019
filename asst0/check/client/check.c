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

/**
 *  @struct     vstr
 *  @brief      Represents a dynamic array ADT specialized for (char *)
 */
struct gcs__vstr {
    struct gcs__vstr_base {
        char **start;          /**< pointer to calloc'ed block of (char *) */
        char **finish;         /**< pointer to one-past the last element */
        char **end_of_storage; /**< pointer to last block of (char *) */
    } impl;
};

typedef struct gcs__vstr gcs__vstr;

#define GCS__VSTR_INITIAL_SIZE 4

/**
 *  @define GCS__VSTR_DEEP_COPY
 *  @brief  Define this directive to enable deep copies of elements into vstr
 */
/*#define GCS__VSTR_DEEP_COPY*/

/**< gcs__vstr: constructor/destructor functions */
void gcs__vstr_init(gcs__vstr *v, size_t capacity);
void gcs__vstr_deinit(gcs__vstr *v);

/**< gcs__vstr: size/capacity functions */
size_t gcs__vstr_size(gcs__vstr *v);
size_t gcs__vstr_capacity(gcs__vstr *v);
bool gcs__vstr_empty(gcs__vstr *v);

/**< gcs__vstr: element access functions */
char **gcs__vstr_at(gcs__vstr *v, size_t index);
char **gcs__vstr_front(gcs__vstr *v);
char **gcs__vstr_back(gcs__vstr *v);

/**< gcs__vstr: reallocation function */
void gcs__vstr_resize(gcs__vstr *v, size_t n);

/**< gcs__vstr: stack functions, push/pop */
void gcs__vstr_pushb(gcs__vstr *v, char **straddr);
void gcs__vstr_popb(gcs__vstr *v);

/**< gcs__vstr: clear elements */
void gcs__vstr_clear(gcs__vstr *v);

/**< gcs__vstr: print contents to stream (stdout) */
void gcs__vstr_puts(gcs__vstr *v);

typedef int check__operand_t;
typedef int check__operator_t;

enum check__err_type {
    ERR_UNSPECIFIED = -1,
    ERR_OPERAND_UNKNOWN = 0,
    ERR_OPERAND_UNEXPECTED = 1,
    ERR_OPERAND_MISSING = 2,
    ERR_OPERAND_TYPE_MISMATCH = 3,
    ERR_OPERATOR_UNKNOWN = 4,
    ERR_OPERATOR_UNEXPECTED = 5,
    ERR_OPERATOR_MISSING = 6,
    ERR_OPERATOR_TYPE_MISMATCH = 7,
    ERR_TYPE_MISMATCH = 8,
    ERR_IDENTIFIER_UNKNOWN = 9,
    ERR_EXPRESSION_UNENDED = 10,
    ERR_EXPRESSION_INCOMPLETE = 11
};

enum check__operand_type {
    TYPE_OPERAND_UNSPECIFIED = 3,
    TYPE_OPERAND_EMPTY = -2,
    TYPE_OPERAND_UNKNOWN = -1,
    TYPE_OPERAND_ARITHMETIC = 0,
    TYPE_OPERAND_LOGICAL = 1
};

enum check__operator_type {
    TYPE_OPERATOR_UNSPECIFIED = -3,
    TYPE_OPERATOR_EMPTY = -2,
    TYPE_OPERATOR_UNKNOWN = -1,
    TYPE_OPERATOR_BINARY_ARITHMETIC = 0,
    TYPE_OPERATOR_UNARY_LOGICAL = 1,
    TYPE_OPERATOR_BINARY_LOGICAL = 2
};

#define CHECK__OPERANDS_LOGICAL "false", "true"
#define CHECK__OPERANDS_ARITHMETIC                                             \
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"

#define CHECK__OPERATORS_LOGICAL_UNARY "NOT"
#define CHECK__OPERATORS_LOGICAL_BINARY "AND", "OR"
#define CHECK__OPERATORS_ARITHMETIC_BINARY "+", "-", "*", "/"

#define CHECK__OPERANDS_LOGICAL_COUNT 2
#define CHECK__OPERANDS_ARITHMETIC_COUNT 10

#define CHECK__OPERATORS_LOGICAL_UNARY_COUNT 1
#define CHECK__OPERATORS_LOGICAL_BINARY_COUNT 2
#define CHECK__OPERATORS_ARITHMETIC_BINARY_COUNT 4

#define CHECK__USAGE "USAGE: ./check [input string]"

#define CHECK__ERR_DESCRIPTIONS                                                \
    "unknown operand", "unexpected operand", "missing operand",                \
        "operand type mismatch", "unknown operator", "unexpected operator",    \
        "missing operator", "operator type mismatch", "type mismatch",         \
        "unknown identifier", "expression unended", "expression incomplete"

#define CHECK__ERR_SCAN "Scan error"
#define CHECK__ERR_PARSE "Parse error"

#define error_scan_expression_incomplete(expr, expno, err_index)               \
    check__fexpr_err(stderr, CHECK__ERR_SCAN, error_desc[ERR_EXPRESSION_INCOMPLETE], expr, expno, err_index);

#define error_parse_operand_unknown(expr, expno, err_index)                    \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERAND_UNKNOWN], expr, expno, err_index);

#define error_parse_operand_unexpected(expr, expno, err_index)                 \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERAND_UNEXPECTED], expr, expno, err_index);

#define error_parse_operand_missing(expr, expno, err_index)                    \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERAND_MISSING], expr, expno, err_index);

#define error_parse_operand_type_mismatch(expr, expno, err_index)              \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERAND_TYPE_MISMATCH], expr, expno, err_index);

#define error_parse_operator_unknown(expr, expno, err_index)                   \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERATOR_UNKNOWN], expr, expno, err_index);

#define error_parse_operator_unexpected(expr, expno, err_index)                \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERATOR_UNEXPECTED], expr, expno, err_index);

#define error_parse_operator_missing(expr, expno, err_index)                   \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERATOR_MISSING], expr, expno, err_index);

#define error_parse_operator_type_mismatch(expr, expno, err_index)             \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_OPERATOR_TYPE_MISMATCH], expr, expno, err_index);

#define error_parse_type_mismatch(expr, expno, err_index)                      \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_TYPE_MISMATCH], expr, expno, err_index);

#define error_parse_identifier_unknown(expr, expno, err_index)                 \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_IDENTIFIER_UNKNOWN], expr, expno, err_index);

#define error_parse_expression_unended(expr, expno, err_index)                 \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_EXPRESSION_UNENDED], expr, expno, err_index);

#define error_parse_expression_incomplete(expr, expno, err_index)              \
    check__fexpr_err(stderr, CHECK__ERR_PARSE, error_desc[ERR_EXPRESSION_INCOMPLETE], expr, expno, err_index);

/**< check: client functions - logging/errors */
int check__fexpr_log(FILE *dest, size_t ct_expr, size_t ct_logical, size_t ct_arithmetic);
int check__fexpr_err(FILE *dest,
                     const char *err_type,
                     const char *desc,
                     gcs__vstr *vt,
                     size_t ct_expr,
                     int index);
void check__arg_check(int argc, const char *argv[]);

#define check__fexpr_ok(dest) fprintf(dest, "%s%s%s\n\n", KGRN_b, "[OK]", KNRM)

/**< check: client functions - scan */
void check__expr_scan(gcs__vstr *v, char *input_string, const char *delimiter);

/**< check: client functions - parse */
void check__expr_parse(gcs__vstr *v,
                       const char *operands_logical[],
                       const char *operands_arithmetic[],
                       const char *operators_logical_unary[],
                       const char *operators_logical_binary[],
                       const char *operators_arithmetic_binary[],
                       const char *delimiter_expr,
                       const char *delimiter_token,
                       bool delimiter_at_end,
                       const char *error_desc[]);

check__operator_t check__get_operator_type(const char *token,
                                           const char *operators_logical_unary[],
                                           const char *operators_logical_binary[],
                                           const char *operators_arithmetic_binary[],
                                           const char *delimiter_expr,
                                           const char *delimiter_token);

check__operand_t check__get_operand_type(const char *token,
                                         const char *operands_logical[],
                                         const char *operands_arithmetic[],
                                         const char *delimiter_expr,
                                         const char *delimiter_token);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    Argument count (sizeof(argv))
 *  @param[in]  argv    Command line arguments
 *
 *  @return     0 on success, else failure
 */
int main(int argc, const char *argv[]) {
    /**
     *  Defining legal operands and operators.
     *
     *  Legal operands:
     *      Arithmetic: { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }
     *      Logical   : { false, true }
     *  Legal operators:
     *      Arithmetic: { +, -, *, / }
     *      Logical   : { AND, OR, NOT }
     *
     *  All operators EXCEPT for logical NOT are binary operators -
     *  binary operators require a left-hand/right-hand side operand.
     *  (e.g. "2 + 2", "true OR false")
     *
     *  Logical NOT is a unary operator -
     *  it has one operand to its right.
     *  (e.g. "NOT false", "NOT true")
     */
    const char *operands_logical[] = { CHECK__OPERANDS_LOGICAL };
    const char *operands_arithmetic[] = { CHECK__OPERANDS_ARITHMETIC };

    const char *operators_logical_unary[] = { CHECK__OPERATORS_LOGICAL_UNARY };
    const char *operators_logical_binary[] = { CHECK__OPERATORS_LOGICAL_BINARY };
    const char *operators_arithmetic_binary[] = { CHECK__OPERATORS_ARITHMETIC_BINARY };

    const char *error_descriptions[] = { CHECK__ERR_DESCRIPTIONS };

    /**
     *  input_string will (eventually) be assigned to argv[1],
     *  pending a sanity check.
     *
     *  (*delimiter), for this application, will be
     *      ';' for expressions
     *      ' ' for tokens
     */
    const char *input_string = NULL;
    const char *delimiter_expr = ";";
    const char *delimiter_token = " ";
    bool delimiter_at_end = false;

    /**
     *  gcs__vstr is an ADT representing a dynamically-sized array,
     *  specialized for (char *). This application has defined gcs__vstr
     *  to shallow copy its elements - all strings pushed to gcs__vstr
     *  will be substrings of input_string.
     *
     *  gcs__vstr can also support deep copying of its elements,
     *  but it was not found to be necessary for this application.
     *
     *  vec_str is the actual struct, v will be a pointer to vec_str.
     */
    gcs__vstr vec_str = { { NULL, NULL, NULL } };
    gcs__vstr *v = NULL;

    /**
     *  This application will not continue past this point if:
     *      argc != 2
     *          ("./[executable name]", and input_string)
     */
    check__arg_check(argc, argv);

    /**
     *  After the sanity check, input_string is assigned argv[1].
     */
    input_string = argv[1];
    delimiter_at_end = input_string[gcs__strlen(input_string) - 1] == (*delimiter_expr);

    /**
     *  Initialize gcs__vstr's buffer/internal pointers.
     */
    v = &vec_str;
    gcs__vstr_init(v, GCS__VSTR_INITIAL_SIZE);

    /**
     *  Scan for expressions.
     *
     *  gcs__strtok will return pointers to substrings from within input_string,
     *  and gcs__vstr will hold those pointers by shallow copy.
     */
    check__expr_scan(v, (char *)(input_string), delimiter_expr);

    /**
     *  Parse tokens from expressions and analyze.
     *
     *  gcs__strtok will return pointers to substrings from within the current
     *  expression, and will be assessed within check__expr parse.
     *
     *  Any errors/diagnostic messages will appear from this function.
     */
    check__expr_parse(v,
                      operands_logical,
                      operands_arithmetic,
                      operators_logical_unary,
                      operators_logical_binary,
                      operators_arithmetic_binary,
                      delimiter_expr,
                      delimiter_token,
                      delimiter_at_end,
                      error_descriptions);

    /**
     *  Release memory allocated by gcs__vstr.
     *  (if deep copies were enabled, memory for copied strings
     *   would be released by this function as well.)
     */
    gcs__vstr_deinit(v);

    /**
     *  Program ended successfully.
     */
    return EXIT_SUCCESS;
}

void check__expr_scan(gcs__vstr *v, char *input_string, const char *delimiter_expr) {
    char *curr = NULL;

    /**
     *  For gcs__strtok's first reading,
     *  we provide the input_string explicitly.
     *
     *  Each subsequent call will refer to a static int and static (char *)
     *  from within gcs__strtok, that reflects the current position index
     *  in input_string.
     *
     *  The substring returned from gcs__strtok is stored in curr,
     *  and pushed to gcs__vstr by means of shallow copy.
     */
    curr = gcs__strtok(input_string, delimiter_expr);
    gcs__vstr_pushb(v, &curr);

    while ((curr = gcs__strtok(NULL, delimiter_expr))) {
        /**
         *  For the remainder of gcs__strtok's readings,
         *  gcs__strtok's first argument will be NULL --
         *  this tells the function to use the static int variable
         *  referring to the static (char *), reflecting the current
         *  position in input_string.
         *
         *  The substring returned will be stored in curr,
         *  and pushed to gcs__vstr by means of shallow copy.
         */
        gcs__vstr_pushb(v, &curr);
    }

    /**
     *  Pointers to substrings within input_string now exist
     *  in v->impl.start. They are delimited by delimiter_expr.
     */
}

void check__expr_parse(gcs__vstr *v,
                       const char *operands_logical[],
                       const char *operands_arithmetic[],
                       const char *operators_logical_unary[],
                       const char *operators_logical_binary[],
                       const char *operators_arithmetic_binary[],
                       const char *delimiter_expr,
                       const char *delimiter_token,
                       bool delimiter_at_end,
                       const char *error_desc[]) {
    size_t v_size = gcs__vstr_size(v);

    size_t ct_found = 0;
    size_t ct_arithmetic = 0;
    size_t ct_logical = 0;
    size_t expno = 0;
   
    for (expno = 0; expno < v_size; expno++) {
        /**
         *  A vstr of expr substrings (tokens)
         *  will be kept for expression analysis.
         */
        gcs__vstr vec_str_tok = { { NULL, NULL, NULL } };
        gcs__vstr *vt = NULL;

        size_t vt_size = 0;
        size_t tokno = 0;

        char *expr = NULL;
        char *expr_base = NULL;
        char *token = NULL;
        size_t ct_errors = 0;

        char *curr_tok = NULL;
        char *prev_tok = NULL;

        bool first = true;
        bool second = false;
        bool third = false;

        bool operator_expected_unary_logical = true;
        bool operator_expected_binary_logical = false;
        bool operator_expected_binary_arithmetic = false;

        bool operand_expected_logical = true;
        bool operand_expected_arithmetic = true;
        bool endexpr_expected = false;

        bool operator_curr_unary_logical = false;
        bool operator_curr_binary_logical = false;
        bool operator_curr_binary_arithmetic = false;
        bool operator_curr_unknown = false;
        bool operator_curr_empty = false;

        bool operand_curr_logical = false;
        bool operand_curr_arithmetic = false;
        bool operand_curr_unknown = false;

        bool one_leading_token_delimiter = false;

        check__operator_t operator_curr = TYPE_OPERATOR_UNSPECIFIED;
        check__operator_t operator_prev = TYPE_OPERATOR_UNSPECIFIED;

        check__operator_t operand_curr = TYPE_OPERAND_UNSPECIFIED;
        check__operator_t operand_prev = TYPE_OPERAND_UNSPECIFIED;

        /**
         *  Initializing vstr's buffer and assigning a pointer
         *  as a "handle".
         */
        gcs__vstr_init(&vec_str_tok, GCS__VSTR_INITIAL_SIZE);
        vt = &vec_str_tok;

        /**
         *  Retrieve the current expression at index i
         *  in v. expr will be split into substrings
         *  for vstr vt - vt will consist of the component
         *  tokens that make up expr.
         */
        expr = *(gcs__vstr_at(v, expno));

        /**
         *  Since expr's base address will be mutated
         *  as it is being split into substrings (tokens),
         *  we must retain a copy of expr so that it can
         *  be printed during error messages.
         */
        expr_base = malloc(gcs__strlen(expr) + 1);
        massert_malloc(expr_base);
        gcs__strcpy(expr_base, expr);

        ulog(stdout, "[EXP]", __FILE__, "expression", __LINE__, "%lu:\t%s\n", expno, expr);

        /**
         *  Just as in check__expr_scan,
         *  the first reading of expr initializes gcs__strtok.
         *  The substring returned will be assigned to token.
         *
         *  token will be pushed to vt by means of shallow copy.
         */
        token = gcs__strtok(expr, delimiter_token);
        gcs__vstr_pushb(vt, &token);

        /**
         *  Each subsequent call to gcs__strtok will refer
         *  to a static int and static (char *) that keeps
         *  the current position of expr.
         *
         *  token will be pushed to vt by means of shallow copy.
         */
        while ((token = gcs__strtok(NULL, delimiter_token))) {
            gcs__vstr_pushb(vt, &token);
        }

        vt_size = gcs__vstr_size(vt);

        ulog(stdout, "[TOK]", __FILE__, "tokens beg", __LINE__, "------");
        gcs__vstr_puts(vt);
        ulog(stdout, "[TOK]", __FILE__, "tokens end", __LINE__, "------\n");

        printf("current expr: %s\n", expr_base);

        for (tokno = 0; tokno < vt_size; tokno++) {
            size_t index = 0;
            size_t tlen = 0;

            /**
             *  curr is the current token
             *  for the current expression
             *  (as described by expr_display)
             *
             *  Use curr for token analysis.
             */
            curr_tok = *(gcs__vstr_at(vt, tokno));
            prev_tok = tokno > 0 ? *(gcs__vstr_at(vt, tokno - 1)) : NULL;
            tlen = gcs__strlen(curr_tok);

            /**
             *  This will advance curr_tok one character if
             *  its first char is a delimiter token.
             *
             *  For all expressions except for the first,
             *  the starting token for each expression will have
             *  one character consisting of a leading delimiter.
             *  (this is good -- input strings with multiple expressions
             *   will behave this way.)
             *
             *  example scenario, in vstr v:
             *  expr 0: "3 + 2"
             *  expr 1: " NOT true"
             *
             *  "3 + 2; NOT true"
             *         ^ this begins the first token of expr 1.
             *           " NOT true" is expr 1, tok 0 of expr 1 is " NOT".
             *           " NOT" will be advanced to "NOT" for comparisons sake.
             *           If tok 0 had "NOT" instead, it would yield a
             *              "parse error: unexpected operand" error.
             */
            if (expno == 0 && curr_tok[0] == (*delimiter_token)) {
                /**
                 *  If we are on the first expression of the input string,
                 *  and the first character of the
                 */
                ++ct_errors;
                error_parse_identifier_unknown(vt, expno, 0);
                curr_tok += 1;
            } else if (expno > 0 && tokno == 0 && curr_tok[0] != (*delimiter_token)) {
                ++ct_errors;
                error_parse_operand_unexpected(vt, expno, 0);
            }

            printf("curr: %s\nprev: %s\n", curr_tok, prev_tok);

            operator_curr = check__get_operator_type(curr_tok,
                                                     operators_logical_unary,
                                                     operators_logical_binary,
                                                     operators_arithmetic_binary,
                                                     delimiter_expr,
                                                     delimiter_token);

            if (operator_curr != TYPE_OPERATOR_UNARY_LOGICAL) {
                operand_curr = check__get_operand_type(curr_tok,
                                                       operands_logical,
                                                       operands_arithmetic,
                                                       delimiter_expr,
                                                       delimiter_token);
            }

            if (prev_tok) {
                operator_prev = check__get_operator_type(prev_tok,
                                                         operators_logical_unary,
                                                         operators_logical_binary,
                                                         operators_arithmetic_binary,
                                                         delimiter_expr,
                                                         delimiter_token);

                if (operator_prev != TYPE_OPERATOR_UNARY_LOGICAL) {
                    operand_prev = check__get_operand_type(prev_tok,
                                                           operands_logical,
                                                           operands_arithmetic,
                                                           delimiter_expr,
                                                           delimiter_token);
                }
            }

            if (operator_expected_unary_logical) {
                if (operator_curr == TYPE_OPERATOR_UNARY_LOGICAL) {
                    operator_expected_unary_logical = false;
                    operator_expected_binary_logical = false;
                    operator_expected_binary_arithmetic = false;
                    operand_expected_arithmetic = false;

                    operand_expected_logical = true;
                    endexpr_expected = true;
                } else {
                }
            } else if (operator_expected_binary_logical) {
                if (operator_curr == TYPE_OPERATOR_BINARY_LOGICAL) {
                    operator_expected_binary_logical = false;
                    operator_expected_unary_logical = false;
                    operator_expected_binary_arithmetic = false;
                    operand_expected_arithmetic = false;

                    operand_expected_logical = true;
                } else {
                }
            } else if (operator_expected_binary_arithmetic) {
                if (operator_curr == TYPE_OPERATOR_BINARY_ARITHMETIC) {
                    operator_expected_binary_arithmetic = false;
                    operator_expected_unary_logical = false;
                    operator_expected_binary_logical = false;
                    operand_expected_logical = false;

                    operand_expected_arithmetic = true;
                } else {
                }
            }

            if (operand_expected_logical) {
                if (operand_curr == TYPE_OPERAND_LOGICAL) {
                    operand_expected_logical = false;
                    operand_expected_arithmetic = false;
                } else {
                }
            } else if (operand_expected_arithmetic) {
                if (operand_curr == TYPE_OPERAND_ARITHMETIC) {
                    operand_expected_arithmetic = false;
                } else {
                }
            }

            if (tokno == vt_size - 1) {
                if (endexpr_expected) {

                } else {
                }
            }

            /*
            operator_prev = operator_curr;
            operand_prev = operand_curr;
            */

            printf("\n");
        }

        if (delimiter_at_end && tokno == vt_size) {
            error_parse_expression_incomplete(vt, expno, 0);
        }

        /**
         *  Release the copied expression string
         *  used for printing error messages.
         */
        free(expr_base);
        expr_base = NULL;

        /**
         *  Release the vstr of (char *) consisting
         *  of expr substrings.
         */
        gcs__vstr_deinit(vt);

        /**
         *  Displays:
         *  Found [x] expressions: [y] logical and [z] arithmetic.
         */
        check__fexpr_log(stdout, ++ct_found, ct_logical, ct_arithmetic);

        if (ct_errors == 0) {
            /**
             *  If and only if no errors are found,
             *  will "[OK]" appear after check__fexpr_log.
             */
            check__fexpr_ok(stdout);
        }
    }
}

check__operator_t check__get_operator_type(const char *token,
                                           const char *operators_logical_unary[],
                                           const char *operators_logical_binary[],
                                           const char *operators_arithmetic_binary[],
                                           const char *delimiter_expr,
                                           const char *delimiter_token) {
    size_t i = 0;

    if (gcs__strlen(token) == 0) {
        return TYPE_OPERATOR_EMPTY;
    }

    for (i = 0; i < CHECK__OPERATORS_LOGICAL_UNARY_COUNT; i++) {
        if (gcs__strcmp(operators_logical_unary[i], token) == 0) {
            return TYPE_OPERATOR_UNARY_LOGICAL;
        }
    }

    for (i = 0; i < CHECK__OPERATORS_LOGICAL_BINARY_COUNT; i++) {
        if (gcs__strcmp(operators_logical_binary[i], token) == 0) {
            return TYPE_OPERATOR_BINARY_LOGICAL;
        }
    }

    for (i = 0; i < CHECK__OPERATORS_ARITHMETIC_BINARY_COUNT; i++) {
        if (gcs__strcmp(operators_arithmetic_binary[i], token) == 0) {
            return TYPE_OPERATOR_BINARY_ARITHMETIC;
        }
    }

    return TYPE_OPERATOR_UNKNOWN;
}

check__operand_t check__get_operand_type(const char *token,
                                         const char *operands_logical[],
                                         const char *operands_arithmetic[],
                                         const char *delimiter_expr,
                                         const char *delimiter_token) {
    size_t i = 0;

    if (gcs__strlen(token) == 0) {
        return TYPE_OPERATOR_EMPTY;
    }

    for (i = 0; i < CHECK__OPERANDS_LOGICAL_COUNT; i++) {
        if (gcs__strcmp(operands_logical[i], token) == 0) {
            return TYPE_OPERAND_LOGICAL;
        }
    }

    for (i = 0; i < CHECK__OPERANDS_ARITHMETIC_COUNT; i++) {
        if (gcs__strcmp(operands_arithmetic[i], token) == 0) {
            return TYPE_OPERAND_ARITHMETIC;
        }
    }

    return TYPE_OPERAND_UNKNOWN;
}

int check__fexpr_log(FILE *dest, size_t ct_expr, size_t ct_logical, size_t ct_arithmetic) {
    char buffer[BUFFER_SIZE];
    int j = 0;

    j = sprintf(buffer + j,
                "Found %s%lu%s expressions: %s%lu%s logical and %s%lu%s "
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
                     gcs__vstr *vt,
                     size_t ct_expr,
                     int index) {
    char buffer[BUFFER_SIZE];
    char spaces[BUFFER_SIZE];
    char expr_fragmt[BUFFER_SIZE];
    int i = 0;
    int j = 0;
    int k = 0;
    size_t vt_size = gcs__vstr_size(vt);

    gcs__strcpy(spaces, "\t");

    if (index > -1) {
        for (i = 1; i <= index + 1; i++) {
            *(spaces + i) = ' ';
        }
    }

    for (i = 0; i < vt_size; i++) {
        char *t = *(gcs__vstr_at(vt, i));
        /*
        t += t[0] == ' ' ? 1 : 0;
        */

        k += sprintf(expr_fragmt + k, "%s", t);

        if (i < vt_size - 1) {
            k += sprintf(expr_fragmt + k, "%s", " ");
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

    /* TODO: implementation of '^', appears under problematic character.
    if (index > -1) {
        j += sprintf(buffer + j, "%s%s", spaces, KYEL_b "^" KNRM);
    }
    */

    j += fprintf(dest, "%s\n", buffer);

    return j;
}

void check__arg_check(int argc, const char *argv[]) {
    size_t i = 0;
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

/**
 *  @brief  "Constructor" function, initializes vstr
 *
 *  @param[in]  v           pointer to vstr
 *  @param[in]  capacity    capacity desired for vstr
 */
void gcs__vstr_init(gcs__vstr *v, size_t capacity) {
    char **start = NULL;

    if (capacity <= 0) {
        WARNING(__FILE__,
                "Provided input capacity was less than or equal to "
                "0. Will default to capacity of 1.");
        capacity = 1;
    }

    start = calloc(capacity, sizeof *v->impl.start);
    massert_calloc(start);

    v->impl.start = start;
    v->impl.finish = v->impl.start;
    v->impl.end_of_storage = v->impl.start + capacity;
}

/**
 *  @brief  "Destructor" function, deinitializes vector
 *
 *  @param[in]  v   pointer to vstr
 *
 *  If deep copies are enabled, any memory allocated by vstr
 *  will be released -- otherwise, the client will be responsible
 *  for any memory to be freed per element.
 */
void gcs__vstr_deinit(gcs__vstr *v) {
    gcs__vstr_clear(v);

    free(v->impl.start);
    v->impl.start = NULL;
    v->impl.finish = NULL;
    v->impl.end_of_storage = NULL;
}

/**
 *  @brief  Returns the logical length of vstr
 *
 *  @param[in]  v   pointer to vstr
 *
 *  @return     logical length of vstr
 */
size_t gcs__vstr_size(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.finish - v->impl.start;
}

/**
 *  @brief  Returns the amount of blocks allocated for vstr's buffer
 *
 *  @param[in]  v   pointer to vstr
 *
 *  @return     capacity of vstr
 */
size_t gcs__vstr_capacity(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.end_of_storage - v->impl.start;
}

/**
 *  @brief  Determines if vstr is empty, or not
 *
 *  @param[in]  v   pointer to vstr
 *
 *  @return     true if v->impl.start == v->impl.finish, false otherwise
 */
bool gcs__vstr_empty(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start == v->impl.finish;
}

/**
 *  @brief  Retrieves the address of an element from vstr at index
 *
 *  @param[in]  v       pointer to vstr
 *  @param[in]  index   index of desired element
 *
 *  @return     address of element at index (dereference for front element)
 */
char **gcs__vstr_at(gcs__vstr *v, size_t index) {
    massert_ptr(v);
    return index < gcs__vstr_size(v) ? (v->impl.start + index) : (NULL);
}

/**
 *  @brief  Retrieves the address of vstr's front element
 *
 *  @param[in]  v       pointer to vstr
 *
 *  @return     v->impl.start (dereference for front element)
 */
char **gcs__vstr_front(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start != v->impl.finish ? (v->impl.start) : NULL;
}

/**
 *  @brief  Retrieves the address of vstr's back element
 *
 *  @param[in]  v       pointer to vstr
 *
 *  @return     v->impl.start (dereference for back element)
 */
char **gcs__vstr_back(gcs__vstr *v) {
    massert_ptr(v);
    return v->impl.start != v->impl.finish ? (v->impl.finish - 1) : NULL;
}

/**
 *  @brief  Resizes vstr to size n
 *
 *  @param[in]  v   pointer to vstr
 *  @param[in]  n   desired size for vstr
 *
 *  If n is less than the current logical length (vstr_size(v)),
 *  v's buffer will be truncated (excess elements are destroyed),
 *  otherwise, v's capacity will be extended.
 */
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

/**
 *  @brief  Appends an element to the rear of vstr
 *
 *  @param[in]  v       pointer to vstr
 *  @param[in]  val     element to be copied
 *
 *  If deep copies are enabled using the directive GCS__VSTR_DEEP_COPY,
 *  (*straddr) will be duplicated into vstr's buffer and will be
 *  freed during vstr_popb/vstr_clear/vstr_deinit --
 *  otherwise, a shallow copy by assignment is made.
 */
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

/**
 *  @brief  Removes element at the rear of the vector
 *
 *  @param[in]  v   pointer to vstr
 *
 *  If deep copies are enabled using the directive GCS__VSTR_DEEP_COPY,
 *  any memory at the rear of vstr's buffer will be freed --
 *  otherwise, v->impl.finish is decremented and will be overwritten
 *  when a new address takes its place.
 */
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

/**
 *  @brief  Destroys elements from within v, but vstr's buffer will persist.
 *
 *  @param[in]  v   pointer to vstr
 */
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

/**
 *  @brief  Print the contents of vstr to stdout
 */
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
