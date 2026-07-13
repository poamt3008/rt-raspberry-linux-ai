/******************************************************************************
 * File: capture_mailbox.h
 * Author: Ander
 *
 * Description:
 * Producer-consumer mailbox connecting task_capture (producer) with
 * task_process (consumer). Holds the filename of the most recently
 * captured image.
 ******************************************************************************/
#ifndef CAPTURE_MAILBOX_H
#define CAPTURE_MAILBOX_H

#include <pthread.h>
#include "config.h"

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    char            filename[FILENAME_MAX_LEN];
    int             ready;   /* 0 = no new image, 1 = image waiting to be processed */
} capture_mailbox_t;

/**
 * capture_mailbox_init - Initialize a mailbox before first use.
 * Must be called once (e.g. from main) before any thread uses put/get.
 */
void capture_mailbox_init(capture_mailbox_t *mb);

/**
 * capture_mailbox_put - Producer side. Called by task_capture after a
 * new photo is saved. Copies filename into the mailbox, marks it
 * ready, and wakes up task_process if it's waiting.
 */
void capture_mailbox_put(capture_mailbox_t *mb, const char *filename);

/**
 * capture_mailbox_get - Consumer side. Called by task_process. Blocks
 * until a new image is ready, then copies the filename into the
 * caller-provided buffer and clears the ready flag.
 *
 * @out_filename: destination buffer, must be at least 32 bytes.
 */
void capture_mailbox_get(capture_mailbox_t *mb, char *out_filename);

#endif /* CAPTURE_MAILBOX_H */