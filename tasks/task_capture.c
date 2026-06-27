/******************************************************************************
 * Source File
 *
 * Purpose:
 * Implement the functionality declared in the corresponding
 * header file.
 *
 * Rules:
 * - Function implementations
 * - Private helper functions
 * - Static variables
 * - Internal logic
 ******************************************************************************/
/******************************************************************************
 * Librerias
 ******************************************************************************/
#include "task_capture.h"
#include "camera.h"
#include "config.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>


/******************************************************************************
 * Task implementation
 ******************************************************************************/
void *task_capture(void *arg)
{
    /**************************************************************************
     * Local variables
     **************************************************************************/
    struct timespec next_activation;
    int image_index = 0;
    char filename[32];
    (void)arg;

    /**************************************************************************
     * Initialization
     **************************************************************************/
    clock_gettime(CLOCK_MONOTONIC, &next_activation);
    
    printf("[CAPTURE] Period = %d s\n", CAPTURE_PERIOD_SEC);

    printf("[CAPTURE] task Started. \n");

    while(1)
    {
        next_activation.tv_sec += CAPTURE_PERIOD_SEC;

        int ret = clock_nanosleep(
            CLOCK_MONOTONIC,
            TIMER_ABSTIME,
            &next_activation,
            NULL);

        if (ret != 0)
        {
            printf("[CAPTURE] clock_nanosleep() failed.\n");
        }

        /*Tarea periodica*/
        snprintf(filename,
                sizeof(filename),
                "image%03d.jpg",
                image_index);

        printf("[CAPTURE] %s\n",filename);

        if (camera_capture(filename) != 0)
        {
            printf("[CAPTURE] Image capture failed.\n");
        }

        image_index++;
        if ( image_index >= 10)
        {
            image_index = 0;
        }
                
    }

    return NULL;

}