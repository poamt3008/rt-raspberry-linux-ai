/******************************************************************************
 * File: main.c
 * Author: Ander
 *
 * Description:
 * Main application entry point.
 *
 * Responsibilities:
 * - Initialize system resources.
 * - Create application tasks.
 * - Wait for task termination.
 * - Release allocated resources.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdio.h>
#include <pthread.h>

#include "camera.h"
#include "task_capture.h"

/******************************************************************************
 * Main
 ******************************************************************************/

int main(void)
{
    /**************************************************************************
     * Local variables
     **************************************************************************/
    pthread_t capture_thread;

    /**************************************************************************
     * Inicializacion
     **************************************************************************/
    printf("===RT Linux IA ===\n");
    if(camera_init() != 0)
    {
        printf("[MAIN] Camera initialization failed.\n");
        return -1;
    }

    /**************************************************************************
     * Create tasks
     **************************************************************************/
    printf("[MAIN] Creating capture task...\n");

    int ret = pthread_create(&capture_thread,
                                NULL,
                                task_capture,
                                NULL);

    if(ret != 0)
    {
        printf("[MAIN] Failed to create capture task.\n");

        camera_close();

        return -1;
    }

    /**************************************************************************
     * Wait for tasks
     **************************************************************************/

    pthread_join(capture_thread, NULL);
    printf("[MAIN] Capture task finished.\n");
    
    /**************************************************************************
     * Release resources
     **************************************************************************/

    camera_close();

    printf("[MAIN] Application finished.\n");

    return 0;

}