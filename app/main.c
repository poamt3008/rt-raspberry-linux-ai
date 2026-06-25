#include <stdio.h>

#include "camera.h"

int main(void)
{
    printf("=== Camera Test ===\n");

    if (camera_init() != 0)
    {
        printf("Camera initialization failed.\n");
        return -1;
    }

    if (camera_capture("test.jpg") != 0)
    {
        printf("Capture failed.\n");
        return -1;
    }

    camera_close();

    printf("Test finished successfully.\n");

    return 0;
}