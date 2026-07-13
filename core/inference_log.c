/******************************************************************************
 * File: inference_log.c
 ******************************************************************************/

#include "inference_log.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/******************************************************************************
 * Private types
 ******************************************************************************/
typedef struct {
    char  photo_filename[FILENAME_MAX_LEN];
    char  timestamp[32];
    int   predicted_digit;
    float confidence;
} log_entry_t;

/******************************************************************************
 * Private state
 ******************************************************************************/
static log_entry_t     entries[LOG_MAX_ENTRIES];
static int             entry_count = 0;   /* how many slots are currently filled */
static int             next_index = 0;    /* circular write position */
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************
 * Private helper functions
 ******************************************************************************/

static void format_timestamp(char *out, size_t out_size)
{
    time_t now = time(NULL);
    struct tm tm_now;

    localtime_r(&now, &tm_now);
    strftime(out, out_size, "%Y-%m-%d %H:%M:%S", &tm_now);
}

/*
 * Rewrite LOG_FILE_PATH from scratch with all currently stored
 * entries, oldest first. Must be called with log_lock held.
 */
static int rewrite_log_file(void)
{
    FILE *fp;
    int i;
    int start;

    fp = fopen(LOG_FILE_PATH, "w");
    if (!fp)
    {
        return -1;
    }

    fprintf(fp, "photo,timestamp,digit,confidence\n");

    /* Entries are stored circularly starting at next_index once the
     * buffer has wrapped around; start from the oldest one. */
    start = (entry_count < LOG_MAX_ENTRIES) ? 0 : next_index;

    for (i = 0; i < entry_count; i++)
    {
        int idx = (start + i) % LOG_MAX_ENTRIES;

        fprintf(fp, "%s,%s,%d,%.2f\n",
                entries[idx].photo_filename,
                entries[idx].timestamp,
                entries[idx].predicted_digit,
                entries[idx].confidence);
    }

    fclose(fp);

    return 0;
}

/******************************************************************************
 * Public function
 ******************************************************************************/
int inference_log_add(const char *photo_filename,
                       int predicted_digit,
                       float confidence)
{
    int ret;

    pthread_mutex_lock(&log_lock);

    strncpy(entries[next_index].photo_filename, photo_filename, FILENAME_MAX_LEN - 1);
    entries[next_index].photo_filename[FILENAME_MAX_LEN - 1] = '\0';

    format_timestamp(entries[next_index].timestamp, sizeof(entries[next_index].timestamp));

    entries[next_index].predicted_digit = predicted_digit;
    entries[next_index].confidence = confidence;

    next_index = (next_index + 1) % LOG_MAX_ENTRIES;
    if (entry_count < LOG_MAX_ENTRIES)
    {
        entry_count++;
    }

    ret = rewrite_log_file();

    pthread_mutex_unlock(&log_lock);

    return ret;
}