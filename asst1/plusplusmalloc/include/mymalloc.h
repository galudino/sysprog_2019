/**
 *  @file       mymalloc.h
 *  @brief      Header file for Asst1: ++malloc
 *
 *  @author     Gemuele Aludino
 *  @date       30 Sep 2019
 *  @copyright  Copyright © 2019 Gemuele Aludino
 */
/**
 *  Copyright © 2019 Gemuele Aludino
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, block_merge, publish, distribute, sublicense,
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

#ifndef MY_MALLOC_H
#define MY_MALLOC_H

/*#define MYMALLOC__RELEASE_MODE*/

#define MYMALLOC__LOW_PROFILE
#define MYMALLOC__BLOCK_SIZE    4096

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if __STD_VERSION__ >= 19990L
#include <stdbool.h>
#include <stdint.h>
#else
# define __func__ ":"
# define false  0
# define true   1
typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#if WIN32 || _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>

#define malloc(size) mymalloc(size, __FILE__, __LINE__)
#define free(ptr) myfree(ptr, __FILE__, __LINE__)

/**< mymalloc: memory allocator functions, allocate and free */
void *mymalloc(size_t size, const char *filename, size_t lineno);
void myfree(void *ptr, const char *filename, size_t lineno);

/**< header_t: print allocated blocks to FILE * stream */
void header_fputs(FILE *dest,
                 const char *filename,
                 const char *funcname,
                 size_t lineno);

#ifndef MYMALLOC__RELEASE_MODE
# define listlog()   header_fputs(stdout, __FILE__, __func__, __LINE__)
#else
# define listlog()
#endif 

/**
 *  Logging utilities
 *  (excerpt from utils.h, gcslib)
 */
#define streql(s1, s2) strcmp(s1, s2) == 0
#define strneql(s1, s2, n) strncmp(s1, s2, n) == 0

#define BUFFER_SIZE 256
#define BUFFER_SIZE_4K 4096
#define MAXIMUM_STACK_BUFFER_SIZE 16384

#define KNRM "\x1B[0;0m"   /**< reset to standard color/weight */
#define KNRM_b "\x1B[0;1m" /**< standard color bold */

#define KGRY "\x1B[0;2m" /**< dark grey */

#define KITL "\x1B[0;3m" /**< italics */
#define KULN "\x1B[0;4m" /**< underline */

#define KBNK "\x1B[0;5m" /**< blink every second */
#define KHIL "\x1B[0;7m" /**< grey highlight */

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
    const char *level,     /**< meant for "BUG", "LOG", "ERROR", or "WARNING" */
    const char *file,      /**< meant for use with the __FILE__ macro */
    const char *func,      /**< meant for use with the __func__ macro */
    long double line,      /**< meant for use with the __LINE__ macro */
    const char *fmt, ...); /**< user's custom message */

/**
 *  Unless you would like to create a customized
 *  debugging message, please use the following preprocessor directives.
 *
 *  BUG is suggested for documenting bugs at certain points in your program.
 *  LOG is a general-purpose messaging tool.
 *  ERROR is used for displaying error messages (i.e. something failed, etc)
 *  WARNING is used to notify the client of an impending issue.
 *
 *  The first argument in BUG, LOG, ERROR, and WARNING is FILEMACRO,
 *  which refers to the following macro:
 *          __FILE__
 *  This is just a string, so if you are building a custom ulog message,
 *  with BUG, LOG, ERROR, or WARNING, and would like to put a different
 *  string in place of __FILE__, you may do so.
 */

/**< Designated default streams for BUG, LOG, ERROR, and WARNING */
#define ULOG_STREAM_BUG stderr
#define ULOG_STREAM_LOG stdout
#define ULOG_STREAM_ERROR stderr
#define ULOG_STREAM_WARNING stderr

/**
 *  @def        ULOG_DISABLE_ALL
 *  @brief      Shorthand macro to disable the following preprocessor macros:
 *              BUG, LOG, ERROR, WARNING
 *
 *  Use the preprocessor directive
 *      #define ULOG_DISABLE_ALL
 *  before the inclusion of utils.h (or before any of the directives below)
 *  to disable the macros BUG, LOG, ERROR, and WARNING all at once.
 *
 *  You may also use any combination of these macros to keep some
 *  of the ulog macro types active.
 *
 *  ulog has the following format:
 *  (level is what appears in LEVEL, usually [BUG], [LOG], [ERROR], or [WARNING]
 *  if using the ulog macros. Using the ulog function allows you to customize
 *  your own error message)
 *  MMM dd yyyy HH:mm:ss LEVEL [filepath/filename:linenumber] function_name message
 */
#ifdef ULOG_DISABLE_ALL
# define ULOG_DISABLE_BUG
# define ULOG_DISABLE_LOG
# define ULOG_DISABLE_ERROR
# define ULOG_DISABLE_WARNING
#endif /* ULOG_DISABLE_ALL */

/** Turn off ulog attributes by invoking one or more of these in a function.
ULOG_TOGGLE_ATTR(DATE);
ULOG_TOGGLE_ATTR(TIME);
ULOG_TOGGLE_ATTR(LEVEL);
ULOG_TOGGLE_ATTR(FILENAME);
ULOG_TOGGLE_ATTR(LINE);
ULOG_TOGGLE_ATTR(FUNCTION);
ULOG_TOGGLE_ATTR(MESSAGE);
*/

/**
 *  @def        BUG
 *  @brief      Shorthand macro for ulog to note bugs in a program
 *
 *  Use the preprocessor directive
 *      #define ULOG_DISABLE_BUG
 *  before the inclusion of utils.h (or before these directives)
 *  to disable the BUG macro.
 */
#if __STDC_VERSION__ >= 199901L
# ifndef ULOG_DISABLE_BUG
#  define BUG(FILEMACRO, ...)                                                    \
          ulog(ULOG_STREAM_BUG, "[BUG]", FILEMACRO, __func__, (long int)__LINE__,    \
              __VA_ARGS__)
# else
#  define BUG(FILEMACRO, ...)
# endif /* ULOG_DISABLE_BUG */
#else
# ifndef ULOG_DISABLE_BUG
#  define BUG(FILEMACRO, MSG)                                                    \
          ulog(ULOG_STREAM_BUG, "[BUG]", FILEMACRO, __func__, (long int)__LINE__, MSG)
# else
#  define BUG(FILEMACRO, MSG)
# endif /* ULOG_DISABLE_BUG */
#endif /* __STDC_VERSION__ >= 199901L */

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
# endif /* ULOG_DISABLE_LOG */
#else
# ifndef ULOG_DISABLE_LOG
#  define LOG(FILEMACRO, MSG)                                                    \
          ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__, MSG)
# else
#  define LOG(FILEMACRO, MSG)
# endif /* ULOG_DISABLE_LOG */
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
# ifndef ULOG_DISABLE_ERROR
#  define ERROR(FILEMACRO, ...)                                                  \
          ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__,                    \
          (long int)__LINE__, __VA_ARGS__)
# endif /* ULOG_DISABLE_ERROR */
#else
# ifndef ULOG_DISABLE_ERROR
#  define ERROR(FILEMACRO, MSG)                                                  \
          ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__,                    \
          (long int)__LINE__, MSG)
# else
#  define ERROR(FILEMACRO, MSG)
# endif /* ULOG_DISABLE_ERROR */
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
if (!CONDITION) {\
    fprintf(stderr, "Assertion failed: (%s)\n", #CONDITION);\
    ERROR(__FILE__, (MESSAGE));\
    abort();\
}

#define massert_ptr(PTR);\
massert(PTR, "['"#PTR"' was found to be NULL - '"#PTR"' must be nonnull to continue.]");

#define massert_ttbl(TTBL);\
massert(TTBL, "['"#TTBL"' was found to be NULL -- '"#TTBL"' is mandatory for data type information]");

#define massert_malloc(PTR);\
massert(PTR, "[Request for heap storage allocation failed (malloc returned NULL and was assigned to '"#PTR"')]");

#define massert_calloc(PTR);\
massert(PTR, "[Request for heap storage allocation failed (calloc returned NULL and was assigned to '"#PTR"')]");

#define massert_realloc(PTR);\
massert(PTR, "[Request for heap storage reallocation failed (realloc returned NULL and was assigned to '"#PTR"')]");

#define massert_pfunc(PFUNC);\
massert(PFUNC, "['"#PFUNC"' was found to be NULL - '"#PFUNC"' must be assigned to a function with a matching prototype.]");

#define massert_container(PTR);\
massert(PTR, "['"#PTR"' was found to be NULL - '"#PTR"' must be assigned to the return value of a container initializer function prior to use.]");

#endif /* MY_MALLOC_H */
