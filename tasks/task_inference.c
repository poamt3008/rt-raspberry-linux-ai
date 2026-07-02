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
#include "network.h"
#include "input_loader.h"

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

    float input[INPUT_SIZE];
    float output[OUTPUT_SIZE];

    char filename[64];

    int digit;
    int digit_index = 0;
    int ret;

    (void)arg;

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

        ret = clock_nanosleep(
            CLOCK_MONOTONIC,
            TIMER_ABSTIME,
            &next_activation,
            NULL);

        if (ret != 0)
        {
            printf("[INFERENCE] clock_nanosleep() failed.\n");
        }
        /**/
        /**********************************************************************
        * Build input filename
        **********************************************************************/
        snprintf(filename,
         sizeof(filename),
         DATASET_PATH "/image_%d00.txt",
         digit_index);

        /**********************************************************************
        * Load input vector
        **********************************************************************/
        if (input_loader_load(filename, input) != 0)
        {
            printf("[INFERENCE] Failed to load %s\n",
                filename);
        }
        else
        {
            /******************************************************************
            * Run neural network
            ******************************************************************/
            digit = network_predict(input, output);

            printf("[INFERENCE] %-20s --> Digit: %d\n",
                filename,
                digit);
        }

        /**********************************************************************
        * Next image
        **********************************************************************/
        digit_index++;

        if (digit_index >= NUMBER_TEST_IMAGES)
        {
            digit_index = 0;
        }
        /**/

    }
    return NULL;

}