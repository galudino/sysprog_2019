/**
 *  @file       source.c
 *  @brief      Source file for Asst2: Spooky Search
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

#include <math.h>

#include "header.h"

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
 *  Return mean from an array of double
 *  
 *  @param[in]  results pointer to double
 *  @param[in]  count   element count of results
 * 
 *  @return sum of all elements in results / count
 */
double mean(double *results, size_t count) {
    size_t i = 0;
    double sum = 0.0;

    if (count == 0) {
        return -1.0;
    }

    for (i = 0; i < count; i++) {
        sum += results[i];
    }

    return (sum / count);
}

/**
 *  Return variance from array of double
 * 
 *  @param[in]  results pointer to double
 *  @param[in]  count element count of results
 * 
 *  @return variance from elements in results
 */
double variance(double *results, size_t count) {
    double avg = 0.0;
    double *results_changed = NULL;
    double sum = 0;

    size_t i = 0;

    if (count == 0) {
        /* sentinel */
        return -123456789.123456789;
    }

    avg = mean(results, count);

    results_changed = calloc(count, sizeof *results_changed);
    assert(results_changed);

    for (i = 0; i < count; i++) {
        results_changed[i] = results[i];
    }

    for (i = 0; i < count; i++) {
        double delta = 0.0;
        double delta_squared = 0.0;

        delta = results_changed[i] - avg;
        delta_squared = (delta * delta);

        results_changed[i] = delta_squared;
    }

    for (i = 0; i < count; i++) {
        sum += results_changed[i];
    }

    return (sum / (count - 1));
}

/**
 *  Return standard deviation from array of double
 * 
 *  @param[in]  results pointer to double
 *  @param[in]  count element count of results
 * 
 *  @return standard deviation from elements in results
 */
double standard_deviation(double *results, size_t count) {
    if (count == 0) {
        /* sentinel */
        return -123456789.123456789;
    }

    return sqrt(variance(results, count));
}
