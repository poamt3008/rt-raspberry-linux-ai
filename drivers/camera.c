/******************************************************************************
 * File: camera.c
 * Author: Ander
 *
 * Description:
 * USB camera driver implementation.
 ******************************************************************************/

#include "camera.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Reserved for future camera initialization.
 * This function keeps the API stable if the
 * implementation changes (e.g. OpenCV, V4L2).
 */
int camera_init(void)
{
    // Abrir cámara
    // Configurar resolución
    // Reservar buffers
    return 0;
}

void camera_close(void)
{
    /*
     * No resources to release in the current implementation.
     */
}


int camera_capture(const char *filename)
{
    char filepath[256];
    char command[512];

    snprintf(filepath,
         sizeof(filepath),
         "%s/%s",
         IMAGE_DIRECTORY,
         filename);

    snprintf(command,
         sizeof(command),
         "ffmpeg -nostdin -y -loglevel error -f v4l2 -i %s "
         "-frames:v 1 -update 1 %s",
         CAMERA_DEVICE,
         filepath);
    
    printf("[CAMERA] Command: %s\n", command);
    int ret = system(command);
    

    if (ret != 0)
    {
        fprintf(stderr,
        "[CAMERA] Capture failed (code=%d).\n",
        ret);
        return -1;
    }
    printf("[CAMERA] Image saved: %s\n", filepath);

    return 0;
}

