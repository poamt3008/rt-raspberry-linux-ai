#include "input_loader.h"

#include <stdio.h>

/******************************************************************************
 * Load an input vector from a text file.
 ******************************************************************************/
int input_loader_load(
    const char *filename,
    float input[INPUT_SIZE])
{
    FILE *fp;

    int i;

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        return -1;
    }

    for (i = 0; i < INPUT_SIZE; i++)
    {
        if (fscanf(fp, "%f", &input[i]) != 1)
        {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);

    return 0;
}