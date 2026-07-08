/* quick smoke test, not part of tests/ yet */
#include "image_proc.h"
#include "network.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>

static int has_jpg_extension(const char *name)
{
    size_t len = strlen(name);

    if (len < 4)
    {
        return 0;
    }

    return (strcmp(name + len - 4, ".jpg") == 0);
}

static void process_one(const char *filepath)
{
    float input[INPUT_SIZE];
    float output[OUTPUT_SIZE];
    int digit_found;
    int predicted;
    char debug_path[300];

    if (image_proc_extract(filepath, input, &digit_found) != 0)
    {
        printf("%-30s FAILED TO LOAD\n", filepath);
        return;
    }

    if (!digit_found)
    {
        printf("%-30s NO DIGIT FOUND\n", filepath);
        return;
    }

    predicted = network_predict(input, output);

    printf("%-30s --> Predicted digit: %d\n", filepath, predicted);

    /* Save the exact 28x28 vector the network saw, upscaled 10x for
     * easy visual inspection (debug only). */
    snprintf(debug_path, sizeof(debug_path), "debug_output/%s_28x28.png",
             strrchr(filepath, '/') ? strrchr(filepath, '/') + 1 : filepath);
    image_proc_save_debug_png(input, debug_path, 10);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        process_one(argv[1]);
        return 0;
    }

    DIR *dir;
    struct dirent *entry;
    char filepath[300];

    dir = opendir(IMAGE_DIRECTORY);
    if (!dir)
    {
        fprintf(stderr, "Could not open directory: %s\n", IMAGE_DIRECTORY);
        fprintf(stderr, "Usage: %s [image.jpg]\n", argv[0]);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (!has_jpg_extension(entry->d_name))
        {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s",
                 IMAGE_DIRECTORY, entry->d_name);

        process_one(filepath);
    }

    closedir(dir);

    return 0;
}