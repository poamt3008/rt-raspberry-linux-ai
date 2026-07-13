/******************************************************************************
 * File: inference_mailbox.h
 * Author: Ander
 *
 * Description:
 * Producer-consumer mailbox connecting task_process (producer) with
 * task_inference (consumer). Holds the normalized input vector ready
 * for network_predict(), plus the source filename (used for logging).
 ******************************************************************************/
#ifndef INFERENCE_MAILBOX_H
#define INFERENCE_MAILBOX_H

#include <pthread.h>
#include "config.h"
#include "weights.h"  /* for INPUT_SIZE */

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    float           vector[INPUT_SIZE];
    char            source_filename[FILENAME_MAX_LEN];
    int             ready;
} inference_mailbox_t;

void inference_mailbox_init(inference_mailbox_t *mb);

/**
 * inference_mailbox_put - Producer side. Called by task_process after
 * a digit was successfully extracted from an image.
 */
void inference_mailbox_put(inference_mailbox_t *mb,
                            const float vector[INPUT_SIZE],
                            const char *source_filename);

/**
 * inference_mailbox_get - Consumer side. Called by task_inference.
 * Blocks until a new vector is ready.
 *
 * @out_vector: destination buffer, must hold INPUT_SIZE floats.
 * @out_filename: destination buffer, must be at least FILENAME_MAX_LEN bytes.
 */
void inference_mailbox_get(inference_mailbox_t *mb,
                            float out_vector[INPUT_SIZE],
                            char *out_filename);

#endif /* INFERENCE_MAILBOX_H */