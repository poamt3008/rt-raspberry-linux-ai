/******************************************************************************
 * File: task_process.c
 *
 * Consumer of capture_mailbox, producer of inference_mailbox.
 * Reactive task (not periodic): blocks until task_capture signals a
 * new photo, runs image_proc_extract() on it, and -- only if a digit
 * was actually found -- forwards the resulting vector to
 * task_inference and goes back to waiting.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "task_process.h"
#include "image_proc.h"
#include "config.h"
#include "time_utils.h"

#include <stdio.h>
#include <time.h>

/******************************************************************************
 * Task implementation
 ******************************************************************************/
void *task_process(void *arg)
{
    /**************************************************************************
     * Local variables
     **************************************************************************/
    task_process_args_t *args = (task_process_args_t *)arg;
    char filename[FILENAME_MAX_LEN];
    char filepath[FILENAME_MAX_LEN + 32];
    float vector[INPUT_SIZE];
    int digit_found;
    struct timespec t_start, t_end; /*to measure the time period*/

    printf("[PROCESS] Task started.\n");

    while (1)
    {
        /* Block until task_capture has a new photo ready */
        capture_mailbox_get(args->capture_mb, filename);

        /******* Start Measuring Computation Time **********/
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        snprintf(filepath, sizeof(filepath), "%s/%s", IMAGE_DIRECTORY, filename);

        printf("[PROCESS] Processing %s\n", filepath);

        if (image_proc_extract(filepath, vector, &digit_found) != 0)
        {
            printf("[PROCESS] Failed to load/decode %s\n", filepath);
        }
        else if (!digit_found)
        {
            printf("[PROCESS] No digit found in %s\n", filepath);
        }
        else
        {
            printf("[PROCESS] Digit found in %s, forwarding to inference.\n", filepath);
            inference_mailbox_put(args->inference_mb, vector, filename);
        }

        /******* End Measuring Computation Time **********/
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        printf("[PROCESS] Task execution time: %.2f ms\n",
           timespec_diff_ms(t_start, t_end));
    }

    return NULL;
}