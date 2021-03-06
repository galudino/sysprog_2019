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
#include <string.h>

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#else
#define false 0
#define true 1
typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef long int ptrdiff_t;
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

#define massert_container(PTR)                                                 \
    ;                                                                          \
    massert(PTR,                                                               \
            "['" #PTR "' was found to be NULL - '" #PTR                        \
            "' must be assigned to the return value of a container "           \
            "initializer function prior to use.]");

#define massert_pfunc(PFUNC)                                                   \
    ;                                                                          \
    massert(PFUNC,                                                             \
            "['" #PFUNC "' was found to be NULL - '" #PFUNC                    \
            "' must be assigned to a function with a matching prototype.]");

/**< pointer utility */
ptrdiff_t ptr_distance(const void *beg, const void *end, size_t width);

#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
/**< gcs: string utilities */
char *gcs__strcpy(char *dst, const char *src);
char *gcs__strncpy(char *dst, const char *src, size_t n);
char *gcs__strdup(const char *src);
char *gcs__strndup(const char *src, size_t n);
size_t gcs__strlen(const char *src);
int gcs__strcmp(const char *c1, const char *c2);
int gcs__strncmp(const char *c1, const char *c2, size_t n);

char *gcs__strtok(char *src, const char *delim);
char *gcs__strtok_r(char *src, const char *delim, char **save_ptr);

void *gcs__memcpy(void *dst, const void *src, size_t width);
void *gcs__memmove(void *dst, const void *src, size_t width);
void *gcs__memset(void *dst, int ch, size_t n);
int gcs__memcmp(const void *s1, const void *s2, size_t n);
#else
#define gcs__strcpy strcpy
#define gcs__memmove strncpy
#define gcs__strdup strdup
#define gcs__strndup strndup
#define gcs__strlen strlen
#define gcs__strcmp strcmp
#define gcs__strncmp strncmp
#define gcs__strtok strtok
#define gcs__strtok_r strtok_r
#define gcs__memcpy memcpy
#define gcs__memmove memmove
#define gcs__memset memset
#define gcs__memcmp memcmp
#endif /* !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_) */

#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
#define streql(s1, s2) gcs__strcmp(s1, s2) == 0
#define strneql(s1, s2, n) gcs__strncmp(s1, s2, n) == 0
#else
#define streql(s1, s2) strcmp(s1, s2) == 0
#define strneql(s1, s2, n) strncmp(s1, s2, n) == 0
#endif

#if __linux__ && !__POSIX__
# if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
#  define strdup(src) gcs__strcpy(malloc(gcs__strlen(src) + 1), src)
# else
#  define strdup(src) strcpy(malloc(strlen(src) + 1), src)
# endif
#endif

/**< (char *) will be address as str by vector_str. */
typedef char *str;

/**
 *  @struct     typetable
 *  @brief      a virtual function table that determines the behavior of
 *              a container ADT when acting with or upon its elements
 */
struct typetable {
    size_t width; /**< sizeof(T) */

    void *(*copy)(void *, const void *); /**< intended for deep copies */
    void (*dtor)(void *); /**< for release of dynamically allocated memory */
    void (*swap)(void *,
                 void *); /**< for moving dynamically allocated memory */

    int (*compare)(const void *, const void *); /**< sorting/searching */
    void (*print)(const void *, FILE *dest);    /**< output to stream */
};

/**< str typetable functions */
void *str_copy(void *dst, const void *src);
void str_dtor(void *arg);
void str_swap(void *arg1, void *arg2);
int str_compare(const void *c1, const void *c2);
void str_print(const void *arg, FILE *dst);

/**< forward-declared typetable instances */
struct typetable ttbl_str;
struct typetable *_str_;

/**
 *  @struct     vector
 *  @brief      Represents a dynamic array ADT
 *
 *  Note that struct vector and struct vector_base are
 *  opaque -- their fields cannot be accessed directly,
 *  nor can instances of struct vector/struct vector_base
 *  be created on the stack. This is done to enforce encapsulation.
 */
struct vector {
    /**
     *  @struct     vector_base
     *  @brief      Decouples vector-related fields from typetable
     */
    struct vector_base {
        void *start;          /**< address of array base (first element) */
        void *finish;         /**< address reserved for next rear element */
        void *end_of_storage; /**< addresses last allocated block of storage */
    } impl;

    struct typetable *ttbl; /**< data width, cpy, dtor, swap, compare, print */
};

typedef struct vector vector;

/**< vector: allocate and construct */
vector *v_new(struct typetable *ttbl);
vector *v_newr(struct typetable *ttbl, size_t n);

/**< vector: destruct and deallocate */
void v_delete(vector **v);

/**< vector: length functions */
size_t v_size(vector *v);

/**< vector: resize functions */
void v_resize(vector *v, size_t n);

/**< vector: capacity based functions */
size_t v_capacity(vector *v);
bool v_empty(vector *v);

/**< vector: reserve/shrinking functions */
void v_reserve(vector *v, size_t n);
void v_shrink_to_fit(vector *v);

/**< vector: element access functions */
void *v_at(vector *v, size_t n);
void *v_front(vector *v);
void *v_back(vector *v);
void *v_data(vector *v);

/**< vector: modifiers - push/pop */
void v_pushb(vector *v, const void *valaddr);
void v_popb(vector *v);

/**< vector: modifiers - clear container */
void v_clear(vector *v);

/**< vector: custom print functions - output to FILE stream */
void v_puts(vector *v);

void v_putsf(vector *v, const char *before, const char *after,
             const char *postelem, const char *empty, size_t breaklim);

void v_fputs(vector *v, FILE *dest);

void v_fputsf(vector *v, FILE *dest, const char *before, const char *after,
              const char *postelem, const char *empty, size_t breaklim);

/**
 *  @struct     vector_str
 *  @brief      Represents a type-safe dynamic array ADT
 *
 *  Note that struct vector_str and struct vector_base_str are
 *  opaque -- their fields cannot be accessed directly,
 *  nor can instances of struct vector_str/struct vector_base_str
 *  be created on the stack. This is done to enforce encapsulation.
 */
struct vector_str {
    struct vector_base_str {
        str *start;          /**< address of array base (first element) */
        str *finish;         /**< address reserved for next rear element */
        str *end_of_storage; /**< addresses last allocated block of storage */
    } impl;

    struct typetable *ttbl; /*<< data width, cpy, dtor, swap, compare, print */
};

/**< struct vector_str: dynamic array ADT typed for (char *) */
typedef struct vector_str vector_str;

/**< vector_str: constructors */
vector_str *vnew_str(void);
vector_str *vnewr_str(size_t n);

/**< vector_str: destructor */
void vdelete_str(vector_str **v);

/**< vector_str: length functions */
size_t vsize_str(vector_str *v);

/**< vector_str: resize functions */
void vresize_str(vector_str *v, size_t n);

/**< vector_str: capacity based functions */
size_t vcapacity_str(vector_str *v);
bool vempty_str(vector_str *v);

/**< vector_str: shrinking functions */
void vshrinktofit_str(vector_str *v);

/**< vector_str: element access functions */
str *vat_str(vector_str *v, size_t n);
str *vfront_str(vector_str *v);
str *vback_str(vector_str *v);
str **vdata_str(vector_str *v);

/**< vector_str: modifiers - push/pop */
void vpushb_str(vector_str *v, str val);
void vpushbptr_str(vector_str *v, str *valaddr);
void vpopb_str(vector_str *v);

/**< vector_str: modifiers - clear container */
void vclear_str(vector_str *v);

/**< vector_str: custom print functions - output to FILE stream */
void vputs_str(vector_str *v);
void vputsf_str(vector_str *v,
                const char *before,
                const char *after,
                const char *postelem,
                const char *empty,
                size_t breaklim);
void vfputs_str(vector_str *v, FILE *dest);
void vfputsf_str(vector_str *v,
                 FILE *dest,
                 const char *before,
                 const char *after,
                 const char *postelem,
                 const char *empty,
                 size_t breaklim);

/**< check: client functions - logging/errors */
void check__arg_check(int argc, const char *argv[]);
int check__fexpr_log(FILE *dest, uint32_t ct_expr, uint32_t ct_logical, uint32_t ct_arithmetic);
int check__fexpr_err(FILE *dest,
                     const char *err_type,
                     const char *desc,
                     const char *expr_fragmt,
                     uint32_t ct_expr,
                     int index);

#define check__fexpr_ok(dest) fprintf(dest, "%s%s%s", KGRN_b, "OK.", KNRM)
#define check__expr_ok() fexpr_ok(stdout)

#define check__expr_log(ct_expr, ct_logical, ct_arithmetic)                    \
    fexpr_log(stdout, ct_expr, ct_logical, ct_arithmetic)

#define check__expr_err(err_type, desc, expr_fragmt, ct_expr, index)           \
    fexpr_err(stderr, err_type, desc, expr_fragmt, ct_expr, index)

/**< check: client functions - tokenize */
void check__expr_populate(vector_str *v, char *input_string, const char *delimiter);
void check__expr_populate_v(vector *v, char *input_string, const char *delimiter);
bool check__expr_assess(const char *expr,
                        const char *operands[],
                        const char *operators[],
                        const char *delimiter);

#define CHECK__OPERANDS                                                        \
    "false", "true", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"

#define CHECK__OPERATORS "+", "-", "*", "/", "AND", "OR", "NOT"

#define CHECK__USAGE "USAGE: ./check [input string]"

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
     *  Define the legal operands and operators
     */
    const char *operands[] = { CHECK__OPERANDS };
    const char *operators[] = { CHECK__OPERATORS };
    const char *delimiter = NULL;

    char *expr = NULL;
    char *input_string = NULL;

    size_t i = 0; /**< loop control variable */

    vector_str *v = NULL;
    /*vector *v = NULL;*/
    size_t vsize = 0; 

    /**
     *  Check argument count (argc)
     *  and argument value (argv).
     *
     *  If argc != 2, abort.
     *  If gcs__strlen(argv[1]) == 0, abort.
     */
    check__arg_check(argc, argv);

    /**
     *  Create and construct an instance of vector_str.
     *  Expression strings are stored here.
     *
     *  vector_str v will be sent to
     *  check__expr_populate, along with argv[1],
     *  the input string, and
     */
    v = vnew_str();
    /*v = v_new(_str_);*/


    delimiter = ";";
    input_string = gcs__strdup(argv[1]);
    check__expr_populate(v, input_string, delimiter);
    /*check__expr_populate_v(v, input_string, delimiter);*/

    free(input_string);
    input_string = NULL;

    /**
     *  Retrieve the size of vector_str v,
     *  this denotes the amount of tokens found.
     */
    vsize = vsize_str(v);
    /*vsize = v_size(v);*/

    for (i = 0; i < vsize; i++) {
        size_t len = 0;
        size_t j = 0;
        char *expr = (*(vat_str(v, i)));
        /*char *expr = *(char **)v_at(v, i);*/

        LOG(__FILE__, expr);
        /*check__expr_assess(expr, operands, operators, " ");*/
    }

    
    vputs_str(v);
    vdelete_str(&v);
    
    /*v_delete(&v);*/


    /*
    vector *v = v_new(_str_);
    char *str = NULL;

    _str_->copy = NULL;

    str = strdup("string1");
    v_pushb(v, &str);

    str = strdup("string2");
    v_pushb(v, &str);

    v_puts(v);

    v_delete(&v);
    */

    /*
    vector_str *v = vnew_str();
    char *str = NULL;

    _str_->copy = NULL;

    str = strdup("string1");
    vpushbptr_str(v, &str);

    str = strdup("string2");
    vpushbptr_str(v, &str);

    vputs_str(v);
    vdelete_str(&v);
    */

    /*
    vector_str *v = vnew_str();

    vpushb_str(v, "string1");
    vpushb_str(v, "string2");

    vputs_str(v);
    vdelete_str(&v);
    */
    
    return EXIT_SUCCESS;
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

void check__expr_populate(vector_str *v, char *input_string, const char *delimiter) {
    char *pos = input_string;
    size_t len = gcs__strlen(pos);

    /**
     *  Failsafe,
     *  in case the graders end up having
     *  argv[1] come from a text file.
     *
     *  If first non-whitespace char is a '"',
     *  advance pos one character.
     *
     *  (We do not want the introductory '"' from the input string
     *   to be included in the first tokenized expression)
     */
    pos += (*pos) == '"' ? 1 : 0;

    /**
     *  Failsafe,
     *  in case the graders end up having
     *  argv[1] come from a text file.
     *
     *  If the last char in pos is a '"', null terminate pos.
     *  Otherwise, leave it alone (it will be assigned whatever it is now)
     *
     *  (We do not want the '"' char prior to the null terminator
     *   to be included in the last tokenized expression)
     */
    *(pos + (len - 2)) = *(pos + (len - 2)) == '\"' ? '\0' : *(pos + (len - 2));

    /**
     *  Initialize the tokenizer by invoking gcs__strtok
     *  with the position string, pos, that points to the
     *  current character within input_string.
     *
     *  Capture the token in expr and append it
     *  to the vector_str instance.
     *
     *  (Since gcs__strtok mutates pos each time
     *   it is invoked, we must deep copy expr into
     *   the vector.)
     */
    pos = gcs__strtok(pos, delimiter);
    vpushbptr_str(v, &pos);

    /**
     *  While valid non-null tokens can be parsed,
     *  assign them to expr and push expr
     *  to the vector.
     */
    while ((pos = gcs__strtok(NULL, delimiter)) != NULL) {
        vpushbptr_str(v, &pos);
    }
}

void check__expr_populate_v(vector *v, char *input_string, const char *delimiter) {
    char *expr = NULL;
    char *pos = input_string;
    size_t len = gcs__strlen(pos);

    /**
     *  Failsafe,
     *  in case the graders end up having
     *  argv[1] come from a text file.
     *
     *  If first non-whitespace char is a '"',
     *  advance pos one character.
     *
     *  (We do not want the introductory '"' from the input string
     *   to be included in the first tokenized expression)
     */
    pos += (*pos) == '"' ? 1 : 0;

    /**
     *  Failsafe,
     *  in case the graders end up having
     *  argv[1] come from a text file.
     *
     *  If the last char in pos is a '"', null terminate pos.
     *  Otherwise, leave it alone (it will be assigned whatever it is now)
     *
     *  (We do not want the '"' char prior to the null terminator
     *   to be included in the last tokenized expression)
     */
    *(pos + (len - 2)) = *(pos + (len - 2)) == '\"' ? '\0' : *(pos + (len - 2));

    /**
     *  Initialize the tokenizer by invoking gcs__strtok
     *  with the position string, pos, that points to the
     *  current character within input_string.
     *
     *  Capture the token in expr and append it
     *  to the vector_str instance.
     *
     *  (Since gcs__strtok mutates pos each time
     *   it is invoked, we must deep copy expr into
     *   the vector.)
     */
    pos = gcs__strtok(pos, delimiter);
    expr = gcs__strdup(pos);
    v_pushb(v, &expr);

    /**
     *  While valid non-null tokens can be parsed,
     *  assign them to expr and push expr
     *  to the vector.
     */
    while ((pos = gcs__strtok(NULL, delimiter)) != NULL) {
        expr = gcs__strdup(pos);
        v_pushb(v, &expr);
    }

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

void *str_copy(void *arg, const void *other) {
    char **target = (char **)(arg);
    char **source = (char **)(other);

    (*target) = malloc(gcs__strlen((*source) + 1));
    massert_malloc((*target));
    gcs__strcpy((*target), (*source));

    return (*target);
}

void str_dtor(void *arg) {
    /**
     *  Actual argument upon invocation should be a (char **);
     *  if a string str is a (char *), then:
     *
     *          cstr_delete(&str);
     *
     *  is the call made, clientside.
     */
    char **target = (char **)(arg);

    free((*target));
    (*target) = NULL;
}

void str_swap(void *arg, void *other) {
    char **dest = (char **)(arg);
    char **source = (char **)(other);

    if ((*dest)) {
        char *temp = gcs__strdup((*dest));
        (*dest) = (*source);
        (*source) = temp;
    } else {
        (*dest) = (*source);
        (*source) = NULL;
    }
}

int str_compare(const void *c1, const void *c2) {
    int result = -1;

    char *first = *((char **)(c1));
    char *second = *((char **)(c2));

#if __STD_VERSION__ >= 199901L
    char cfirst[gcs__strlen(first) + 1];
    char csecond[gcs__strlen(second) + 1];
#else
    char *cfirst = malloc(gcs__strlen(first) + 1);
    char *csecond = malloc(gcs__strlen(second) + 1);
    massert_malloc(cfirst);
    massert_malloc(second);
#endif /* __STDC_VERSION__ >= 199901L */

    gcs__strcpy(cfirst, first);
    gcs__strcpy(csecond, second);

    /* Omitted for check.c
    str_trim(cfirst, NULL);
    str_trim(csecond, NULL);
    */

    result = gcs__strcmp(cfirst, csecond);

#if __STD_VERSION__ >= 199901L
    free(cfirst);
    cfirst = NULL;

    free(csecond);
    csecond = NULL;
#endif /* __STDC_VERSION__ >= 199901L */

    return result;
}

void str_print(const void *arg, FILE *dest) {
    fprintf(dest, "%s", *(char **)arg);
}

struct typetable ttbl_str = { sizeof(str), str_copy,    str_dtor,
                              str_swap,    str_compare, str_print };

struct typetable *_str_ = &ttbl_str;

static vector *v_allocate(void);
static void v_init(vector *v, struct typetable *ttbl, size_t capacity);
static void v_deinit(vector *v);
static void v_swap_addr(vector *v, void *first, void *second);

/**
 *  @brief  Allocates, constructs, and returns a pointer to vector,
 *          size VECTOR_DEFAULT_CAPACITY (16)
 *
 *  @param[in]  ttbl    pointer to struct typetable for
 *                      width/copy/dtor/swap/compare/print
 *
 *  @return     pointer to vector
 */
vector *v_new(struct typetable *ttbl) {
    vector *v = v_allocate();                   /* allocate */
    v_init(v, ttbl, 16);   /* construct */
    return v;                                   /* return */
}

/**
 *  @brief  Allocates, constructs, and returns a pointer to vector,
 *          size n
 *
 *  @param[in]  ttbl   pointer to struct typetable for
 *                     width/copy/dtor/swap/compare/print
 *
 *  @return     pointer to vector
 */
vector *v_newr(struct typetable *ttbl, size_t n) {
    vector *v = v_allocate();                   /* allocate */
    v_init(v, ttbl, n);                         /* construct */
    return v;                                   /* return */
}

/**
 *  @brief  Calls v_deinit (vector's destructor) and deallocates the pointer v.
 *
 *  @param[out] v   Address of a pointer to vector
 *
 *  Every call to any of vector's "new" functions should be accompanied
 *  by a call to v_delete when a pointer to vector is no longer needed.
 *
 *  If the vector has a ttbl with a dtor function defined,
 *  the elements within the vector will be destroyed using the dtor function.
 *
 *  Note that if the elements from within v are dynamically allocated,
 *  and/or the elements have dynamically allocated fields --
 *  and there is no dtor function defined, memory management
 *  will become the client's responsibility.
 */
void v_delete(vector **v) {
    massert_container((*v));

    /**
     *  Deinitialization involves releasing all dynamically allocated
     *  memory at each field (if dtor function is defined in ttbl,
     *  and is written to release such memory) --
     *  after that step (or if there is no memory to free within the elements),
     *  (*v)->impl.start has its memory freed, and the remainder of (*v)'s fields
     *  are set NULL.
     */
    v_deinit((*v));

    /* finally, the memory (*v) points to will be freed. */
    free((*v));
    (*v) = NULL;
}

/**
 *  @brief  Returns the logical length of v
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     logical length of v
 */
size_t v_size(vector *v) {
    massert_container(v);
    /* effectively v->impl.finish - v->impl.start */
    return ptr_distance(v->impl.start, v->impl.finish, v->ttbl->width);
}

/**
 *  @brief  Resizes the vector to size n
 *
 *  @param[in]  v   pointer to vector
 *  @param[in]  n   desired size for v
 *
 *  If n is less than the current logical length (v_size(v)),
 *  v will be truncated (excess elements are destroyed),
 *  otherwise, v's capacity will be extended.
 */
void v_resize(vector *v, size_t n) {
    size_t old_size = 0;
    size_t old_capacity = 0;

    void *target = NULL;
    void *newstart = NULL;

    int back_index = 0;
    int i = 0;

    size_t fin = 0;
    size_t end = 0;
    
    massert_container(v);

    old_size = v_size(v);
    old_capacity = v_capacity(v);

    if (old_capacity == n) {
        return;
    } else if (n == 0) {
        WARNING(__FILE__, "v_resize must receive a nonzero value for n.");
    }

    /**
     *  If n is less than the old size, and items were deep copied --
     *  dynamically allocated memory will be released as per the dtor function
     *  prior to truncation
     *  (if dtor is defined in ttbl, copy should be defined as well)
     */
    if (n < old_size && v->ttbl->dtor) {
        target = (char *)(v->impl.start) + (n * v->ttbl->width);

        back_index = n - 1;
        for (i = 0; i < back_index; i++) {
            v->ttbl->dtor(target);
            target = (char *)(target) - (v->ttbl->width);
        }
    }

    newstart = realloc(v->impl.start, n * v->ttbl->width);
    massert_realloc(newstart);

    fin = n > old_size ? old_size : n;
    end = n > old_size ? n : fin;

    v->impl.start = newstart;
    v->impl.finish = (char *)(v->impl.start) + (fin * v->ttbl->width);
    v->impl.end_of_storage = (char *)(v->impl.start) + (end * v->ttbl->width);
}

/**
 *  @brief  Returns the capacity of v
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return         capacity of v
 */
size_t v_capacity(vector *v) {
    massert_container(v);
    /* effectively v->impl.end_of_storage - v->impl.start */
    return ptr_distance(v->impl.start, v->impl.end_of_storage, v->ttbl->width);
}

/**
 *  @brief  Determines if v is an empty vector, or not
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     true if v->impl.start == v->impl.finish, false otherwise
 */
bool v_empty(vector *v) {
    massert_container(v);
    /**
     *  Since v->impl.finish is always one address ahead of vector's back element,
     *  if v->impl.start == v->impl.finish, the vector is empty.
     */
    return v->impl.start == v->impl.finish;
}

/**
 *  @brief  Reserves n blocks of elements for v
 *
 *  @param[out] v   pointer to vector
 *  @param[in]  n   desired amount of blocks to reserve
 *
 *  This function is effectively a conditional resize --
 *  n must exceed that of v's current capacity.
 */
void v_reserve(vector *v, size_t n) {
    massert_container(v);

    /**
     *  Reserve is effectively a resize,
     *  but verifies that the value of n meets the requirements for a reservation.
     */
    if (n > v_capacity(v)) {
        v_resize(v, n);
    } else {
        /* no-op */
        ERROR(__FILE__, "n must be greater than vector's buffer capacity.");
        return;
    }
}

/**
 *  @brief  Shrinks vector's buffer to that of its logical length
 *
 *  @param[in] v    pointer to vector
 *
 *  This function is effectively a conditional resize --
 *  but verifies that:
 *      - vector is not empty
 *      - vector's finish pointer is not equal to end_of_storage pointer
 */
void v_shrink_to_fit(vector *v) {
    massert_container(v);

    if ((v->impl.start != v->impl.finish) && (v->impl.finish != v->impl.end_of_storage)) {
        v_resize(v, v_size(v));
    }
}

/**
 *  @brief  Retrieves the address of an element from vector at index n
 *
 *  @param[in]  v   pointer to vector
 *  @param[in]  n   index of desired element
 *
 *  @return     address of element at n
 *
 *  A bounds check is performed to ensure that n is a valid index.
 *
 *  Note that the address (pointer) of the element type is returned,
 *  be sure to cast the return of v_at to the pointer type of
 *  the element, then dereference the casted v_at to yield
 *  the element itself.
 */
void *v_at(vector *v, size_t n) {
    size_t size = 0;
    void *target = NULL;

    massert_container(v);

    size = v_size(v);
    target = NULL;

    if (n >= size) {
        char str[256];
        sprintf(str, "Input %lu is greater than vector's logical length, %lu -- index out of bounds.", n, size);
        ERROR(__FILE__, str);
        return NULL;
    } else if (n == 0) {
        /* if n is 0, it's the front of the vector */
        target = v->impl.start;
    } else if (n == (size - 1)) {
        /* if n is (size - 1), the back index, effectively (v->impl.finish - 1) */
        target = (char *)(v->impl.finish) - (v->ttbl->width);
    } else {
        /* if n is anywhere within (0, size - 1), effectively (v->impl.start + n) */
        target = (char *)(v->impl.start) + (n * v->ttbl->width);
    }

    return target ? target : NULL;
}

/**
 *  @brief  Retrieves the address of the front element from vector
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     address of front element
 *
 *  Note that the address (pointer) of the element type is returned,
 *  be sure to cast the return of v_front to the pointer type of
 *  the element, then dereference the casted v_front to yield
 *  the element itself.
 */
void *v_front(vector *v) {
    massert_container(v);
    /**
     *  v_front() returns v->impl.start,
     *  so if
     *      TYPE is your intended data type,
     *  and
     *      *(TYPE *)(v->impl.start)
     *  refers to a (TYPE),
     *  and
     *      (TYPE *)(v->impl.start)
     *  refers to a (TYPE *)(,
     *  then v_front(v) can be casted as such:
     *      (TYPE *)(v_front(v))
     *  and dereferenced --
     *     *(TYPE *)(v_front(v))
     *  to yield:
     *      TYPE front = *(TYPE *)(v_front(v));
     */
    return v->impl.start;
}

/**
 *  @brief  Retrieves the address of the rear element from vector
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     address of rear element
 *
 *  Note that the address (pointer) of the element type is returned,
 *  be sure to cast the return of v_back to the pointer type of
 *  the element, then dereference the casted v_back to yield
 *  the element itself.
 */
void *v_back(vector *v) {
    massert_container(v);
    /**
     *  v_back() returns (char *)(v->impl.finish) - (v->ttbl->width),
     *  which is effectively (v->impl.finish - 1).
     *  so if
     *      TYPE is your intended data type,
     *  and
     *      *((TYPE *)(v->impl.finish) - (v->ttbl->width));
     *  refers to a (TYPE),
     *  and
     *      (TYPE *)(v->impl.finish) - (v->ttbl->width);
     *  refers to a (TYPE *),
     *  then v_back(v) can be casted as such:
     *      (TYPE *)(v_back(v))
     *  and dereferenced --
     *     *(TYPE *)(v_back(v))
     *  to yield:
     *      TYPE back = *(TYPE *)(v_back(v));
     */
    return (char *)(v->impl.finish) - (v->ttbl->width);
}

/**
 *  @brief  Retrieves the address of vector's buffer
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     address of v->impl.start
 */
void *v_data(vector *v) {
    massert_container(v);
    /**
     *  v_data() returns the address of v->impl.start,
     *  so if
     *      TYPE is your intended data type,
     *  and
     *      *(TYPE *)(v->impl.start)
     *  refers to a (TYPE),
     *  and
     *       (TYPE *)(v->impl.start)
     *  refers to a (TYPE *),
     *  and
     *      v_data(v)
     *  returns &(v->impl.start),
     *  then v_data(v) can be casted as such:
     *      (TYPE **)(v_data(v))
     *  and dereferenced --
     *     *(TYPE **)(v_data(v));
     *  to yield:
     *      TYPE *array = *(TYPE **)(v_data(v));
     *  and array[i] will yield a TYPE at index i.
     */
    return &(v->impl.start);
}

/**
 *  @brief  Appends an element to the rear of the vector
 *
 *  @param[in]  v       pointer to vector
 *  @param[in]  valaddr address of element to be copied
 *
 *  If a copy function is defined in v's ttbl,
 *  valaddr will be deep-copied into v.
 *  Otherwise, valaddr will be shallow-copied into v
 *  using memcpy.
 */
void v_pushb(vector *v, const void *valaddr) {
    massert_container(v);
    massert_ptr(valaddr);

    /**
     *  A doubling strategy is employed when the finish pointer
     *  meets the end_of_storage pointer.
     */
    if (v->impl.finish == v->impl.end_of_storage) {
        v_resize(v, v_capacity(v) * 2);
    }

    if (v->ttbl->copy) {
        /* if copy fn defined in ttbl, deep copy */
        v->ttbl->copy(v->impl.finish, valaddr);
    } else {
        /* if no copy defined in ttbl, shallow copy */
        gcs__memcpy(v->impl.finish, valaddr, v->ttbl->width);
    }

    /* advance finish pointer to the next empty block */
    v->impl.finish = (char *)(v->impl.finish) + (v->ttbl->width);
}

/**
 *  @brief  Removes element at the rear of the vector
 *
 *  @param[in]  v   pointer to vector
 *
 *  If a dtor function is defined in v's ttbl,
 *  the rear element will be destroyed using the dtor function
 *  from within ttbl.
 *
 *  Otherwise, the popped element will be overwritten by the next
 *  element that is added at the finish pointer.
 *
 *  Memory management of dynamically allocated elements/elements with
 *  dynamically allocated fields become the client's responsibility
 *  if a dtor function is NOT defined within v's ttbl.
 */
void v_popb(vector *v) {
    massert_container(v);

    if (v->impl.finish == v->impl.start) {
        /* v_popb is a no-op if vector is empty */
        return;
    }

    /* decrement the finish pointer to the address of the "victim" block */
    v->impl.finish = (char *)(v->impl.finish) - (v->ttbl->width);

    if (v->ttbl->dtor) {
        /**
         *  If dtor defined in ttbl,
         *  release memory at finish pointer as defined by dtor.
         */
        v->ttbl->dtor(v->impl.finish);
    }

    /**
     *  If no dtor defined, the next element appendage
     *  will simply be overwritten
     */
}

/**
 *  @brief  Destroys elements from within v
 *
 *  @param[in]  v   pointer to vector
 *
 *  Memory management of dynamically allocated elements
 *  and/or elements with dynamically allocated fields
 *  become the client's responsibility if a dtor function
 *  is NOT defined within v's ttbl.
 */
void v_clear(vector *v) {
    massert_container(v);

    if (v->impl.finish == v->impl.start) {
        /**
         *  if vector is empty
         *  (start and finish pointers share same address),
         *  it's a no-op.
         */
        return;
    }

    if (v->ttbl->dtor) {
        /* decrementing finish pointer to match the address of the last element */

        /**
         *  If elements were deep-copied,
         *  their memory must be released as per the
         *  client-supplied dtor function in ttbl.
         */
        v->impl.finish = (char *)(v->impl.finish) - (v->ttbl->width);

        while (v->impl.finish != v->impl.start) {
            v->ttbl->dtor(v->impl.finish);
            v->impl.finish = (char *)(v->impl.finish) - (v->ttbl->width);
        }

        v->ttbl->dtor(v->impl.finish);
        gcs__memset(v->impl.start, 0, v_size(v) * v->ttbl->width);
        /* v->impl.finish is already at v->impl.start. */
    } else {
        /**
         *  If elements were shallow copied,
         *  (no dtor function specified in ttbl) --
         *  we just memset and reset the finish pointer.
         */
        gcs__memset(v->impl.start, 0, v_size(v) * v->ttbl->width);
        v->impl.finish = v->impl.start;
    }
}

/**
 *  @brief  Prints a diagnostic of vector to stdout
 *
 *  @param[in]  v   pointer to vector
 */
void v_puts(vector *v) {
    /* redirect to v_fputs with stream stdout */
    v_fputs(v, stdout);
}

/**
 *  @brief  Prints the contents of vector with user-defined formatting
 *
 *  @param[in]  v           pointer to vector
 *  @param[in]  before      string that appears before any elements appear
 *  @param[in]  after       string that appears after all the elements have appeared
 *  @param[in]  postelem    string that appears after each element, except the last one
 *  @param[in]  breaklim    amount of elements that print before a line break occurs.
 *                          0 means no line breaks
 */
void v_putsf(vector *v,
             const char *before,
             const char *after,
             const char *postelem,
             const char *empty,
             size_t breaklim) {
    /* redirect to v_fputsf with stream stdout */
    v_fputsf(v, stdout, before, after, postelem, empty, breaklim);
}

/**
 *  @brief  Prints a diagnostic of vector to file stream dest
 *
 *  @param[in]  v       pointer to vector
 *  @param[in]  dest    file stream (e.g stdout, stderr, a file)
 */
void v_fputs(vector *v, FILE *dest) {
    char buffer1[MAXIMUM_STACK_BUFFER_SIZE];
    char buffer2[MAXIMUM_STACK_BUFFER_SIZE];

    const char *link = "------------------------------";
    const char *bytes_label = NULL;
    const char *postelem = "";
    const char *empty = "--- Container is empty ---";

    const size_t breaklim = 1;

    massert_container(v);
    massert_ptr(dest);

    sprintf(buffer1, "\n%s\n%s\n%s\n", link, "Elements", link);

    bytes_label = v->ttbl->width == 1 ? "byte" : "bytes";

    sprintf(buffer2, "%s\n%s\t\t%lu\n%s\t%lu\n%s\t%lu %s\n%s\n", link, "Size",
            v_size(v), "Capacity", v_capacity(v), "Element size", v->ttbl->width,
            bytes_label, link);

    v_fputsf(v, dest, buffer1, buffer2, postelem, empty, breaklim);
}

/**
 *  @brief  Prints the contents of vector with user-defined formatting,
 *          to file stream dest
 *
 *  @param[in]  v           pointer to vector
 *  @param[in]  dest        file stream (e.g. stdout, stderr, a file)
 *  @param[in]  before      string that appears before any elements appear
 *  @param[in]  after       string that appears after all the elements have appeared
 *  @param[in]  postelem    string that appears after each element, except the last one
 *  @param[in]  breaklim    amount of elements that print before a line break occurs.
 *                          0 means no line breaks
 */
void v_fputsf(vector *v, FILE *dest,
              const char *before,
              const char *after,
              const char *postelem,
              const char *empty,
              size_t breaklim) {
    void (*print)(const void *, FILE *dest) = NULL;

    size_t size = 0;
    size_t i = 0;
    size_t curr = 0;

    void *target = NULL;

    massert_container(v);
    massert_ptr(dest);

    fprintf(dest, "%s", before ? before : "");

    print = v->ttbl->print;

    size = v_size(v);

    if (size == 0) {
        fprintf(dest, "%s\n", empty ? empty : "");
    } else {
        target = v->impl.start;

        for (i = 0, curr = 1; i < size; i++, curr++) {
            print(target, dest);

            /* address - disable for release */
            fprintf(dest, "\t\t(%s%p%s)", KCYN, target, KNRM);

            if (i < size - 1) {
                fprintf(dest, "%s", postelem ? postelem : "");
            }

            if (curr == breaklim) {
                curr = 0;
                fprintf(dest, "\n");
            }

            target = (char *)(target) + (v->ttbl->width);
        }
    }

    fprintf(dest, "%s", after ? after : "");
}

/**
 *  @brief  Calls malloc to allocate memory for a pointer to vector
 *
 *  @return     pointer to vector
 */
static vector *v_allocate(void) {
    vector *v = NULL;
    v = malloc(sizeof *v);
    return v;
}

/**
 *  @brief  "Constructor" function, initializes vector
 *
 *  @param[in]  v       pointer to vector
 *  @param[in]  ttbl    pointer to typetable; width/copy/dtor/swap/compare/dtor
 *  @param[in]  capacity    capacity desired for vector
 */
static void v_init(vector *v, struct typetable *ttbl, size_t capacity) {
    void *start = NULL;

    massert_container(v);

    massert_ptr(ttbl);
    v->ttbl = ttbl;

    if (capacity <= 0) {
        WARNING(__FILE__, "Provided input capacity was less than or equal to 0. Will default to capacity of 1.");
        capacity = 1;
    } 

    start = calloc(capacity, v->ttbl->width);
    massert_calloc(start);

    v->impl.start = start;
    v->impl.finish = v->impl.start;

    v->impl.end_of_storage
    = (char *)(v->impl.start) + (capacity * v->ttbl->width);
}

/**
 *  @brief "Destructor" function, deinitializes vector
 *
 *  @param[in]  v   pointer to vector
 */
static void v_deinit(vector *v) {
    if (v == NULL) {
        return;
    }

    v_clear(v);

    free(v->impl.start);
    v->impl.start = NULL;
    v->impl.finish = NULL;
    v->impl.end_of_storage = NULL;

    v->ttbl = NULL;
}

/**
 *  @brief  Swaps the content at first and second, address from within v
 *
 *  @param[in]  v       pointer to vector
 *  @param[out] first   first address to swap content
 *  @param[out] second  second address to swap content
 */
static void v_swap_addr(vector *v, void *first, void *second) {
    void *temp = NULL;

    massert_container(v);
    massert_ptr(first);
    massert_ptr(second);

    temp = malloc(v->ttbl->width);
    massert_malloc(temp);
    gcs__memcpy(temp, first, v->ttbl->width);

    gcs__memcpy(first, second, v->ttbl->width);
    gcs__memcpy(second, temp, v->ttbl->width);

    free(temp);
    temp = NULL;
}

/**
 *  @def        VECTOR_TMPL_MAXIMUM_STACK_BUFFER_SIZE
 *  @brief      Arbitrary maximum size for a statically allocated (char) buffer
 */
#define VECTOR_TMPL_MAXIMUM_STACK_BUFFER_SIZE 16384
#define VECTOR_DEFAULT_CAPACITY 16

/* optional macros for accessing the innards of vector_base */
#define AT(VEC, INDEX) ((VEC->impl.start) + (INDEX))
#define FRONT(VEC) ((VEC->impl.start))
#define BACK(VEC) ((VEC->impl.finish) - 1)
#define END(VEC) (VEC->impl.end_of_storage)

static vector_str *vallocate_str(void);
static void vinit_str(vector_str *v, size_t capacity);
static void vdeinit_str(vector_str *v);

/**
 *  @brief  Allocates, constructs, and returns a pointer to vector_str,
 *          size VECTOR_DEFAULT_CAPACITY (16)
 *
 *  @return     pointer to vector_str
 */
vector_str *vnew_str(void) {
    vector_str *v = vallocate_str();       /* allocate */
    vinit_str(v, VECTOR_DEFAULT_CAPACITY); /* construct */
    return v;                              /* return */
}

/**
 *  @brief  Allocates, constructs, and returns a pointer to vector_str,
 *          size n
 *
 *  @return     pointer to vector_str
 */
vector_str *vnewr_str(size_t n) {
    vector_str *v = vallocate_str(); /* allocate */
    vinit_str(v, n);                 /* construct */
    return v;                        /* return */
}

/**
 *  @brief  Calls vdeinit_str (vector_str's destructor) and deallocates the
 *pointer v.
 *
 *  @param[out] v   Address of a pointer to vector_str
 *
 *  Every call to any of vector_str's "new" functions should be accompanied
 *  by a call to vdelete_str when a pointer to vector_str is no longer needed.
 *
 *  If the vector_str has a ttbl with a dtor function defined,
 *  the elements within the vector_str will be destroyed using the dtor
 *function.
 *
 *  Note that if the elements from within v are dynamically allocated,
 *  and/or the elements have dynamically allocated fields --
 *  and there is no dtor function defined, memory management
 *  will become the client's responsibility.
 */
void vdelete_str(vector_str **v) {
    massert_ptr((*v));

    /**
     *  Deinitialization involves releasing all dynamically allocated
     *  memory at each field (if dtor function is defined in ttbl,
     *  and is written to release such memory) --
     *  after that step (or if there is no memory to free within the elements),
     *  (*v)->impl.start has its memory freed, and the remainder of (*v)'s
     * fields
     *  are set NULL.
     */
    vdeinit_str((*v));

    /* finally, the memory (*v) points to will be freed. */
    free((*v));
    (*v) = NULL;
}

/**
 *  @brief  Returns the logical length of v
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  @return     logical length of v
 */
size_t vsize_str(vector_str *v) {
    massert_container(v);
    return v->impl.finish - v->impl.start;
}

/**
 *  @brief  Resizes the vector_str to size n
 *
 *  @param[in]  v   pointer to vector_str
 *  @param[in]  n   desired size for v
 *
 *  If n is less than the current logical length (vsize_str(v)),
 *  v will be truncated (excess elements are destroyed),
 *  otherwise, v's capacity will be extended.
 */
void vresize_str(vector_str *v, size_t n) {
    size_t old_size = 0;
    size_t old_capacity = 0;

    str *target = NULL;
    str *newstart = NULL;

    int back_index = 0;
    int i = 0;

    size_t fin = 0;
    size_t end = 0;

    massert_container(v);

    old_size = vsize_str(v);
    old_capacity = vcapacity_str(v);

    if (old_capacity == n) {
        return;
    } else if (n == 0) {
        WARNING(__FILE__, "vresize must receive a nonzero value for n.");
    }

    /**
     *  If n is less than the old size, and items were deep copied --
     *  dynamically allocated memory will be released as per the dtor function
     *  prior to truncation
     *  (if dtor is defined in ttbl, copy should be defined as well)
     */
    if (n < old_size && v->ttbl->dtor) {
        target = v->impl.start + n;

        back_index = n - 1;
        for (i = 0; i < back_index; i++) {
            v->ttbl->dtor(target--);
        }
    }

    newstart = realloc(v->impl.start, n * v->ttbl->width);
    massert_realloc(newstart);

    fin = n > old_size ? old_size : n;
    end = n > old_size ? n : fin;

    v->impl.start = newstart;
    v->impl.finish = v->impl.start + fin;
    v->impl.end_of_storage = v->impl.start + end;
}

/**
 *  @brief  Returns the capacity of v
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  @return         capacity of v
 */
size_t vcapacity_str(vector_str *v) {
    massert_container(v);
    return v->impl.end_of_storage - v->impl.start;
}

/**
 *  @brief  Determines if v is an empty vector, or not
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  @return     true if v->impl.start == v->impl.finish, false otherwise
 */
bool vempty_str(vector_str *v) {
    massert_container(v);
    /**
     *  Since v->impl.finish is always one address ahead of vector's back
     * element,
     *  if v->impl.start == v->impl.finish, the vector is empty.
     */
    return v->impl.start == v->impl.finish;
}

/**
 *  @brief  Shrinks vector_str's buffer to that of its logical length
 *
 *  @param[in] v    pointer to vector_str
 *
 *  This function is effectively a conditional resize --
 *  but verifies that:
 *      - vector is not empty
 *      - vector's finish pointer is not equal to end_of_storage pointer
 */
void vshrinktofit_str(vector_str *v) {
    massert_container(v);

    if ((v->impl.start != v->impl.finish) && (v->impl.finish != v->impl.end_of_storage)) {
        vresize_str(v, vsize_str(v));
    }
}

/**
 *  @brief  Retrieves the address of an element from vector at index n
 *
 *  @param[in]  v   pointer to vector_str
 *  @param[in]  n   index of desired element
 *
 *  @return     address of element at n
 *
 *  A bounds check is performed to ensure that n is a valid index.
 */
str *vat_str(vector_str *v, size_t n) {
    size_t size = 0;
    str *target = NULL;

    massert_container(v);

    size = vsize_str(v);
    target = NULL;

    if (n >= size) {
        char str[256];
        sprintf(str,
                "Input %lu is greater than vector's logical length, %lu -- "
                "index out of bounds.",
                n,
                size);
        ERROR(__FILE__, str);
        return NULL;
    } else if (n == 0) {
        /* if n is 0, it's the front of the vector */
        target = v->impl.start;
    } else if (n == (size - 1)) {
        /* if n is (size - 1), the back index, effectively (v->impl.finish - 1)
         */
        target = v->impl.finish - 1;
    } else {
        /* if n is anywhere within (0, size - 1), effectively (v->impl.start +
         * n) */
        target = v->impl.start + n;
    }

    return target ? target : NULL;
}

/**
 *  @brief  Retrieves the address of the front element from vector_str
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  @return     address of front element
 */
str *vfront_str(vector_str *v) {
    massert_container(v);
    return v->impl.start;
}

/**
 *  @brief  Retrieves the address of the rear element from vector_str
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  @return     address of rear element
 */
str *vback_str(vector_str *v) {
    massert_container(v);
    return v->impl.finish - 1;
}

/**
 *  @brief  Retrieves the address of vector_str's buffer
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  @return     address of v->impl.start
 */
str **vdata_str(vector_str *v) {
    massert_container(v);
    return &(v->impl.start);
}

/**
 *  @brief  Appends an element to the rear of the vector
 *
 *  @param[in]  v       pointer to vector
 *  @param[in]  val     element to be copied
 *
 *  If a copy function is defined in v's ttbl,
 *  valaddr will be deep-copied into v.
 *  Otherwise, valaddr will be shallow-copied into v
 *  using memcpy.
 */
void vpushb_str(vector_str *v, str val) {
    massert_container(v);

    /**
     *  A doubling strategy is employed when the finish pointer
     *  meets the end_of_storage pointer.
     */
    if (v->impl.finish == v->impl.end_of_storage) {
        vresize_str(v, vcapacity_str(v) * 2);
    }

    if (v->ttbl->copy) {
        /* deep copy */
        v->ttbl->copy(v->impl.finish++, &val);
    } else {
        /* shallow copy */
        gcs__memcpy(v->impl.finish++, val, v->ttbl->width);
    }

    /* finish pointer advanced to the next empty block */
}

/**
 *  @brief  Appends an element to the rear of the vector
 *
 *  @param[in]  v       pointer to vector_str
 *  @param[in]  valaddr address of element to be copied
 *
 *  If a copy function is defined in v's ttbl,
 *  valaddr will be deep-copied into v.
 *  Otherwise, valaddr will be shallow-copied into v
 *  using memcpy.
 */
void vpushbptr_str(vector_str *v, str *valaddr) {
    massert_container(v);
    massert_ptr(valaddr);

    /**
     *  A doubling strategy is employed when the finish pointer
     *  meets the end_of_storage pointer.
     */
    if (v->impl.finish == v->impl.end_of_storage) {
        vresize_str(v, vcapacity_str(v) * 2);
    }

    if (v->ttbl->copy) {
        /* deep copy */
        v->ttbl->copy(v->impl.finish++, valaddr);
    } else {
        /* shallow copy */
        gcs__memcpy(v->impl.finish++, valaddr, v->ttbl->width);
    }

    /* finish pointer advanced to the next empty block */
}

/**
 *  @brief  Removes element at the rear of the vector
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  If a dtor function is defined in v's ttbl,
 *  the rear element will be destroyed using the dtor function
 *  from within ttbl.
 *
 *  Otherwise, the popped element will be overwritten by the next
 *  element that is added at the finish pointer.
 *
 *  Memory management of dynamically allocated elements/elements with
 *  dynamically allocated fields become the client's responsibility
 *  if a dtor function is NOT defined within v's ttbl.
 */
void vpopb_str(vector_str *v) {
    massert_container(v);

    if (v->impl.finish == v->impl.start) {
        /* vpopb is a no-op if vector is empty */
        return;
    }

    /* decrement the finish pointer to the address of the "victim" block */
    --v->impl.finish;

    if (v->ttbl->dtor) {
        /**
         *  If dtor defined in ttbl,
         *  release memory at finish pointer as defined by dtor.
         */
        v->ttbl->dtor(v->impl.finish);
    }

    /**
     *  If no dtor defined, the next element appendage
     *  will simply be overwritten
     */
}

/**
 *  @brief  Destroys elements from within v
 *
 *  @param[in]  v   pointer to vector_str
 *
 *  Memory management of dynamically allocated elements
 *  and/or elements with dynamically allocated fields
 *  become the client's responsibility if a dtor function
 *  is NOT defined within v's ttbl.
 */
void vclear_str(vector_str *v) {
    massert_container(v);

    if (v->impl.finish == v->impl.start) {
        /**
         *  if vector is empty
         *  (start and finish pointers share same address),
         *  it's a no-op.
         */
        return;
    }

    if (v->ttbl->dtor) {
        /* decrementing finish pointer to match the address of the last element
         */

        /**
         *  If elements were deep-copied,
         *  their memory must be released as per the
         *  client-supplied dtor function in ttbl.
         */
        --v->impl.finish;

        while (v->impl.finish != v->impl.start) {
            v->ttbl->dtor(v->impl.finish--);
        }

        v->ttbl->dtor(v->impl.finish);
        gcs__memset(v->impl.start, '\0', vsize_str(v));
        /* v->impl.finish is already at v->impl.start. */
    } else {
        /**
         *  If elements were shallow copied,
         *  (no dtor function specified in ttbl) --
         *  we just memset and reset the finish pointer.
         */
        gcs__memset(v->impl.start, '\0', vsize_str(v));
        v->impl.finish = v->impl.start;
    }
}

/**
 *  @brief  Prints a diagnostic of vector_str to stdout
 *
 *  @param[in]  v   pointer to vector_str
 */
void vputs_str(vector_str *v) {
    /* redirect to vfputs with stream stdout */
    vfputs_str(v, stdout);
}

/**
 *  @brief  Prints the contents of vector_str with user-defined formatting
 *
 *  @param[in]  v           pointer to vector_str
 *  @param[in]  before      string that appears before any elements appear
 *  @param[in]  after       string that appears after all the elements have
 *appeared
 *  @param[in]  postelem    string that appears after each element, except the
 *last one
 *  @param[in]  breaklim    amount of elements that print before a line break
 *occurs.
 *                          0 means no line breaks
 */
void vputsf_str(vector_str *v,
                const char *before,
                const char *after,
                const char *postelem,
                const char *empty,
                size_t breaklim) {
    /* redirect to vfputsf with stream stdout */
    vfputsf_str(v, stdout, before, after, postelem, empty, breaklim);
}

/**
 *  @brief  Prints a diagnostic of vector_str to file stream dest
 *
 *  @param[in]  v       pointer to vector_str
 *  @param[in]  dest    file stream (e.g stdout, stderr, a file)
 */
void vfputs_str(vector_str *v, FILE *dest) {
    char buffer1[MAXIMUM_STACK_BUFFER_SIZE];
    char buffer2[MAXIMUM_STACK_BUFFER_SIZE];

    const char *link = "------------------------------";
    const char *bytes_label = NULL;
    const char *postelem = "";
    const char *empty = "--- Container is empty ---";

    const size_t breaklim = 1;

    massert_container(v);
    massert_ptr(dest);

    sprintf(buffer1, "\n%s\n%s\n%s\n", link, "Elements", link);

    bytes_label = v->ttbl->width == 1 ? "byte" : "bytes";

    sprintf(buffer2,
            "%s\n%s\t\t%lu\n%s\t%lu\n%s\t%lu %s\n%s\n",
            link,
            "Size",
            vsize_str(v),
            "Capacity",
            vcapacity_str(v),
            "Element size",
            v->ttbl->width,
            bytes_label,
            link);

    vfputsf_str(v, dest, buffer1, buffer2, postelem, empty, breaklim);
}

/**
 *  @brief  Prints the contents of vector_str with user-defined formatting,
 *          to file stream dest
 *
 *  @param[in]  v           pointer to vector_str
 *  @param[in]  dest        file stream (e.g. stdout, stderr, a file)
 *  @param[in]  before      string that appears before any elements appear
 *  @param[in]  after       string that appears after all the elements have
 *appeared
 *  @param[in]  postelem    string that appears after each element, except the
 *last one
 *  @param[in]  breaklim    amount of elements that print before a line break
 *occurs.
 *                          0 means no line breaks
 */
void vfputsf_str(vector_str *v,
                 FILE *dest,
                 const char *before,
                 const char *after,
                 const char *postelem,
                 const char *empty,
                 size_t breaklim) {
    void (*print)(const void *, FILE *dest) = NULL;

    size_t size = 0;
    size_t i = 0;
    size_t curr = 0;

    str *target = NULL;

    massert_container(v);
    massert_ptr(dest);

    fprintf(dest, "%s", before ? before : "");

    print = v->ttbl->print;

    size = vsize_str(v);

    if (size == 0) {
        fprintf(dest, "%s\n", empty ? empty : "");
    } else {
        target = v->impl.start;

        for (i = 0, curr = 1; i < size; i++, curr++) {
            print(target, dest);

            /* address - disable for release */
            fprintf(dest, "\t\t(%s%p%s)", KCYN, (void *)(target), KNRM);

            if (i < size - 1) {
                fprintf(dest, "%s", postelem ? postelem : "");
            }

            if (curr == breaklim) {
                curr = 0;
                fprintf(dest, "\n");
            }

            ++target;
        }
    }

    fprintf(dest, "%s", after ? after : "");
}

/**
 *  @brief  Calls malloc to allocate memory for a pointer to vector_str
 *
 *  @return     pointer to vector_str
 */
static vector_str *vallocate_str(void) {
    vector_str *v = NULL;
    v = malloc(sizeof *v);
    return v;
}

/**
 *  @brief  "Constructor" function, initializes vector
 *
 *  @param[in]  v           pointer to vector_str
 *  @param[in]  capacity    capacity desired for vector
 */
static void vinit_str(vector_str *v, size_t capacity) {
    str *start = NULL;

    massert_container(v);

    v->ttbl = _str_;

    if (capacity <= 0) {
        WARNING(__FILE__,
                "Provided input capacity was less than or equal to "
                "0. Will default to capacity of 1.");
        capacity = 1;
    }

    start = calloc(capacity, v->ttbl->width);
    massert_calloc(start);

    v->impl.start = start;
    v->impl.finish = v->impl.start;
    v->impl.end_of_storage = v->impl.start + capacity;
}

/**
 *  @brief "Destructor" function, deinitializes vector
 *
 *  @param[in]  v   pointer to vector_str
 */
static void vdeinit_str(vector_str *v) {
    if (v == NULL) {
        return;
    }

    vclear_str(v);

    free(v->impl.start);
    v->impl.start = NULL;
    v->impl.finish = NULL;
    v->impl.end_of_storage = NULL;

    v->ttbl = NULL;
}

#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
char *gcs__strcpy(char *dst, const char *src) {
    /*
    char ch = ' ';
    int i = 0;

    while ((ch = (*src++)) != '\0') {
        (*dst++) = ch;
        ++i;
    }

    *(dst + i) = '\0';

    return dst;
    */

    char *pos = dst;
    while ((*(pos++) = *(src++)) != '\0') { }
    return dst;

    /*
    int i = 0;
    while ((dst[i] = src[i]) != '\0') {
        i++;
    }
    return dst;
    */
}

char *gcs__strncpy(char *dst, const char *src, size_t n) {
    /*
    char ch = ' ';
    int i = 0;

    while ((ch = (*src++)) != '\0' && (i++) < n) {
        (*dst++) = ch;
    }
    */

    /*
    char *pos = dst;
    size_t i = 0;
    bool found = false;
    while ((*(pos++) = *(src++)) != '\0') { 
        if ((++i) == n) {
            found = true;
            break;
        }
    }

    pos[i] = found ? '\0' : pos[i];
    */
    char *pos = dst;
    size_t i = 0;
    while (i++ != n && (*pos++ = *src++));

    return dst;    
}

char *gcs__strdup(const char *src) {
    char *str = NULL;
    str = malloc(gcs__strlen(src) + 1);
    massert_malloc(str);

    gcs__strcpy(str, src);
    return str;
}

char *gcs__strndup(const char *src, size_t n) {
    char *str = NULL;
    size_t len = gcs__strlen(src);
    int delta = len - n;

    size_t i = 0;
    bool found = false;

    if (delta > 0) {
        str = malloc(n);
        massert_malloc(str);
        gcs__strncpy(str, src, n);
    }

    return str;
}

size_t gcs__strlen(const char *src) {
    int len = 0;
    while ((*src++) != '\0') {
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
     *  We iterate from left to right until the first
     *  pair of unlike characters between c1 and c2 are found.
     *  (of matching indices, of course.)
     *
     *  if c1[0] == c2[0], we continue until c1[n] != c2[n].
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

char *gcs__strtok(char *src, const char *delim) {
    static char *old_str = NULL;
    return gcs__strtok_r(src, delim, &old_str);
}

char *gcs__strtok_r(char *src, const char *delim, char **save_ptr) {
    char *end = NULL;
    size_t len = 0;
    size_t i = 0;

    src = src ? src : (*save_ptr);

    /**
     *  Nothing to tokenize.
     */
    if ((*src) == '\0') {
        (*save_ptr) = src;
        return NULL;
    }

    /**
     *  Advance to delimiter delim.
     */
    len = gcs__strlen(src);
    for (i = 0; i < len; i++) {
        if (src[i] == (*delim)) {
            break;
        }
    }

    if ((*src) == '\0') {
        /**
         *  End of string reached.
         *  Delimiter never found.
         */
        (*save_ptr) = src;
        return NULL;
    } else {
        src[i] = '\0';
    }

    end = src + (i + 1);

    if ((*end) == '\0') {
        (*save_ptr) = end;
        return src;
    }

    (*save_ptr) = end;

    return src;
}

int gcs__strncmp(const char *c1, const char *c2, size_t n) {
    int diff = 0;
    int i = 0;

    while ((i++) < n || (*c1) != '\0') {
        if ((*c1++) != (*c2++)) {
            diff += (c1 - c2);
        }
    }

    return diff;
}

void *gcs__memcpy(void *dst, const void *src, size_t width) {
    char *dest = dst;
    const char *source = src;
    int i = 0;

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
        *(dest + i) + *(temp + i);
    }

    free(temp);
    temp = NULL;

    return dst;
}

void *gcs__memset(void *dst, int ch, size_t n) {}

int gcs__memcmp(const void *s1, const void *s2, size_t n) {}
#else

#endif /* !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_) */

/**
 *  Determine the memory "block" distance between two addresses,
 *  beg and end, by their "block" widths, determined by sizeof(TYPENAME)
 *
 *  @param[in]  beg     the beginning address
 *  @param[in]  end     the ending address
 *  @param[in]  width   sizeof(TYPENAME), or size in bytes of TYPENAME
 *
 *  @return     distance between beg and end
 *
 *  Precondition: beg and end must be non-NULL and pointing to memory
 *                of the same data type
 */
ptrdiff_t ptr_distance(const void *beg, const void *end, size_t width) {
    char *start = (((char *)(beg)) + (width));
    char *finish = (((char *)(end)) + (width));

    return ((finish - start) / width);
}

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
