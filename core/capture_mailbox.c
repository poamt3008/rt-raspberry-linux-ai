/******************************************************************************
 * File: capture_mailbox.c
 * Author: Ander
 *
 * Description:
 * Implements the producer-consumer mailbox connecting task_capture
 * (producer) with task_process (consumer).
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "capture_mailbox.h"

#include <string.h>

/******************************************************************************
 * Public functions
 ******************************************************************************/

void capture_mailbox_init(capture_mailbox_t *mb)
{
    pthread_mutex_init(&mb->lock, NULL);
    pthread_cond_init(&mb->cond, NULL);
    mb->filename[0] = '\0';
    mb->ready = 0;
}

void capture_mailbox_put(capture_mailbox_t *mb, const char *filename)
{
    pthread_mutex_lock(&mb->lock);

    strncpy(mb->filename, filename, sizeof(mb->filename) - 1);
    mb->filename[sizeof(mb->filename) - 1] = '\0';
    mb->ready = 1;

    pthread_cond_signal(&mb->cond);

    pthread_mutex_unlock(&mb->lock);
}

void capture_mailbox_get(capture_mailbox_t *mb, char *out_filename)
{
    pthread_mutex_lock(&mb->lock);

    while (!mb->ready)
    {
        pthread_cond_wait(&mb->cond, &mb->lock);
    }

    strncpy(out_filename, mb->filename, FILENAME_MAX_LEN);
    mb->ready = 0;

    pthread_mutex_unlock(&mb->lock);
}