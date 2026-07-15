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
#include "task_process.h"
#include "task_inference.h"
#include "task_network.h"
#include "capture_mailbox.h"
#include "inference_mailbox.h"
#include "shared_result_client.h"

/******************************************************************************
 * Main
 ******************************************************************************/

int main(void)
{
    /**************************************************************************
     * Local variables
     **************************************************************************/
    pthread_t capture_thread;
    pthread_t process_thread;
    pthread_t inference_thread;
    pthread_t network_thread;

    capture_mailbox_t   capture_mb;
    inference_mailbox_t inference_mb;
    task_process_args_t process_args;

    int ret = 0;

    /**************************************************************************
     * Inicializacion
     **************************************************************************/
    printf("=== RT Linux IA ===\n");

    if (camera_init() != 0)
    {
        printf("[MAIN] Camera initialization failed.\n");
        return -1;
    }

    if (shared_result_client_init() != 0)
{
    printf("[MAIN] Shared result client not connected (Process 1 not running?). "
           "Continuing without it.\n");
}

    capture_mailbox_init(&capture_mb);
    inference_mailbox_init(&inference_mb);

    process_args.capture_mb   = &capture_mb;
    process_args.inference_mb = &inference_mb;

    /**************************************************************************
     * Create tasks
     **************************************************************************/
    printf("[MAIN] Creating capture task...\n");

    ret = pthread_create(&capture_thread,
                          NULL,
                          task_capture,
                          &capture_mb);

    if (ret != 0)
    {
        printf("[MAIN] Failed to create capture task.\n");
        camera_close();
        return -1;
    }

    printf("[MAIN] Creating process task...\n");

    ret = pthread_create(&process_thread,
                          NULL,
                          task_process,
                          &process_args);

    if (ret != 0)
    {
        printf("[MAIN] Failed to create process task.\n");
        camera_close();
        return -1;
    }

    printf("[MAIN] Creating inference task...\n");

    ret = pthread_create(&inference_thread,
                          NULL,
                          task_inference,
                          &inference_mb);

    if (ret != 0)
    {
        printf("[MAIN] Failed to create inference task.\n");
        camera_close();
        return -1;
    }

    printf("[MAIN] Creating network identification task...\n");

    ret = pthread_create(&network_thread,
                          NULL,
                          task_network,
                          NULL);

    if (ret != 0)
    {
        printf("[MAIN] Failed to create network task.\n");
        camera_close();
        return -1;
    }

    /**************************************************************************
     * Wait for tasks
     **************************************************************************/

    pthread_join(capture_thread, NULL);
    pthread_join(process_thread, NULL);
    pthread_join(inference_thread, NULL);
    pthread_join(network_thread, NULL);

    printf("[MAIN] All tasks finished.\n");

    /**************************************************************************
     * Release resources
     **************************************************************************/
    shared_result_client_close();
    camera_close();

    printf("[MAIN] Application finished.\n");

    return 0;
}