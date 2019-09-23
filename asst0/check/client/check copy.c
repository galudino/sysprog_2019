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

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#else
# define false  0
# define true   1
typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef long int ptrdiff_t;
#endif

#include <unistd.h>

#include <assert.h>
#include <string.h>
#include <strings.h>

#define BUFFER_SIZE 256
#define MAXIMUM_STACK_BUFFER_SIZE 16384

#define KNRM "\x1B[0;0m"   /**< reset to standard color/weight */
#define KNRM_b "\x1B[0;1m" /**< standard color bold */

#define KGRY "\x1B[0;2m" /**< dark grey */

#define KBNK "\x1B[0;5m" /**< blink every second */

#define KRED "\x1B[0;31m" /**< red */
#define KGRN "\x1B[0;32m" /**< green */
#define KYEL "\x1B[0;33m" /**< yellow */
#define KBLU "\x1B[0;34m" /**< blue */
#define KMAG "\x1B[0;35m" /**< magenta */
#define KCYN "\x1B[0;36m" /**< cyan */
#define KWHT "\x1B[0;37m" /**< white */

#define KRED_b "\x1B[1;31m" /**< red bold */
#define KGRN_b "\x1B[1;32m" /**< green bold */
#define KYEL_b "\x1B[1;33m" /**< yellow bold */
#define KBLU_b "\x1B[1;34m" /**< blue bold */
#define KMAG_b "\x1B[1;35m" /**< magenta bold */
#define KCYN_b "\x1B[1;36m" /**< cyan bold */
#define KWHT_b "\x1B[1;37m" /**< white bold */

/**< utils: debugging */
int ulog(
    FILE *dest,
    const char *level,     /**< meant for "LOG" or "ERROR" */
    const char *file,      /**< meant for use with the __FILE__ macro */
    const char *func,      /**< meant for use with the __func__ macro */
    long double line,      /**< meant for use with the __LINE__ macro */
    const char *fmt, ...); /**< user's custom message */

bool ulog_attrs_disable[] = {false, false, false, false, false, false, false};

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
 *  MMM dd yyyy HH:mm:ss LEVEL [filepath/filename:linenumber] function_name message
 */
#ifdef ULOG_DISABLE_ALL
# define ULOG_DISABLE_BUG
# define ULOG_DISABLE_LOG
# define ULOG_DISABLE_ERROR
# define ULOG_DISABLE_WARNING
#endif

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
# ifndef ULOG_DISABLE_LOG
#  define LOG(FILEMACRO, ...)                                                    \
          ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__,    \
          __VA_ARGS__)
# else
#  define LOG(FILEMACRO, ...)
# endif
#else
# ifndef ULOG_DISABLE_LOG
#  define LOG(FILEMACRO, MSG)                                                    \
          ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__, MSG)
# else
#  define LOG(FILEMACRO, MSG)
# endif
#endif

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
# ifndef ULOG_DISABLE_ERROR
#  define ERROR(FILEMACRO, ...)                                                  \
          ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__,                    \
          (long int)__LINE__, __VA_ARGS__)
# endif
#else
# ifndef ULOG_DISABLE_ERROR
#  define ERROR(FILEMACRO, MSG)                                                  \
          ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__,                    \
          (long int)__LINE__, MSG)
# else
#  define ERROR(FILEMACRO, MSG)
# endif   
#endif

#define UTILS_LOG_ATTRS_COUNT 7
enum ULOG_ATTRS { DATE, TIME, LEVEL, FILENAME, LINE, FUNCTION, MESSAGE };
extern bool ulog_attrs_disable[UTILS_LOG_ATTRS_COUNT];

#define ULOG_TOGGLE_ATTR(ULOG_ATTR)                                            \
    ulog_attrs_disable[ULOG_ATTR] =                                            \
        (ulog_attrs_disable[ULOG_ATTR]) ? (false) : (true)

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
#define massert(CONDITION, MESSAGE)\
if (((CONDITION) == (false))) {\
    fprintf(stderr, "Assertion failed: (%s)\n", #CONDITION);\
    ERROR(__FILE__, (MESSAGE));\
    abort();\
}

#define massert_ptr(PTR);\
massert(PTR, "['"#PTR"' was found to be NULL - '"#PTR"' must be nonnull to continue.]");

#define massert_malloc(PTR);\
massert(PTR, "[Request for heap storage allocation failed (malloc returned NULL and was assigned to '"#PTR"')]");

#define massert_calloc(PTR);\
massert(PTR, "[Request for heap storage allocation failed (calloc returned NULL and was assigned to '"#PTR"')]");

#define massert_realloc(PTR);\
massert(PTR, "[Request for heap storage reallocation failed (realloc returned NULL and was assigned to '"#PTR"')]");

#define massert_pfunc(PFUNC);\
massert(PFUNC, "['"#PFUNC"' was found to be NULL - '"#PFUNC"' must be assigned to a function with a matching prototype.]");

/**
 *  Pointer utilities
 */
ptrdiff_t ptr_distance(const void *beg, const void *end, size_t width);

/**< C-String macros */
#define gcs__streql(s1, s2) gcs__strcmp(s1, s2) == 0
#define gcs__strneql(s1, s2, n) gcs__strncmp(s1, s2, n) == 0

/**< client program functions */
char *gcs__strcpy(char *dst, const char *src);
char *gcs__strncpy(char *dst, const char *src, size_t n);
char *gcs__strdup(const char *src);
char *gcs__strndup(const char *src, size_t n);
uint32_t gcs__strlen(const char *src);
int gcs__strcmp(const char *c1, const char *c2);
int gcs__strncmp(const char *c1, const char *c2, size_t n);

int fexpr_log(FILE *dest, uint32_t ct_expr, 
              uint32_t ct_logical, uint32_t ct_arithmetic);
int fexpr_err(FILE *dest, const char *err_type, 
              const char *desc, const char *expr_fragmt, 
              uint32_t ct_expr, int index);

#define fexpr_ok(dest)  fprintf(dest, "%s%s%s", KGRN_b, "OK.", KNRM)
#define expr_ok()       fexpr_ok(stdout)
#define expr_log(ct_expr, ct_logical, ct_arithmetic) fexpr_log(stdout, ct_expr, ct_logical, ct_arithmetic)
#define expr_error(err_type, desc, expr_fragmt, ct_expr, index) fexpr_err(stderr, err_type, desc, expr_fragmt, ct_expr, index)

typedef struct vector_str vector_str;
#define VECTOR_DEFAULT_CAPACITY 2

struct vector_str {
    struct vector_str_base {
        char **start;
        char **finish;
        char **end_of_storage;
    } impl;

    struct typetable *ttbl;
};

void message_test();
void input_test();

char *expr_tok(char *str, const char *delim);

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, char *argv[]) {
    uint32_t v_len = 0;
    uint32_t v_cap = 16;

    int i = 0;
    char *pos = argv[1];
    uint32_t length = strlen(pos);
    bool first_char = true;

    vector_str *v = NULL;
    char **v_start = NULL;
    char **curr = NULL;

    v = malloc(sizeof *v);
    massert_ptr(v);

    v_start = calloc(VECTOR_DEFAULT_CAPACITY, sizeof *v_start);
    massert_ptr(v_start);
    v->impl.start = v_start;
    v->impl.finish = v->impl.start;
    v->impl.end_of_storage = v->impl.start + VECTOR_DEFAULT_CAPACITY;

    for (i = 0; i < length; i++) {
        if (v->impl.finish == v->impl.end_of_storage) {
            ptrdiff_t sz = ptr_distance(v->impl.start, 
                                        v->impl.finish, 
                                        sizeof *v->impl.start);
            ptrdiff_t cap = ptr_distance(v->impl.start, 
                                         v->impl.end_of_storage, 
                                         sizeof *v->impl.start);

            char **new_start = NULL;
            new_start = realloc(v->impl.start, (cap * 2) * sizeof *v->impl.start);
            massert_ptr(new_start);

            v->impl.start = new_start;
            v->impl.finish = v->impl.start + sz;
            v->impl.end_of_storage = v->impl.start + (cap * 2);
        }

        if (pos[i] == ';' || pos[i] == '"') {
            char *result = NULL;

            if (first_char) {
                if (pos[i] == '"') {
                    first_char = false;
                    ++pos;
                    continue;
                }
            }
            
            result = malloc(strlen(pos) + 1);
            massert_malloc(result);
            strcpy(result, pos);
            result[i] = '\0';

            (*v->impl.finish++) = result;
            pos += (i + 1);
            i = 0;
        } else if (pos[i] == ' ' && first_char) {
            
        }

        first_char == false;
    }

    v_len = ptr_distance(v->impl.start, v->impl.finish, sizeof *v->impl.start);

    curr = v->impl.start;
    i = 0;
    
    while (curr != v->impl.finish) {
        printf("v[%d]: %s\n", i++, (*curr++));
    }

    free(v->impl.start);
    v->impl.start = NULL;
    v->impl.finish = NULL;
    v->impl.end_of_storage = NULL;
    
    free(v);
    v = NULL;

    return EXIT_SUCCESS;
}

char *gcs__strcpy(char *dst, const char *src) {

}

char *gcs__strdup(const char *src) {

}

char *expr_tok(char *str, const char *delim) {
    char *result = "dummy";
    return result;
}

void message_test() {
    bool status = false;
    const char *desc = "operator mismatch";
    const char *err_type = "Parse error";
    char *expr_fragmt = "2 & 3";
    uint32_t ct_expr = 1;

    expr_log(4, 2, 2);
  
    if (status) {
        expr_ok();
    } else {
        expr_error(err_type, desc, expr_fragmt, ct_expr, 2);
    }

}

void input_test(int argc, char *argv[]) {
    uint32_t svec_len = 0;
    uint32_t svec_cap = 16;
    char **svec = NULL;

    int i = 0;
    char *pos = argv[1];
    uint32_t length = strlen(pos);
    bool first_char = true;

    message_test();

    svec = calloc(svec_cap, sizeof *svec);
    massert_calloc(svec);

    for (i = 0; i < length; i++) {
        if (svec_len == svec_cap) {
            char **svec_new = NULL;

            svec_cap *= 2;
            svec_new = realloc(svec, svec_cap * sizeof *svec_new);
            massert_realloc(svec_new);

            svec = svec_new;
        }

        if (pos[i] == ';' || pos[i] == '"') {
            char *result = NULL;
            
            result = malloc(strlen(pos) + 1);
            massert_malloc(result);
            strcpy(result, pos);

            result[i] = '\0';
            svec[svec_len++] = result;

            pos += (i + 1);
            i = 0;
        }

        first_char == false;
    }

    for (i = 0; i < svec_len; i++) {
        printf("svec[%d]: %s\n", i, svec[i]);
    }
    
    for (i = 0; i < svec_len; i++) {
        free(svec[i]);
        svec[i] = NULL;
    }

    free(svec);
    svec = NULL;
}

int fexpr_log(FILE *dest, uint32_t ct_expr, 
              uint32_t ct_logical, uint32_t ct_arithmetic) {
    char buffer[BUFFER_SIZE];
    int j = 0;

    j = sprintf(buffer + j,
                "Found %s%d%s expressions: %s%d%s logical and %s%d%s arithmetic.", 
                KWHT_b, ct_expr, KNRM, 
                KWHT_b, ct_logical, KNRM, 
                KWHT_b, ct_arithmetic, KNRM);

    return fprintf(dest, "%s\n", buffer);
}

int fexpr_err(FILE *dest, const char *err_type, 
                const char *desc, const char *expr_fragmt, 
                uint32_t ct_expr, int index) {
    char buffer[BUFFER_SIZE];
    char spaces[BUFFER_SIZE];
    int i = 0;
    int j = 0;

    strcpy(spaces, "\t");

    if (index > -1) {
        for (i = 1; i <= index + 1; i++) {
            spaces[i] = ' ';
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

/*
int fexpr_err(FILE *dest, const char *err_type, 
                const char *desc, const char *expr_fragmt, uint32_t ct_expr, int index) {
    char buffer[BUFFER_SIZE];
    char spaces[BUFFER_SIZE];
    int i = 0;
    int j = 0;

    strcpy(spaces, "\t");

    if (index > -1) {
        for (i = 1; i <= index + 1; i++) {
            spaces[i] = ' ';
        }
    }

    j += sprintf(buffer + j, KNRM_b"expression:%d:"KNRM""KRED_b" error: "KWHT_b"[%s]"KNRM" %s in\n\t\"%s\"\n", ct_expr, err_type, desc, expr_fragmt);

    if (index > -1) {
        j += sprintf(buffer + j, "%s%s", spaces, KYEL_b"^"KNRM);
    }

    return fprintf(dest, "%s\n", buffer);
}
*/

/**
 *  Utility function for debugging/error messages
 *
 *  @param[in]      dest        stream destination
 *  @param[in]      level       literals "BUG", "ERROR", "WARNING", or "LOG"
 *  @param[in]      file        macro __FILE__ is to be used here (by client)
 *  @param[in]      func        macro __func__ is to be used here
 *  @param[in]      line        macro __LINE__ is to be used here
 *  @param[in]      fmt         formatting to be used
 *
 *  @return         character count of buffer (from fprintf)
 */
int ulog(FILE *dest, const char *level, const char *file, const char *func,
         long double line, const char *fmt, ...) {

    char buffer[MAXIMUM_STACK_BUFFER_SIZE];
    char temp[256];

    const char *color = KNRM;
    const char *blink = "";

    bool found = false;
    bool is_integer = false;
    bool is_currency = *file == '$';

    int j = 0;

    if (streql(level, "[BUG]")) {
        color = KYEL_b;
        found = true;
    }

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

    if (found == false) {
        if (streql(level, "[WARNING]")) {
            color = KMAG_b;
            blink = KBNK;
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
    } else if (ulog_attrs_disable[FILENAME] &&
               ulog_attrs_disable[LINE] == false) {
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
    } else if (ulog_attrs_disable[FILENAME] == false &&
               ulog_attrs_disable[LINE] == false) {
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

    if (ulog_attrs_disable[FUNCTION] == false &&
        ulog_attrs_disable[MESSAGE] == false) {
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
