/**
 *  @file       header.h
 *  @brief      Header file for Asst2: Spooky Search
 *
 *  @author     Gemuele Aludino
 *  @date       04 Nov 2019
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

#ifndef HEADER_H
#define HEADER_H

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
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#if WIN32 || _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>

#define elapsed_time_ns(BEF, AFT)                                              \
    (((double)((pow(10.0, 9.0) * AFT.tv_sec) + (AFT.tv_nsec))) -               \
     ((double)((pow(10.0, 9.0) * BEF.tv_sec) + (BEF.tv_nsec))))

#define elapsed_time_ms(BEF, AFT)                                              \
    ((elapsed_time_ns(BEF, AFT) * pow(10.0, -6.0)))
#define elapsed_time_s(BEF, AFT) (elapsed_time_ns(BEF, AFT) * pow(10.0, -9.0))

#define convert_ns_to_s(NS) ((NS) * pow(10.0, -9.0))
#define convert_ns_to_ms(NS) ((NS) * pow(10.0, -6.0))
#define convert_ns_to_mcs(NS) ((NS) * (pow(10.0, -3.0)))

#define MCS "µs"

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

/* randrnge has an inclusive, exclusive ranging: [min, max) */
#define randrnge(min, max) ((rand() % ((max) - (min))) + (min))

ptrdiff_t ptr_distance(const void *beg, const void *end, size_t width);

double mean(double *results, size_t count);
double standard_deviation(double *results, size_t count);

#endif /* HEADER_H */
