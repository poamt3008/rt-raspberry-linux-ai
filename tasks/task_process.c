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

#include <stdio.h>

/******************************************************************************
 * Task implementation
 ******************************************************************************/
void *task_process(void *arg)
{
    task_process_args_t *args = (task_process_args_t *)arg;
    char filename[FILENAME_MAX_LEN];
    char filepath[FILENAME_MAX_LEN + 32];
    float vector[INPUT_SIZE];
    int digit_found;

    printf("[PROCESS] Task started.\n");

    while (1)
    {
        /* Block until task_capture has a new photo ready */
        capture_mailbox_get(args->capture_mb, filename);

        snprintf(filepath, sizeof(filepath), "%s/%s", IMAGE_DIRECTORY, filename);

        printf("[PROCESS] Processing %s\n", filepath);

        if (image_proc_extract(filepath, vector, &digit_found) != 0)
        {
            printf("[PROCESS] Failed to load/decode %s\n", filepath);
            continue; /* go back to waiting for the next photo */
        }

        if (!digit_found)
        {
            printf("[PROCESS] No digit found in %s\n", filepath);
            continue;
        }

        printf("[PROCESS] Digit found in %s, forwarding to inference.\n", filepath);

        inference_mailbox_put(args->inference_mb, vector, filename);
    }

    return NULL;
}