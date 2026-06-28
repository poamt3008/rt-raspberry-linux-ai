/******************************************************************************
 * Source File
 *
 * Purpose:
 * Implement the functionality declared in the corresponding
 * header file.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "task_inference.h"
#include "time_utils.h"
#include "config.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>

/******************************************************************************
 * Task implementation
 ******************************************************************************/
void *task_inference(void *arg)
{
 /**************************************************************************
 * Local variables
**************************************************************************/
    struct timespec next_activation;
    (void)arg;
/**************************************************************************
* Initialization
**************************************************************************/

    clock_gettime(CLOCK_MONOTONIC, &next_activation);
    
    printf("[INFERENCE] Period = %d ms\n", INFERENCE_PERIOD_MS);

    printf("[INFERENCE] Task started.\n");
    /**************************************************************************
    * Main loop
    **************************************************************************/
    while(1)
    {
        timespec_add_ms(&next_activation,
                        INFERENCE_PERIOD_MS);

        int ret = clock_nanosleep(
            CLOCK_MONOTONIC,
            TIMER_ABSTIME,
            &next_activation,
            NULL);

        if (ret != 0)
        {
            printf("[INFERENCE] clock_nanosleep() failed.\n");
        }

        printf("[INFERENCE] Waiting for image...\n");

    }
    return NULL;

}