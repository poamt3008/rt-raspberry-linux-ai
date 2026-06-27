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
    printf("[CAPTURE] task Started. \n");


    /********INICIO de Tarea Periodica*************/
    while(1)
    {
        /*Tarea periodica*/
        snprintf(filename,
                sizeof(filename),
                "image%03d.jpg",
                image_index);

        printf("[CAPTURE] %s\n",filename);

        camera_capture(filename);

        image_index++;
        if ( image_index >= 10)
        {
            image_index = 0;
        }
        break;
    }

    return NULL;

}