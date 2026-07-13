/******************************************************************************
 * File: task_process.h
 * Author: Ander
 ******************************************************************************/
#ifndef TASK_PROCESS_H
#define TASK_PROCESS_H

#include "capture_mailbox.h"
#include "inference_mailbox.h"

typedef struct {
    capture_mailbox_t   *capture_mb;
    inference_mailbox_t *inference_mb;
} task_process_args_t;

void *task_process(void *arg);

#endif /* TASK_PROCESS_H */