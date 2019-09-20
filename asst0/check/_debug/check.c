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
# define false  0
# define true   1
typedef unsigned char bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
#endif

#include <unistd.h>

#include <assert.h>
#include <string.h>
#include <strings.h>

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

#define ESC_CHARS "\t\n\v\f\r\" "
#define NULL_TERMINATOR '\0'
#define QUOTATION_MARK '\"'
#define COMMA ','
#define PERIOD '.'
#define DASH '-'

/**< optional typedefs for pointers-to-functions */
typedef bool (*unary_predicate_fn)(const void *);
typedef bool (*binary_predicate_fn)(const void *, const void *);
typedef void (*consumer_fn)(const void *);

typedef void *(*copy_fn)(void *, const void *);
typedef void (*dtor_fn)(void *);
typedef void (*swap_fn)(void *, void *);
typedef int (*compare_fn)(const void *, const void *);
typedef void (*print_fn)(const void *, FILE *);

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

struct typetable *_char_;
struct typetable *_str_;
struct typetable *_void_ptr_;

void *str_copy(void *arg, const void *other);
void str_dtor(void *arg);
void str_swap(void *arg, void *other);
int str_compare(const void *c1, const void *c2);
void str_print(const void *arg, FILE *dest);

void void_ptr_dtor(void *arg);
int void_ptr_compare(const void *c1, const void *c2);
void void_ptr_print(const void *arg, FILE *dest);

/**< utils: debugging */
int ulog(
    FILE *dest,
    const char *level,     /**< meant for "BUG", "LOG", "ERROR", or "WARNING" */
    const char *file,      /**< meant for use with the __FILE__ macro */
    const char *func,      /**< meant for use with the __func__ macro */
    long double line,      /**< meant for use with the __LINE__ macro */
    const char *fmt, ...); /**< user's custom message */

bool ulog_disable[] = {false, false, false, false, false};
bool ulog_attrs_disable[] = {false, false, false, false, false, false, false};

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
 *  @def        BUG
 *  @brief      Shorthand macro for ulog to note bugs in a program
 */
#if __STDC_VERSION__ >= 199901L
#define BUG(FILEMACRO, ...)                                                    \
    ulog(ULOG_STREAM_BUG, "[BUG]", FILEMACRO, __func__, (long int)__LINE__,    \
         __VA_ARGS__)
#else
#define BUG(FILEMACRO, MSG)                                                    \
    ulog(ULOG_STREAM_BUG, "[BUG]", FILEMACRO, __func__, (long int)__LINE__, MSG)
#endif

/**
 *  @def        LOG
 *  @brief      Shorthand macro for ulog to create messages for a program
 */
#if __STDC_VERSION__ >= 199901L
#define LOG(FILEMACRO, ...)                                                    \
    ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__,    \
         __VA_ARGS__)
#else
#define LOG(FILEMACRO, MSG)                                                    \
    ulog(ULOG_STREAM_LOG, "[LOG]", FILEMACRO, __func__, (long int)__LINE__, MSG)
#endif

/**
 *  @def        ERROR
 *  @brief      Shorthand macro for ulog to display errors for a program
 */
#if __STDC_VERSION__ >= 199901L
#define ERROR(FILEMACRO, ...)                                                  \
    ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__,                    \
         (long int)__LINE__, __VA_ARGS__)
#else
#define ERROR(FILEMACRO, MSG)                                                  \
    ulog(ULOG_STREAM_ERROR, "[ERROR]", FILEMACRO, __func__,                    \
         (long int)__LINE__, MSG)
#endif

/**
 *  @def        WARNING
 *  @brief      Shorthand macro for ulog to display warning for a program
 */
#if __STDC_VERSION__ >= 199901L
#define WARNING(FILEMACRO, ...)                                                \
    ulog(ULOG_STREAM_WARNING, "[WARNING]", FILEMACRO, __func__,                \
         (long int)__LINE__, __VA_ARGS__)
#else
#define WARNING(FILEMACRO, MSG)                                                \
    ulog(ULOG_STREAM_WARNING, "[WARNING]", FILEMACRO, __func__,                \
         (long int)__LINE__, MSG)
#endif

#define UTILS_LOG_COUNT 5
extern bool ulog_disable[UTILS_LOG_COUNT];
enum ULOG_TYPE { ALL, BUG, LOG, ERROR, WARNING };

#define ULOG_TOGGLE_TYPE(ULOG_TYPE)                                            \
    (ulog_disable[ULOG_TYPE]) = (ulog_disable[ULOG_TYPE]) ? (false) : (true)

#define UTILS_LOG_ATTRS_COUNT 7
enum ULOG_ATTRS { DATE, TIME, LEVEL, FILENAME, LINE, FUNCTION, MESSAGE };
extern bool ulog_attrs_disable[UTILS_LOG_ATTRS_COUNT];

#define ULOG_TOGGLE_ATTR(ULOG_ATTR)                                            \
    ulog_attrs_disable[ULOG_ATTR] =                                            \
        (ulog_attrs_disable[ULOG_ATTR]) ? (false) : (true);

/**
 *  When using the ULOG toggle macros below:
 *
 *  Copy and paste these directives into a function (like main(), or in a
 *  function where you would like the toggle to occur, for example)
 *  and uncomment to toggle any of the ulog types and/or attributes on/off.
 *
 *  (Tip: instead of toggling all of these off to turn off
 *        all utils_log appearances, use the
 *              ULOG_TOGGLE_TYPE(ALL)
 *         directive instead, to disable all utils_log appearances.)
 *
 *  All toggles are OFF (false) by default.
 *  Uncommenting the macro will set the respective type/attribute to true.
 *  Invoking the directive again will undo the previous action.
 */

/**
ULOG_TOGGLE_TYPE(ALL);
ULOG_TOGGLE_TYPE(BUG);
ULOG_TOGGLE_TYPE(LOG);
ULOG_TOGGLE_TYPE(ERROR);
ULOG_TOGGLE_TYPE(WARNING);

ULOG_TOGGLE_ATTR(DATE);
ULOG_TOGGLE_ATTR(TIME);
ULOG_TOGGLE_ATTR(LEVEL);
ULOG_TOGGLE_ATTR(FILENAME);
ULOG_TOGGLE_ATTR(LINE);
ULOG_TOGGLE_ATTR(FUNCTION);
ULOG_TOGGLE_ATTR(MESSAGE);
*/

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

/**
 *  Pointer utilities
 */
ptrdiff_t ptr_distance(const void *beg, const void *end, size_t width);

/**< C-String functions/macros */
char *str_trim_left(char *to_trim, const char *charset);
char *str_trim_right(char *to_trim, const char *charset);
char *str_trim(char *to_trim, const char *charset);

#define streql(s1, s2) strcmp(s1, s2) == 0
#define strneql(s1, s2, n) strncmp(s1, s2, n) == 0

#if __linux__ && !__POSIX__
#define strdup(str)         strcpy(malloc(strlen(str) + 1), str)
#define strndup(str, n)     strcpy(malloc(strlen(str + n) + 1), (str + n));
#endif

/**
 *  @typedef    iterator
 *  @brief      Alias for (struct iterator)
 */
typedef struct iterator iterator;

/**
 *  @typedef    iterator_table
 *  @brief      Alias for (struct iterator_table)
 */
typedef struct iterator_table iterator_table;

/**
 *  @struct     iterator
 *  @brief      Abstraction for a container
 */
struct iterator {
    struct iterator_table *itbl;         /**< common function interface */
    void *container;                     /**< data structure, for begin, end */
    void *curr;    /**< current position (address of a block, or node, etc.) */
};

/**
 *  @struct     iterator_table
 *  @brief      Determines the functionality of an iterator,
 *              specific to a particular container type
 */
struct iterator_table {
    iterator    (*begin)       (void *);
    iterator    (*end)         (void *);

    iterator    (*next)        (iterator);
    iterator    (*next_n)      (iterator, int);

    iterator    (*prev)        (iterator);
    iterator    (*prev_n)      (iterator, int);

    iterator *  (*advance)     (iterator *, int);
    iterator *  (*incr)        (iterator *);
    iterator *  (*decr)        (iterator *);

    void *      (*curr)        (iterator);
    void *      (*start)       (iterator);
    void *      (*finish)      (iterator);

    int         (*distance)    (iterator *, iterator *);

    bool        (*has_next)    (iterator);
    bool        (*has_prev)    (iterator);

    struct typetable * (*get_ttbl)(void *);
};

/**< iterator: begin/end (returns new iterator) */
iterator it_begin(iterator it);
iterator it_end(iterator it);

/**< iterator: next/next_n (returns new iterator) */
iterator it_next(iterator);
iterator it_next_n(iterator it, int n);

/**< iterator: prev/prev_n  (returns new iterator) */
iterator it_prev(iterator);
iterator it_prev_n(iterator it, int n);

/**< iterator: advance/incr/decr (mutates iterator) */
iterator *it_advance(iterator *it, int n);
iterator *it_incr(iterator *it);
iterator *it_decr(iterator *it);

/**< iterator: curr/start/finish (retrieve elements) */
void *it_curr(iterator it);
void *it_start(iterator it);
void *it_finish(iterator it);

/**< iterator: calculate distance */
int it_distance(iterator *first, iterator *last);

/**< iterator: determine if end of iteration reached */
bool it_has_next(iterator it);
bool it_has_prev(iterator it);

/**< iterator: retrieve typetable */
struct typetable *it_get_ttbl(iterator it);

#define massert_iterator(ITER)\
massert(ITER, "['"#ITER"' was found to be NULL - '"#ITER"' must point to an initialized iterator, such that it refers to a non-null pointer-to-container.]");

/**
 *  @def        VECTOR_DEFAULT_CAPACITY
 *  @brief      Default capacity for vector's internal buffer
 */
#define VECTOR_DEFAULT_CAPACITY 16

/**
 *  @typedef    vector
 *  @brief      Alias for (struct vector)
 *
 *  All instances of (struct vector) will be addressed as (vector).
 */
typedef struct vector vector;

/**
 *  @typedef    vector_ptr
 *  @brief      Alias for (struct vector *) or (vector *)
 *
 *  All instances of (struct vector *) will be addressed as (vector *).
 *
 *  This typedef is to be used only for macros that perform token-pasting.
 *  (such macros require input types that contain spaces and/or asterisks
 *   be aliased into a single word)
 */
typedef struct vector *vector_ptr;

/**
 *  @typedef    vector_dptr
 *  @brief      Alias for (struct vector **) or (vector **)
 *
 *  All instances of (struct vector **) will be addressed as (vector **).
 *
 *  This typedef is to be used only for macros that perform token-pasting.
 *  (such macros require input types that contain spaces and/or asterisks
 *    be aliased into a single word)
 */
typedef struct vector **vector_dptr;

/**
 *      By default, elements are deep copied into the containers,
 *      iff the typetable provided upon instantiation has a copy function
 *      that performs a deep copy of its argument.
 *
 *      If there is no deep copy function provided in the typetable,
 *      elements are shallow copied.
 *
 *      "fill" functions always create deep copies of elements when used.
 *      (provided a deep copy function is defined in the typetable)
 */

/**< vector: allocate and construct */
vector *v_new(struct typetable *ttbl);
vector *v_newr(struct typetable *ttbl, size_t n);
/** v_newfill, v_newrnge, v_newcopy, v_newmove omitted */

/**< vector: destruct and deallocate */
void v_delete(vector **v);

/**< vector: iterator functions */
iterator v_begin(vector *v);
iterator v_end(vector *v);

/**< vector: length functions */
size_t v_size(vector *v);

/**< vector: resize functions */
void v_resize(vector *v, size_t n);
/** v_resizefill omitted */

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

/**< vector: element access functions w/ const qualifier */
const void *v_at_const(vector *v, size_t n);
const void *v_front_const(vector *v);
const void *v_back_const(vector *v);
const void *v_data_const(vector *v);

/**< vector: modifiers - assignment */
/** v_assignrnge, vassignfill omitted */

/**< vector: modifiers - push/pop */
void v_pushb(vector *v, const void *valaddr);
void v_popb(vector *v);

/**< vector: modifiers - insertion */
/** v_insert, v_insertfill, v_insertrnge, v_insertmove omitted */

/**< vector: modifiers - erasure */
/** v_erase, v_erasernge omitted */

/**< vector: modifiers - container swappage */
/** v_swap omitted */

/**< vector: modifiers - clear container */
void v_clear(vector *v);

/**< vector: custom modifiers - insertion/erasure/replacement by index */
/** v_insert_at, v_erase_at, v_replace_at omitted */

/**< vector: custom modifiers - element swappage */
/** v_swap_elem omitted */

/**< vector: custom modifiers - element removal by search */
/** v_remove, v_remove_if omitted */

/**< vector: custom modifiers - merge/reverse */
/**< v_merge, v_reverse omitted */

/**< vector: custom utility functions - arrtov / ptrtov */
/** v_arrtov, v_ptrtov omitted */

/**< vector: custom utility functions - search / sort by default comparator */
/** v_search, v_sort omitted */

/**< vector: custom print functions - output to FILE stream */
void v_puts(vector *v);

void v_putsf(vector *v, const char *before, const char *after,
             const char *postelem, const char *empty, size_t breaklim);

void v_fputs(vector *v, FILE *dest);

void v_fputsf(vector *v, FILE *dest, const char *before, const char *after,
              const char *postelem, const char *empty, size_t breaklim);

/**< vector: required function prototypes for (struct typetable) */
/**< vector_copy, vector_dtor, vector_swap, vector_compare, 
     vector_print omitted */

/**< vector: change typetable */
/**< v_set_ttbl omitted */

/**< vector: retrieve width/copy/dtor/swap/compare/print/typetable */
/**< v_get_width, v_get_copy, v_get_dtor, v_get_swap, v_get_compare, 
     v_get_print, v_get_ttbl omitted */


/**< ptrs to vtables */
/** struct typetable *_vector_ omitted */
extern struct iterator_table *_vector_iterator_;

/**< client program functions */
int fexpr_log(FILE *dest, uint32_t ct_expr, 
              uint32_t ct_logical, uint32_t ct_arithmetic) {

}

int fexpr_err(FILE *dest, const char *err_type, 
                const char *desc, const char *expr_fragmt, uint32_t ct_expr) {

}

#define expr_log(ct_expr, ct_logical, ct_arithmetic) fexpr_log(stdout, ct_expr, ct_logical, ct_arithmetic)
#define expr_error(err_type, desc, expr_fragmt, ct_expr) fexpr_err(stderr, err_type, desc, expr_fragmt, ct_expr)

/**
 *  @brief  Program execution begins here
 *
 *  @param[in]  argc    argument count
 *  @param[in]  argv    command line arguments
 *
 *  @return     exit status
 */
int main(int argc, char *argv[]) {
    char *input = NULL;
    char ch = ' ';
    char *pos = NULL;
    vector *v = NULL;
    iterator it = { NULL, NULL, NULL };
    char **curr = NULL;
    char **sentinel = NULL;
    char *clone = NULL;

    input = argv[1];

    pos = input;

    v = v_new(_str_);

    input = (*input) == '\"' ? (input + 1) : (input);

    pos = strtok(input, ";");
    v_pushb(v, &pos);
    input += strlen(pos) + 1;

    while ((pos = strtok(input, ";")) != NULL) {
        if (streql(pos, "\"") || pos[strlen(pos)] == '"') {
            break;
        }

        v_pushb(v, &pos);
        input += strlen(pos) + 1;
    }

    it = v_begin(v);
    sentinel = it_finish(it);
    while ((curr = it_curr(it)) != sentinel) {
        str_trim((*curr), NULL);
        it_incr(&it);
    }



    v_puts(v);

    return EXIT_SUCCESS;
}

#include <assert.h>
#include <math.h>
#include <string.h>

#define VECTOR_MAXIMUM_STACK_BUFFER_SIZE 16384
#define VECTOR_DEFAULT_CAPACITY          16

/**< optional macros for accessing the innards of vector_base */
#define AT(VEC, INDEX)      ((char *)(VEC->impl.start) + ((INDEX) * (VEC->ttbl->width)))
#define FRONT(VEC)          VEC->impl.start
#define BACK(VEC)           ((char *)(VEC->impl.finish) - (VEC->ttbl->width))
#define END(VEC)            VEC->impl.end_of_storage

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

static vector *v_allocate(void);
static void v_init(vector *v, struct typetable *ttbl, size_t capacity);
static void v_deinit(vector *v);
static void v_swap_addr(vector *v, void *first, void *second);

/**< struct typetable ttbl_vector, struct typetable *_vector_ omitted */

static iterator vi_begin(void *arg);
static iterator vi_end(void *arg);

static iterator vi_next(iterator it);
static iterator vi_next_n(iterator it, int n);

static iterator vi_prev(iterator it);
static iterator vi_prev_n(iterator it, int n);

static int vi_distance(iterator *first, iterator *last);

static iterator *vi_advance(iterator *it, int n);
static iterator *vi_incr(iterator *it);
static iterator *vi_decr(iterator *it);

static void *vi_curr(iterator it);
static void *vi_start(iterator it);
static void *vi_finish(iterator it);

static bool vi_has_next(iterator it);
static bool vi_has_prev(iterator it);

static struct typetable *vi_get_ttbl(void *arg);

struct iterator_table itbl_vector = {
    vi_begin,
    vi_end,
    vi_next,
    vi_next_n,
    vi_prev,
    vi_prev_n,
    vi_advance,
    vi_incr,
    vi_decr,
    vi_curr,
    vi_start,
    vi_finish,
    vi_distance,
    vi_has_next,
    vi_has_prev,
    vi_get_ttbl
};

struct iterator_table *_vector_iterator_ = &itbl_vector;

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
    v_init(v, ttbl, VECTOR_DEFAULT_CAPACITY);   /* construct */
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
 *  @brief  Returns an iterator that points to the first element in vector
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     iterator that refers to v
 */
iterator v_begin(vector *v) {
    massert_container(v);
    return vi_begin(v);
}

/**
 *  @brief  Returns an iterator that pointers to an element that is one block
 *          past the last element in v
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     iterator that refers to v
 */
iterator v_end(vector *v) {
    massert_container(v);
    return vi_end(v);
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
 *  @brief  Retrieves the address of an element from vector at index n
 *
 *  @param[in]  v   pointer to vector
 *  @param[in]  n   index of desired element
 *
 *  @return     address of element at n, const qualified
 *
 *  A bounds check is performed to ensure that n is a valid index.
 *
 *  Note that the address (pointer) of the element type is returned,
 *  be sure to cast the return of v_at to the pointer type of
 *  the element, then dereference the casted v_at to yield
 *  the element itself.
 */
const void *v_at_const(vector *v, size_t n) {
    void *target = NULL;

    massert_container(v);

    if (n >= v_size(v)) {
        char str[256];
        sprintf(str, "Input %lu is greater than vector's logical length, %lu -- index out of bounds.", n, v_size(v));
        ERROR(__FILE__, str);
        return NULL;
    }

    target = (char *)(v->impl.start) + (n * v->ttbl->width);
    return target ? target : NULL;
}

/**
 *  @brief  Retrieves the address of the front element from vector
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     address of front element, const qualified
 *
 *  Note that the address (pointer) of the element type is returned,
 *  be sure to cast the return of v_front to the pointer type of
 *  the element, then dereference the casted v_front to yield
 *  the element itself.
 */
const void *v_front_const(vector *v) {
    massert_container(v);
    return v->impl.start;
}

/**
 *  @brief  Retrieves the address of the rear element from vector
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     address of rear element, const qualified
 *
 *  Note that the address (pointer) of the element type is returned,
 *  be sure to cast the return of v_back to the pointer type of
 *  the element, then dereference the casted v_back to yield
 *  the element itself.
 */
const void *v_back_const(vector *v) {
    massert_container(v);
    return (char *)(v->impl.finish) - (v->ttbl->width);
}

/**
 *  @brief  Retrieves the address of vector's buffer
 *
 *  @param[in]  v   pointer to vector
 *
 *  @return     address of v->impl.start, const qualified
 */
const void *v_data_const(vector *v) {
    massert_container(v);
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
        memcpy(v->impl.finish, valaddr, v->ttbl->width);
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
        memset(v->impl.start, 0, v_size(v) * v->ttbl->width);
        /* v->impl.finish is already at v->impl.start. */
    } else {
        /**
         *  If elements were shallow copied,
         *  (no dtor function specified in ttbl) --
         *  we just memset and reset the finish pointer.
         */
        memset(v->impl.start, 0, v_size(v) * v->ttbl->width);
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

    print = v->ttbl->print ? v->ttbl->print : void_ptr_print;

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

    v->ttbl = ttbl ? ttbl : _void_ptr_;

    if (v->ttbl != _void_ptr_) {
        v->ttbl->compare = v->ttbl->compare ? v->ttbl->compare : NULL;
        v->ttbl->copy = v->ttbl->copy ? v->ttbl->copy : NULL;
        v->ttbl->print = v->ttbl->print ? v->ttbl->print : NULL;
        v->ttbl->dtor = v->ttbl->dtor ? v->ttbl->dtor : NULL;
    }

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
    memcpy(temp, first, v->ttbl->width);

    memcpy(first, second, v->ttbl->width);
    memcpy(second, temp, v->ttbl->width);

    free(temp);
    temp = NULL;
}

/**
 *  @brief  Initializes and returns an iterator that refers to arg
 *
 *  @param[in]  arg     pointer to vector
 *
 *  @return     iterator that refers to v,
 *              position is at v's first element
 */
static iterator vi_begin(void *arg) {
    vector *v = NULL;
    iterator it;

    v = (vector *)(arg);

    it.itbl = _vector_iterator_;
    it.container = v;
    it.curr = v->impl.start;

    return it;
}

/**
 *  @brief  Initializes and returns an iterator that refers to arg
 *
 *  @param[in]  arg     pointer to vector
 *
 *  @return     iterator that refers to v;
 *              position is at one block past v's last element
 */
static iterator vi_end(void *arg) {
    vector *v = NULL;
    iterator it;

    v = (vector *)(arg);

    it.itbl = _vector_iterator_;
    it.container = v;
    it.curr = v->impl.finish;

    return it;
}

/**
 *  @brief  Initializes and returns an iterator that
 *          is one block past it's current position
 *
 *  @param[in]  it      iterator that refers to a vector
 *
 *  @return     a new iterator that is one block past it's current position
 */
static iterator vi_next(iterator it) {
    vector *v = NULL;
    iterator iter;

    v = (vector *)(it.container);
 
    iter.itbl = _vector_iterator_;
    iter.container = v;
    iter.curr = it.curr;

    if (iter.curr == v->impl.finish) {
        ERROR(__FILE__, "Cannot advance - iterator already at end.");
    } else {
        iter.curr = (char *)(v->impl.finish) + (v->ttbl->width);
    }

    return iter;
}

/**
 *  @brief  Initializes and returns an iterator that
 *          is n blocks past it's current position
 *
 *  @param[in]  it      iterator that refers to a vector
 *
 *  @return     a new iterator that is n block's past it's current position
 */
static iterator vi_next_n(iterator it, int n) {
    vector *v = NULL;
    iterator iter;
    int pos = 0;

    v = (vector *)(it.container);

    iter.itbl = _vector_iterator_;
    iter.container = v;
    iter.curr = it.curr;

    pos = ptr_distance(v->impl.start, iter.curr, v->ttbl->width);

    if ((v_size(v) - pos) <= 0) {
        char str[256];
        sprintf(str, "Cannot advance %d times from position %d.", n, pos);
        ERROR(__FILE__, str);
    } else {
        iter.curr = (char *)(iter.curr) + (n * v->ttbl->width);
    }

    return iter;
}

/**
 *  @brief  Initializes and returns an iterator that
 *          is one block behind it's current position
 *
 *  @param[in]  it      iterator that refers to a vector
 *
 *  @return     a new iterator that is one block behind it's current position
 */
static iterator vi_prev(iterator it) {
    vector *v = NULL;
    iterator iter;

    v = (vector *)(it.container);

    iter.itbl = _vector_iterator_;
    iter.container = v;
    iter.curr = it.curr;

    if (iter.curr == v->impl.start) {
        ERROR(__FILE__, "Cannot retract - already at begin.");
    } else {
        iter.curr = (char *)(v->impl.finish) - (v->ttbl->width);
    }

    return iter;
}

/**
 *  @brief  Initializes and returns an iterator that
 *          is n blocks behind it's current position
 *
 *  @param[in]  it      iterator that refers to a vector
 *
 *  @return     a new iterator that is n block's behind it's current position
 */
static iterator vi_prev_n(iterator it, int n) {
    vector *v = NULL;
    iterator iter;
    int pos = 0;

    v = (vector *)(it.container);

    iter.itbl = _vector_iterator_;
    iter.container = v;
    iter.curr = it.curr;

    pos = ptr_distance(v->impl.start, iter.curr, v->ttbl->width);

    if ((v_size(v) - pos) <= 0) {
        char str[256];
        sprintf(str, "Cannot retract %d times from position %d.", n, pos);
        ERROR(__FILE__, str);
    } else {
        iter.curr = (char *)(iter.curr) + (n * v->ttbl->width);
    }

    return iter;
}

/**
 *  @brief  Determines the distance between first and last numerically
 *
 *  @param[in]  first   pointer to iterator that refers to a vector
 *  @param[in]  last    pointer to iterator that refers to a vector
 *
 *  @return     numerical distance between first and last
 *
 *  This function can also be used to determine the numerical position
 *  of a particular iterator, since indices are not stored within iterators.
 *
 *  To find the index position of an iterator, leave one of the parameters
 *  NULL when calling the distance function.
 */
static int vi_distance(iterator *first, iterator *last) {
    vector *v = NULL;

    if (first == NULL && last != NULL) {
        v = (vector *)(last->container);
        return (int)(ptr_distance(v->impl.start, last->curr, v->ttbl->width));
    } else if (last == NULL && first != NULL) {
        v = (vector *)(first->container);
        return (int)(ptr_distance(v->impl.start, first->curr, v->ttbl->width));
    } else if (first == NULL && last == NULL) {
        ERROR(__FILE__, "Both iterator first and last are NULL.");
        return 0;
    } else {
        v = (vector *)(first->container);
        return (int)(ptr_distance(first->curr, last->curr, v->ttbl->width));
    }
}

/**
 *  @brief  Advances the position of it n blocks
 *
 *  @param[in]  it      pointer to iterator that refers to a vector
 *  @param[in]  n       desired amount of blocks to move
 *
 *  @return     pointer to iterator
 */
static iterator *vi_advance(iterator *it, int n) {
    vector *v = NULL;
    int pos = 0;

    massert_iterator(it);

    pos = ptr_distance(v->impl.start, it->curr, v->ttbl->width);

    if ((v_size(v) - pos) < 0) {
        char str[256];
        sprintf(str, "Cannot advance %d times from position %d.", n, pos);
        ERROR(__FILE__, str);
    } else {
        v = (vector *)(it->container);
        it->curr = (char *)(it->curr) + (n * v->ttbl->width);
    }

    return it;
}


/**
 *  @brief  Increments the position of it 1 block forward
 *
 *  @param[in]  it     pointer to iterator that refers to a vector
 *
 *  @return     pointer to iterator
 */
static iterator *vi_incr(iterator *it) {
    vector *v = NULL;

    massert_iterator(it);

    v = (vector *)(it->container);

    if (it->curr == v->impl.finish) {
        ERROR(__FILE__, "Cannot increment - already at end.");
    } else {
        it->curr = (char *)(it->curr) + (v->ttbl->width);
    }

    return it;
}

/**
 *  @brief  Decrements the position of it 1 block backward
 *
 *  @param[in]  it     pointer to iterator that refers to a vector
 *
 *  @return     pointer to iterator
 */
static iterator *vi_decr(iterator *it) {
    vector *v = NULL;

    massert_iterator(it);

    v = (vector *)(it->container);

    if (it->curr == v->impl.start) {
        ERROR(__FILE__, "Cannot decrement this iterator, already at begin.");
    } else {
        it->curr = (char *)(it->curr) - (v->ttbl->width);
    }

    return it;
}

/**
 *  @brief  Retrieves the address of the value referred to
 *          by it's current position
 *
 *  @param[in]  it  iterator that refers to a vector
 *
 *  @return     address of an element from within a vector
 */
static void *vi_curr(iterator it) {
    return it.curr;
}

/**
 *  @brief  Retrieves the address of first element from
 *          the vector it is iterating
 *
 *  @param[in]  it  iterator that refers to a vector
 *
 *  @return     address of the first element from within a vector
 */
static void *vi_start(iterator it) {
    vector *v = (vector *)(it.container);
    return v->impl.start;
}

/**
 *  @brief  Retrieves the address of the block one past
 *          the last element from within the vector it is iterating
 *
 *  @param[in]  it  iterator that refers to a vector
 *
 *  @return     address of the element that is one block past
 *              the rear element from within the vector being iterated
 */
static void *vi_finish(iterator it) {
    vector *v = (vector *)(it.container);
    return v->impl.finish;
}

/**
 *  @brief  Determines if it has elements to visit in the forward direction
 *
 *  @param[in]  it  iterator that refers to a vector
 *
 *  @return     true if elements remain in the forward direction,
 *              false otherwise
 */
static bool vi_has_next(iterator it) {
    vector *v = (vector *)(it.container);
    return it.curr != v->impl.finish;
}

/**
 *  @brief  Determines if it has elements to visit in the backward direction
 *
 *  @param[in]  it  iterator that refers to a vector
 *
 *  @return     true if elements remain in the backward direction,
 *              false otherwise
 */
static bool vi_has_prev(iterator it) {
    vector *v = (vector *)(it.container);
    return it.curr != v->impl.start;
}

/**
 *  @brief  Retrieve a container's typetable
 *
 *  @param[in]  arg     pointer to vector
 *
 *  @return     pointer to typetable
 */
static struct typetable *vi_get_ttbl(void *arg) {
    vector *v = (vector *)(arg);
    return v->ttbl;
}

/**
 *  @brief  Returns an iterator to the beginning of a container
 *          using its itbl
 *
 *  @param[in]  it  iterator representing a container
 *
 *  @return     iterator at the beginning of a container
 */
iterator it_begin(iterator it) {
    massert_ptr(it.itbl->begin);
    return it.itbl->begin(it.container);
}

/**
 *  @brief  Returns an iterator to the end of a container
 *          using its itbl
 *
 *  @param[in]  it  iterator representing a container
 *
 *  @return     iterator at the end of a container
 */
iterator it_end(iterator it) {
    massert_ptr(it.itbl->end);
    return it.itbl->end(it.container);
}

/**
 *  @brief  Returns a new iterator that is one block past
 *          it's current position
 *
 *  @param[in]  it  iterator representing a container
 *
 *  @return     iterator that is one block past it's position
 */
iterator it_next(iterator it) {
    return it.itbl->next(it);
}

/**
 *  @brief  Returns a new iterator that is n blocks past
 *          it's current position
 *
 *  @param[in]  it  iterator representing a container
 *
 *  @return     iterator that is n blocks past it's current position
 */
iterator it_next_n(iterator it, int n) {
    return it.itbl->next_n(it, n);
}

/**
 *  @brief  Returns a new iterator that is one block behind
 *          it's current position
 *
 *  @param[in]  it  iterator representing a container
 *
 *  @return     iterator that is one block behind it's current position
 */
iterator it_prev(iterator it) {
    return it.itbl->prev(it);
}

/**
 *  @brief  Returns a new iterator that is n blocks behind
 *          it's current position
 *
 *  @param[in]  it  iterator representing a container
 *
 *  @return     iterator that is n blocks behind it's current position
 */
iterator it_prev_n(iterator it, int n) {
    return it.itbl->prev_n(it, n);
}

/**
 *  @brief  Advances/retracts iterator n blocks
 *
 *  @param[in]  it  pointer to iterator
 *
 *  @return     same pointer to iterator, but advanced n blocks
 */
iterator *it_advance(iterator *it, int n) {
    assert(it);
    return it->itbl->advance(it, n);
}

/**
 *  @brief  Advances it one block (goes forward)
 *
 *  @param[in]  it  pointer to iterator
 *
 *  @return     same pointer to iterator, but advanced one block
 */
iterator *it_incr(iterator *it) {
    massert_iterator(it);
    massert_pfunc(it->itbl->incr);
    return it->itbl->incr(it);
}

/**
 *  @brief  Retracts it one block (goes backward)
 *
 *  @param[in]  it  pointer to iterator
 *
 *  @return     same pointer to iterator, but advanced one block
 */
iterator *it_decr(iterator *it) {
    massert_iterator(it);
    massert_pfunc(it->itbl->decr);
    return it->itbl->decr(it);
}

/**
 *  @brief  Returns address of element at current position
 *
 *  @param[in]  it  iterator that represents a container
 *
 *  @return     address of element at current position
 */
void *it_curr(iterator it) {
    return it.itbl->curr(it);
}

/**
 *  @brief  Returns address of element at beginning of iterator
 *
 *  @param[in]  it  iterator that represents a container
 *
 *  @return     address of element at beginning of container
 */
void *it_start(iterator it) {
    return it.itbl->start(it);
}

/**
 *  @brief  Returns address of element at end of iterator
 *
 *  @param[in]  it  iterator that represents a container
 *
 *  @return     address of element at end of container
 */
void *it_finish(iterator it) {
    return it.itbl->finish(it);
}

/**
 *  @brief  Determines the distance between first and last numerically
 *
 *  @param[in]  first   pointer to iterator
 *  @param[in]  last    pointer to iterator
 *
 *  @return     numerical distance between first and last
 *
 *  This function can also be used to determine the numerical position
 *  of a particular iterator, since indices are not stored within iterators.
 *
 *  To find the index position of an iterator, leave one of the parameters
 *  NULL when calling the distance function.
 */
int it_distance(iterator *first, iterator *last) {
    if (first == NULL && last != NULL) {
        return last->itbl->distance(NULL, last);
    } else if (last == NULL && first != NULL) {
        return first->itbl->distance(first, NULL);
    } else if (first == NULL && last == NULL) {
        return 0;
    } else if (first->itbl != last->itbl) {
        return 0;
    } else {
        if (first->itbl != last->itbl) {
            return 0;
        } else {
            return first->itbl->distance(first, last);
        }
    }
}

/**
 *  @brief  Determines if it has elements to visit in the forward direction
 *
 *  @param[in]  it  iterator that refers to a container
 *
 *  @return     true if elements remain in the forward direction,
 *              false otherwise
 */
bool it_has_next(iterator it) {
    massert_pfunc(it.itbl->has_next);
    return it.itbl->has_next(it);
}

/**
 *  @brief  Determines if it has elements to visit in the backward direction
 *
 *  @param[in]  it  iterator that refers to a container
 *
 *  @return     true if elements remain in the backward direction,
 *              false otherwise
 */
bool it_has_prev(iterator it) {
    massert_pfunc(it.itbl->has_prev);
    return it.itbl->has_prev(it);
}

/**
 *  @brief  Retrieve a container's typetable
 *
 *  @param[in]  it  iterator that refers to a container
 *
 *  @return     pointer to typetable
 */
struct typetable *it_get_ttbl(iterator it) {
    return it.itbl->get_ttbl(it.container);
}

void *str_copy(void *arg, const void *other) {
    char **target = (char **)(arg);
    char **source = (char **)(other);

    /*
    (*target) = strdup((*source));
    return (*target);
    */

    (*target) = strcpy(malloc(strlen((*source)) + 1), (*source));
    return (*target) ? (*target) : NULL;
}

void *void_ptr_copy(void *arg, const void *other) {
    void **source = (void **)(other);
    return (*source);
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

void void_ptr_dtor(void *arg) { /* DO NOT CHANGE - empty body */
}

void str_swap(void *arg, void *other) {
    char **dest = (char **)(arg);
    char **source = (char **)(other);

    if ((*dest)) {
        char *temp = strcpy(malloc(strlen((*dest)) + 1), (*dest));
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
    char cfirst[strlen(first) + 1];
    char csecond[strlen(second) + 1];
#else
    char *cfirst = malloc(strlen(first) + 1);
    char *csecond = malloc(strlen(second) + 1);
    massert_malloc(cfirst);
    massert_malloc(second);
#endif

    strcpy(cfirst, first);
    strcpy(csecond, second);

    str_trim(cfirst, NULL);
    str_trim(csecond, NULL);

    result = strcmp(cfirst, csecond);

#if __STD_VERSION__ >= 199901L
    free(cfirst);
    cfirst = NULL;

    free(csecond);
    csecond = NULL;
#endif

    return result;
}

int void_ptr_compare(const void *c1, const void *c2) {
    const char *format = "%d";

    char pointer1[MAXIMUM_STACK_BUFFER_SIZE];
    char pointer2[MAXIMUM_STACK_BUFFER_SIZE];

    sprintf(pointer1, format, c1);
    sprintf(pointer2, format, c2);

    return atoi(pointer1) - atoi(pointer2);
}

void str_print(const void *arg, FILE *dest) {
    fprintf(dest, "%s", *(char **)arg);
}

void void_ptr_print(const void *arg, FILE *dest) {
    fprintf(dest, "%p", arg);
}

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

    if (ulog_disable[ALL] ||
        (ulog_attrs_disable[DATE] && ulog_attrs_disable[TIME] &&
         ulog_attrs_disable[LEVEL] && ulog_attrs_disable[FILENAME] &&
         ulog_attrs_disable[LINE] && ulog_attrs_disable[FUNCTION] &&
         ulog_attrs_disable[MESSAGE]) ||
        dest == NULL || level == NULL || file == NULL || func == NULL ||
        line == -666 || fmt == NULL) {
        return 0;
    }

    if (streql(level, "[BUG]")) {
        if (ulog_disable[BUG]) {
            return 0;
        }

        color = KYEL_b;
        found = true;
    }

    if (found == false) {
        if (streql(level, "[LOG]")) {
            if (ulog_disable[LOG]) {
                return 0;
            }

            color = KCYN_b;
            found = true;
        }
    }

    if (found == false) {
        if (streql(level, "[ERROR]")) {
            if (ulog_disable[ERROR]) {
                return 0;
            }

            color = KRED_b;
            blink = KBNK;
            found = true;
        }
    }

    if (found == false) {
        if (streql(level, "[WARNING]")) {
            if (ulog_disable[WARNING]) {
                return 0;
            }

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

char *str_trim_left(char *to_trim, const char *charset) {
    size_t trim_length = 0;

    if (strcmp(to_trim, "") == 0) {
        return to_trim;
    }

    if (charset == NULL) {
        charset = ESC_CHARS; /**< "\t\n\v\f\r\" */
    }

    trim_length = strspn(to_trim, charset);

    if (trim_length > 0) {
        size_t str_length = strlen(to_trim);

        if (trim_length == str_length) {
            to_trim[0] = NULL_TERMINATOR;
        } else {
            memmove(to_trim, to_trim + trim_length,
                    str_length + 1 - trim_length);
        }
    }

    return to_trim;
}

char *str_trim_right(char *to_trim, const char *charset) {
    size_t i = 0;

    if (strcmp(to_trim, "") == 0) {
        return to_trim;
    }

    if (charset == NULL) {
        charset = ESC_CHARS; /**< "\t\n\v\f\r\" */
    }

    i = strlen(to_trim) - 1;

    while (strchr(charset, to_trim[i]) != NULL) {
        to_trim[i] = NULL_TERMINATOR;
        i--;
    }

    return to_trim;
}

char *str_trim(char *to_trim, const char *charset) {
    if (strcmp(to_trim, "") == 0) {
        return to_trim;
    }

    return str_trim_left(str_trim_right(to_trim, charset), charset);
}

struct typetable ttbl_str = {
    sizeof(char *),
    str_copy,
    str_dtor,
    str_swap,
    str_compare,
    str_print
};

struct typetable *_str_ = &ttbl_str;

struct typetable ttbl_void_ptr = {
    sizeof(void *),
    void_ptr_copy,
    void_ptr_dtor,
    NULL,
    void_ptr_compare,
    void_ptr_print
};

struct typetable *_void_ptr_ = &ttbl_void_ptr;

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
