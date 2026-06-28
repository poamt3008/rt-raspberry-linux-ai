#ifndef TIME_UTILS_H
#define TIME_UTILS_H

/******************************************************************************
 * Header File
 *
 * Purpose:
 * Public interface for time utility functions.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <time.h>

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/* Add milliseconds to a timespec structure. */
void timespec_add_ms(struct timespec *time,
                     uint32_t milliseconds);

/* Compute the time difference between two timestamps in milliseconds. */
double timespec_diff_ms(struct timespec start,
                        struct timespec end);

/* Convert a timespec structure to milliseconds. */
uint64_t timespec_to_ms(struct timespec time);

#endif