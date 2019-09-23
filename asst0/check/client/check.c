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
# ifndef ULOG_DISABLE_WARNING
#  define WARNING(FILEMACRO, ...)                                                \
          ulog(ULOG_STREAM_WARNING, "[WARNING]", FILEMACRO, __func__,                \
          (long int)__LINE__, __VA_ARGS__)
# endif /* ULOG_DISABLE_WARNING */
#else
# ifndef ULOG_DISABLE_WARNING
#  define WARNING(FILEMACRO, MSG)                                                \
          ulog(ULOG_STREAM_WARNING, "[WARNING]", FILEMACRO, __func__,                \
          (long int)__LINE__, MSG)
# else
#  define WARNING(FILEMACRO, MSG)
# endif /* ULOG_DISABLE_WARNING */
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

#define massert_container(PTR);\
massert(PTR, "['"#PTR"' was found to be NULL - '"#PTR"' must be assigned to the return value of a container initializer function prior to use.]");

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

void *gcs__memcpy(void *dst, const void *src, size_t width);
void *gcs__memmove(void *dst, const void *src, size_t width);
void *gcs__memset(void *dst, int ch, size_t n);
int gcs__memcmp(const void *s1, const void *s2, size_t n);
#endif /* !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_) */

#if !defined(_STRING_H) || __APPLE__ && !defined(_STRING_H_)
#define streql(s1, s2) gcs__strcmp(s1, s2) == 0
#define strneql(s1, s2, n) gcs__strncmp(s1, s2, n) == 0
#else
#define streql(s1, s2) strcmp(s1, s2) == 0
#define strneql(s1, s2, n) strncmp(s1, s2, n) == 0
#endif

#if __linux__ && !__POSIX__
#define strdup(str) strcpy(malloc(strlen(str) + 1), src)
#define strndup(str, n) strcpy(malloc(n + 1), (str + n))
#endif /* __linux__ && !__POSIX__ */

/**< (char *) will be address as str by vector_str. */
typedef char *str;

/**< struct typetable: type information for a container */
struct typetable;

/**< str typetable functions */
void *str_copy(void *dst, const void *src);
void str_dtor(void *arg);
void str_swap(void *arg1, void *arg2);
int str_compare(const void *c1, const void *c2);
void str_print(const void *arg, FILE *dst);

/**< forward-declared typetable instances */
struct typetable ttbl_str;
struct typetable *_str_;

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
void vputsf_str(vector_str *v, const char *before, const char *after,
               const char *postelem, const char *empty, size_t breaklim);
void vfputs_str(vector_str *v, FILE *dest);
void vfputsf_str(vector_str *v, FILE *dest, const char *before,
                const char *after, const char *postelem, const char *empty,
                size_t breaklim);

/**< check: client functions - logging/errors */
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
char *check__expr_tok(char *src, const char *delim);
bool check__expr_assess(const char *expr,
                        const char *operands[],
                        const char *operators[],
                        const char *delim);

#define CHECK__OPERANDS                                                        \
    "false", "true", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"

#define CHECK__OPERATORS "+", "-", "*", "/", "AND", "OR", "NOT"

int main(int argc, char *argv[]) {
    const char *operands[] = { CHECK__OPERANDS };
    const char *operators[] = { CHECK__OPERATORS };

    char *pos = NULL;
    size_t len = 0;
    size_t i = 0;
    char ch = ' ';
    bool first_char = true;

    vector_str *v = NULL;
    size_t vsize = 0;
    char *expr = NULL;

    pos = *(argv + 1);
    len = gcs__strlen(pos);

    while ((*pos) == ' ') {
        ++pos;
    }

    *(pos + (len - 1)) = *(pos + (len - 1)) == '\"' ? '\0' : *(pos + (len - 1));

    v = vnew_str();
    i = 0;

    for (i = 0; i < len; i++) {
        if (*(pos + i) == '"' && first_char) {
            pos += (i + 1);
        }

        if (*(pos + i) == ';') {
            *(pos + i) = '\0';
            expr = pos;
            vpushb_str(v, expr);
            pos += (i + 1);
            i = 0;
        }

        first_char = first_char ? false : first_char;
    }

    vpushb_str(v, pos);

    vsize = vsize_str(v);
    for (i = 0; i < vsize; i++) {
        expr = (*(vat_str(v, i)));
        check__expr_assess(expr, operands, operators, " ");
    }

    /**< for debugging only, remove later. */
    vputs_str(v);
    vdelete_str(&v);

    return EXIT_SUCCESS;
}

int check__fexpr_log(FILE *dest, uint32_t ct_expr, uint32_t ct_logical, uint32_t ct_arithmetic) {
    char buffer[BUFFER_SIZE];
    int j = 0;

    j = sprintf(buffer + j,
                "Found %s%d%s expressions: %s%d%s logical and %s%d%s arithmetic.", 
                KWHT_b, ct_expr, KNRM, 
                KWHT_b, ct_logical, KNRM, 
                KWHT_b, ct_arithmetic, KNRM);

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
                 "%s: "KWHT_b"%s"KNRM" in "KMAG_b"expression %d"KNRM": %s in\n\t\"%s\"\n", KRED_b"Error"KNRM, 
                 err_type, ct_expr, desc, expr_fragmt);
    
    if (index > -1) {
        j += sprintf(buffer + j, "%s%s", spaces, KYEL_b"^"KNRM);
    }

    return fprintf(dest, "%s\n", buffer);
}

char *check__expr_tok(char *src, const char *delim) {


    return src;
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

void *str_copy(void *arg, const void *other) {
    char **target = (char **)(arg);
    char **source = (char **)(other);

    (*target) = malloc(gcs__strlen(*source) + 1);
    massert_malloc((*target));
    gcs__strcpy((*target), (*source));

    return (*target) ? (*target) : NULL;
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

    result = strcmp(cfirst, csecond);

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

struct typetable ttbl_str = {
    sizeof(str),
    str_copy,
    str_dtor,
    str_swap,
    str_compare,
    str_print
};

struct typetable *_str_ = &ttbl_str;

/**
 *  @def        VECTOR_TMPL_MAXIMUM_STACK_BUFFER_SIZE
 *  @brief      Arbitrary maximum size for a statically allocated (char) buffer
 */
#define VECTOR_TMPL_MAXIMUM_STACK_BUFFER_SIZE 16384
#define VECTOR_DEFAULT_CAPACITY 16

/* optional macros for accessing the innards of vector_base */
#define AT(VEC, INDEX)  ((VEC->impl.start) + (INDEX))
#define FRONT(VEC)      ((VEC->impl.start))
#define BACK(VEC)       ((VEC->impl.finish) - 1)
#define END(VEC)        (VEC->impl.end_of_storage)

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
        str *start;           /**< address of array base (first element) */
        str *finish;          /**< address reserved for next rear element */
        str *end_of_storage;  /**< addresses last allocated block of storage */
    } impl;

    struct typetable *ttbl; /*<< data width, cpy, dtor, swap, compare, print */
};

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
    vector_str *v = vallocate_str();  /* allocate */
    vinit_str(v, VECTOR_DEFAULT_CAPACITY); /* construct */
    return v;                                   /* return */
}

/**
 *  @brief  Allocates, constructs, and returns a pointer to vector_str,
 *          size n
 *
 *  @return     pointer to vector_str
 */
vector_str *vnewr_str(size_t n) {
    vector_str *v = vallocate_str();/* allocate */
    vinit_str(v, n);                     /* construct */
    return v;                                 /* return */
}

/**
 *  @brief  Calls vdeinit_str (vector_str's destructor) and deallocates the pointer v.
 *
 *  @param[out] v   Address of a pointer to vector_str
 *
 *  Every call to any of vector_str's "new" functions should be accompanied
 *  by a call to vdelete_str when a pointer to vector_str is no longer needed.
 *
 *  If the vector_str has a ttbl with a dtor function defined,
 *  the elements within the vector_str will be destroyed using the dtor function.
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
     *  (*v)->impl.start has its memory freed, and the remainder of (*v)'s fields
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
     *  Since v->impl.finish is always one address ahead of vector's back element,
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
        sprintf(str, "Input %lu is greater than vector's logical length, %lu -- index out of bounds.", n, size);
        ERROR(__FILE__, str);
        return NULL;
    } else if (n == 0) {
        /* if n is 0, it's the front of the vector */
        target = v->impl.start;
    } else if (n == (size - 1)) {
        /* if n is (size - 1), the back index, effectively (v->impl.finish - 1) */
        target = v->impl.finish - 1;
    } else {
        /* if n is anywhere within (0, size - 1), effectively (v->impl.start + n) */
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
        gcs__memcpy(v->impl.finish++, &val, v->ttbl->width);
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
        /* decrementing finish pointer to match the address of the last element */

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
        gcs__memset(v->impl.start, 0, vsize_str(v));
        /* v->impl.finish is already at v->impl.start. */
    } else {
        /**
         *  If elements were shallow copied,
         *  (no dtor function specified in ttbl) --
         *  we just memset and reset the finish pointer.
         */
        gcs__memset(v->impl.start, 0, vsize_str(v));
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
 *  @param[in]  after       string that appears after all the elements have appeared
 *  @param[in]  postelem    string that appears after each element, except the last one
 *  @param[in]  breaklim    amount of elements that print before a line break occurs.
 *                          0 means no line breaks
 */
void vputsf_str(vector_str *v, const char *before, const char *after,
               const char *postelem, const char *empty, size_t breaklim) {
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

    sprintf(buffer2, "%s\n%s\t\t%lu\n%s\t%lu\n%s\t%lu %s\n%s\n", link, "Size",
            vsize_str(v), "Capacity", vcapacity_str(v), "Element size", v->ttbl->width,
            bytes_label, link);

    vfputsf_str(v, dest, buffer1, buffer2, postelem, empty, breaklim);
}

/**
 *  @brief  Prints the contents of vector_str with user-defined formatting,
 *          to file stream dest
 *
 *  @param[in]  v           pointer to vector_str
 *  @param[in]  dest        file stream (e.g. stdout, stderr, a file)
 *  @param[in]  before      string that appears before any elements appear
 *  @param[in]  after       string that appears after all the elements have appeared
 *  @param[in]  postelem    string that appears after each element, except the last one
 *  @param[in]  breaklim    amount of elements that print before a line break occurs.
 *                          0 means no line breaks
 */
void vfputsf_str(vector_str *v, FILE *dest, const char *before,
                const char *after, const char *postelem, const char *empty,
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
        WARNING(__FILE__, "Provided input capacity was less than or equal to 0. Will default to capacity of 1.");
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
    char ch = ' ';
    int i = 0;

    while ((ch = (*src++)) != '\0') {
        (*dst++) = ch;
        ++i;
    }

    *(dst + i) = '\0';

    return dst;
}

char *gcs__strncpy(char *dst, const char *src, size_t n) {
    char ch = ' ';
    int i = 0;

    while ((ch = (*src++)) != '\0' && (i++) < n) {
        (*dst++) = ch;
    }

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

    if (delta > 0) {
        str = malloc(n + 1);
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
    int diff = 0;
    while ((*c1) != '\0') {
        if ((*c1++) != (*c2++)) {
            diff += (c1 - c2);
        }
    }

    return diff;
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

void *gcs__memset(void *dst, int ch, size_t n) {

}

int gcs__memcmp(const void *s1, const void *s2, size_t n) {

}

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
