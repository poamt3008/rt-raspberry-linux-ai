/******************************************************************************
 * File: inference_mailbox.c
 ******************************************************************************/

#include "inference_mailbox.h"

#include <string.h>

void inference_mailbox_init(inference_mailbox_t *mb)
{
    pthread_mutex_init(&mb->lock, NULL);
    pthread_cond_init(&mb->cond, NULL);
    memset(mb->vector, 0, sizeof(mb->vector));
    mb->source_filename[0] = '\0';
    mb->ready = 0;
}

void inference_mailbox_put(inference_mailbox_t *mb,
                            const float vector[INPUT_SIZE],
                            const char *source_filename)
{
    pthread_mutex_lock(&mb->lock);

    memcpy(mb->vector, vector, sizeof(mb->vector));
    strncpy(mb->source_filename, source_filename, sizeof(mb->source_filename) - 1);
    mb->source_filename[sizeof(mb->source_filename) - 1] = '\0';
    mb->ready = 1;

    pthread_cond_signal(&mb->cond);

    pthread_mutex_unlock(&mb->lock);
}

void inference_mailbox_get(inference_mailbox_t *mb,
                            float out_vector[INPUT_SIZE],
                            char *out_filename)
{
    pthread_mutex_lock(&mb->lock);

    while (!mb->ready)
    {
        pthread_cond_wait(&mb->cond, &mb->lock);
    }

    memcpy(out_vector, mb->vector, sizeof(mb->vector));
    strncpy(out_filename, mb->source_filename, FILENAME_MAX_LEN);
    mb->ready = 0;

    pthread_mutex_unlock(&mb->lock);
}