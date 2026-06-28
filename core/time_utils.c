/******************************************************************************
 * Source File
 *
 * Purpose:
 * Implement the functionality declared in the corresponding
 * header file.
 *
 * Rules:
 * - Function implementations
 * - Private helper functions
 * - Static variables
 * - Internal logic
 
 * Module:
 * Time utility functions for periodic real-time tasks.
 *
 * This module centralizes all operations related to
 * struct timespec manipulation.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "time_utils.h"

void timespec_add_ms(struct timespec *time,
                      uint32_t milliseconds)
{
    /* Add whole seconds*/
    time->tv_sec += milliseconds / 1000;

    /* Add remaining milliseconds as nanoseconds */
    time->tv_nsec += (milliseconds % 1000) * 1000000L;

    /* Normalize nanoseconds*/
    if (time->tv_nsec >= 1000000000L )
    {
        time->tv_sec++;
        time->tv_nsec -= 1000000000L;
    }

}

double timespec_diff_ms(struct timespec start,
                        struct timespec end)
{
    double seconds;
    double nanoseconds;

    /* Compute time difference */
    seconds = (double)(end.tv_sec - start.tv_sec);
    nanoseconds = (double)(end.tv_nsec - start.tv_nsec);

    /* Convert to milliseconds */
    return (seconds * 1000.0) +
           (nanoseconds / 1000000.0);
}

uint64_t timespec_to_ms(struct timespec time)
{
    return ((uint64_t)time.tv_sec * 1000ULL) +
           ((uint64_t)time.tv_nsec / 1000000ULL);
}