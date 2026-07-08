/* quick smoke test, not part of tests/ yet */
#include "image_proc.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>

static void print_ascii(const float vec[INPUT_SIZE])
{
    const char *shades = " .:-=+*#%@";
    int y, x;

    for (y = 0; y < 28; y++)
    {
        for (x = 0; x < 28; x++)
        {
            float v = vec[y * 28 + x];
            int idx = (int)(v * 9.0f);
            if (idx > 9) idx = 9;
            if (idx < 0) idx = 0;
            putchar(shades[idx]);
        }
        putchar('\n');
    }
}

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
    float vec[INPUT_SIZE];
    int found;

    printf("=== %s ===\n", filepath);

    if (image_proc_extract(filepath, vec, &found) != 0)
    {
        fprintf(stderr, "  Failed to load/decode image.\n");
        return;
    }

    if (!found)
    {
        printf("  No digit found in image.\n\n");
        return;
    }

    printf("  Digit detected. ASCII preview:\n\n");
    print_ascii(vec);
    printf("\n");
}

int main(int argc, char **argv)
{
    /* If a specific file is given, process only that one. */
    if (argc == 2)
    {
        process_one(argv[1]);
        return 0;
    }

    /* Otherwise, scan IMAGE_DIRECTORY (from config.h) for all .jpg files. */
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