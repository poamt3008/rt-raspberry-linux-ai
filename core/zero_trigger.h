/******************************************************************************
 * File: zero_trigger.h
 * Author: Ander
 *
 * Description:
 * Minimal event counter: task_inference calls zero_trigger_notify()
 * each time digit 0 is detected. task_led_blink consumes one event
 * per activation, running a single bounded blink window in response.
 ******************************************************************************/
#ifndef ZERO_TRIGGER_H
#define ZERO_TRIGGER_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int             count;   /* pending "digit 0 detected" events */
} zero_trigger_t;

void zero_trigger_init(zero_trigger_t *t);
void zero_trigger_notify(zero_trigger_t *t);
void zero_trigger_wait(zero_trigger_t *t);

#endif /* ZERO_TRIGGER_H */