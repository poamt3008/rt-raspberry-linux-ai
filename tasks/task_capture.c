#include "task_capture.h"
#include "camera.h"
#include "config.h"
#include "time_utils.h"
#include "capture_mailbox.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>

void *task_capture(void *arg)
{
    capture_mailbox_t *mb = (capture_mailbox_t *)arg;
    struct timespec next_activation;
    int image_index = 0;
    char filename[FILENAME_MAX_LEN];

    clock_gettime(CLOCK_MONOTONIC, &next_activation);

    printf("[CAPTURE] Period = %d ms\n", CAPTURE_PERIOD_MS);
    printf("[CAPTURE] Task Started. \n");

    while(1)
    {
        timespec_add_ms(&next_activation, CAPTURE_PERIOD_MS);

        int ret = clock_nanosleep(
            CLOCK_MONOTONIC,
            TIMER_ABSTIME,
            &next_activation,
            NULL);

        if (ret != 0)
        {
            printf("[CAPTURE] clock_nanosleep() failed.\n");
        }

        snprintf(filename, sizeof(filename), "image%03d.jpg", image_index);

        printf("[CAPTURE] %s\n", filename);

        if (camera_capture(filename) != 0)
        {
            printf("[CAPTURE] Image capture failed.\n");
        }
        else
        {
            capture_mailbox_put(mb, filename);
        }

        image_index++;
        if (image_index >= NUMBER_IMAGES)
        {
            image_index = 0;
        }
    }

    return NULL;
}