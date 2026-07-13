#ifndef TASK_INFERENCE_H
#define TASK_INFERENCE_H

/******************************************************************************
 * File: task_inference.h
 * Author: Ander
 *
 * Description:
 * Periodic neural network inference task.
 ******************************************************************************/
#include "inference_mailbox.h"

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/* Periodic inference task. */
/* arg must be a valid pointer to an inference_mailbox_t */
void *task_inference(void *arg);

#endif