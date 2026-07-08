/******************************************************************************
 * File: image_proc.c
 *
 * Pipeline:
 *   1. Decode JPEG to raw RGB bytes (stb_image)
 *   2. Convert to grayscale
 *   3. Otsu threshold (automatic, adapts to each photo's lighting)
 *   4. Flood-fill connected components; keep the largest one that:
 *        - does NOT touch the image border (discards lens vignetting)
 *        - has a plausible area (rejects sensor noise/dust, and rejects
 *          components spanning most of the frame)
 *        - has a low enough average intensity to be real ink rather
 *          than a lighter shadow blob
 *   5. Re-run flood-fill once more from the winning component's seed to
 *      build a dedicated mask -- any pixel inside the bounding box that
 *      is NOT part of this exact connected component is forced to
 *      background (0), discarding stray marks/halos/noise within the
 *      crop rectangle.
 *   6. Scale the masked crop preserving aspect ratio so its longer side
 *      is ~20px, place it centered in the 28x28 canvas, then shift by
 *      center-of-mass -- matching the original MNIST normalization
 *      convention (digit occupies ~20x20 of a 28x28 frame, centered by
 *      ink centroid, not by bare bounding-box center).
 *   7. Flatten row-major into the output vector.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "image_proc.h"
#include "config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 * Local constants
 ******************************************************************************/
#define DIGIT_SIDE 28   /* sqrt(INPUT_SIZE) -- 28*28 = 784 */

/* Reject a component too small to be a real digit (sensor noise,
 * dust, JPEG artifacts) or too large (background/paper edge spanning
 * most of the frame). Expressed as a ratio of total image area so it
 * scales with camera resolution. */
#define MIN_DIGIT_AREA_RATIO 0.003  /* 0.3% of total image area */
#define MAX_DIGIT_AREA_RATIO 0.5    /* 50% of total image area */

/* Real ink is close to black. Shadows are a lighter, mid-gray --
 * even if they fall under the Otsu cut, their average intensity
 * is noticeably higher than solid ink. Validated against real
 * printed digits 0-9: genuine ink ranges ~46-103 depending on
 * stroke thickness; shadow/vignette blobs measured ~150-190. */
#define MAX_INK_MEAN_INTENSITY 130.0

/* MNIST convention: the digit occupies roughly 20 of the 28 pixels,
 * leaving a natural margin, and is centered by ink centroid. */
#define MNIST_TARGET_DIGIT_SIZE 20

/******************************************************************************
 * Private helper functions
 ******************************************************************************/

static unsigned char *to_grayscale(const unsigned char *rgb,
                                    int width,
                                    int height,
                                    int channels)
{
    unsigned char *gray;
    int x;
    int y;

    gray = malloc((size_t)width * (size_t)height);
    if (!gray)
    {
        return NULL;
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            const unsigned char *px = &rgb[(size_t)(y * width + x) * channels];
            float value;

            if (channels >= 3)
            {
                value = 0.299f * px[0] + 0.587f * px[1] + 0.114f * px[2];
            }
            else
            {
                value = (float)px[0];
            }

            gray[y * width + x] = (unsigned char)value;
        }
    }

    return gray;
}

/*
 * Otsu's method: automatically pick the grayscale threshold that best
 * separates the image into two classes (background / ink) by
 * maximizing the between-class variance.
 */
static int otsu_threshold(const unsigned char *gray, int width, int height)
{
    long histogram[256] = {0};
    long total = (long)width * (long)height;
    long i;
    double sum_all = 0.0;
    double sum_bg = 0.0;
    long weight_bg = 0;
    double best_variance = -1.0;
    int best_threshold = 128; /* fallback if something goes wrong */
    int t;

    for (i = 0; i < total; i++)
    {
        histogram[gray[i]]++;
    }

    for (t = 0; t < 256; t++)
    {
        sum_all += (double)t * (double)histogram[t];
    }

    for (t = 0; t < 256; t++)
    {
        long weight_fg;
        double sum_fg;
        double mean_bg;
        double mean_fg;
        double between_variance;

        weight_bg += histogram[t];
        if (weight_bg == 0)
        {
            continue;
        }

        weight_fg = total - weight_bg;
        if (weight_fg == 0)
        {
            break;
        }

        sum_bg += (double)t * (double)histogram[t];
        sum_fg = sum_all - sum_bg;

        mean_bg = sum_bg / (double)weight_bg;
        mean_fg = sum_fg / (double)weight_fg;

        between_variance = (double)weight_bg * (double)weight_fg *
                            (mean_bg - mean_fg) * (mean_bg - mean_fg);

        if (between_variance > best_variance)
        {
            best_variance = between_variance;
            best_threshold = t;
        }
    }

    return best_threshold;
}

/*
 * Flood-fill (iterative BFS) starting at (start_x, start_y), marking
 * all connected "ink" pixels (gray < threshold, 4-connectivity) as
 * visited. Updates the component's bounding box, accumulates the sum
 * of grayscale intensities (used later to compute the mean), and
 * reports whether it touched any image border.
 */
static long flood_fill(const unsigned char *gray,
                        unsigned char *visited,
                        int width,
                        int height,
                        int threshold,
                        int start_x,
                        int start_y,
                        int *min_x,
                        int *min_y,
                        int *max_x,
                        int *max_y,
                        int *touches_border,
                        long *intensity_sum,
                        int *queue)
{
    long head = 0;
    long tail = 0;
    long area = 0;

    *min_x = start_x;
    *max_x = start_x;
    *min_y = start_y;
    *max_y = start_y;
    *touches_border = 0;
    *intensity_sum = 0;

    queue[tail++] = start_y * width + start_x;
    visited[start_y * width + start_x] = 1;

    while (head < tail)
    {
        int idx = queue[head++];
        int x = idx % width;
        int y = idx / width;
        int dx;
        static const int offsets[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

        area++;
        *intensity_sum += gray[idx];

        if (x < *min_x) *min_x = x;
        if (x > *max_x) *max_x = x;
        if (y < *min_y) *min_y = y;
        if (y > *max_y) *max_y = y;

        if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
        {
            *touches_border = 1;
        }

        for (dx = 0; dx < 4; dx++)
        {
            int nx = x + offsets[dx][0];
            int ny = y + offsets[dx][1];
            int nidx;

            if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            {
                continue;
            }

            nidx = ny * width + nx;

            if (visited[nidx] || gray[nidx] >= threshold)
            {
                continue;
            }

            visited[nidx] = 1;
            queue[tail++] = nidx;
        }
    }

    return area;
}

/*
 * Find the largest connected "ink" component that:
 *   - does NOT touch the image border
 *   - has a plausible area (not noise, not most of the frame)
 *   - has a low enough average intensity to be real ink
 *
 * Returns 1 and fills the bounding box + seed point if found, 0 otherwise.
 * The seed point (out_seed_x, out_seed_y) is a pixel guaranteed to be
 * inside the winning component -- used later to rebuild a dedicated
 * mask for that exact component via a second flood_fill() call.
 */
static int find_digit_component(const unsigned char *gray,
                                 int width,
                                 int height,
                                 int threshold,
                                 int *out_min_x,
                                 int *out_min_y,
                                 int *out_max_x,
                                 int *out_max_y,
                                 int *out_seed_x,
                                 int *out_seed_y)
{
    unsigned char *visited;
    int *queue;
    long best_area = 0;
    int found = 0;
    int x, y;
    long total_pixels = (long)width * (long)height;
    long min_area = (long)(MIN_DIGIT_AREA_RATIO * (double)total_pixels);
    long max_area = (long)(MAX_DIGIT_AREA_RATIO * (double)total_pixels);

    visited = calloc((size_t)total_pixels, 1);
    queue = malloc((size_t)total_pixels * sizeof(int));

    if (!visited || !queue)
    {
        free(visited);
        free(queue);
        return 0;
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            int idx = y * width + x;
            int min_x, min_y, max_x, max_y, touches_border;
            long area;
            long intensity_sum;
            double mean_intensity;

            if (visited[idx] || gray[idx] >= threshold)
            {
                continue;
            }

            area = flood_fill(gray, visited, width, height, threshold,
                               x, y, &min_x, &min_y, &max_x, &max_y,
                               &touches_border, &intensity_sum, queue);

            if (touches_border)
            {
                continue; /* vignetting, or ink merged with a shadow reaching the edge */
            }

            if (area < min_area || area > max_area)
            {
                continue; /* too small (noise) or too large (shadow/paper edge) */
            }

            mean_intensity = (double)intensity_sum / (double)area;

            if (mean_intensity > MAX_INK_MEAN_INTENSITY)
            {
                continue; /* too light on average to be solid ink -- likely a shadow blob */
            }

            if (area > best_area)
            {
                best_area = area;
                *out_min_x = min_x;
                *out_min_y = min_y;
                *out_max_x = max_x;
                *out_max_y = max_y;
                *out_seed_x = x;
                *out_seed_y = y;
                found = 1;
            }
        }
    }

    free(visited);
    free(queue);

    return found;
}

/*
 * Rebuild a dedicated mask for exactly the winning component, by
 * flood-filling once more from its seed point on a fresh visited
 * buffer. mask[i] == 1 means pixel i belongs to the digit's ink.
 */
static void build_component_mask(const unsigned char *gray,
                                  unsigned char *mask,
                                  int width,
                                  int height,
                                  int threshold,
                                  int seed_x,
                                  int seed_y,
                                  int *queue)
{
    int min_x, min_y, max_x, max_y, touches_border;
    long intensity_sum;

    memset(mask, 0, (size_t)width * (size_t)height);

    flood_fill(gray, mask, width, height, threshold,
               seed_x, seed_y, &min_x, &min_y, &max_x, &max_y,
               &touches_border, &intensity_sum, queue);
}

/*
 * Proper MNIST-style preprocessing:
 *   1. Scale the tight ink bbox preserving aspect ratio so the LONGER
 *      side becomes ~20px (leaving natural margin inside the 28x28 canvas).
 *   2. Place it centered in the 28x28 canvas by bounding-box center.
 *   3. Shift by center-of-mass so the ink's centroid lands at (14,14),
 *      matching the original MNIST normalization algorithm.
 *
 * Only pixels belonging to `mask` (the exact winning component) are
 * treated as ink -- everything else inside the crop rectangle is
 * forced to background (0), discarding stray marks/halos/noise that
 * happened to fall inside the bounding box but are not part of the
 * digit's connected stroke.
 */
static void crop_resize_normalize(const unsigned char *gray,
                                   const unsigned char *mask,
                                   int width,
                                   int min_x,
                                   int min_y,
                                   int max_x,
                                   int max_y,
                                   float output[INPUT_SIZE])
{
    int crop_w = max_x - min_x + 1;
    int crop_h = max_y - min_y + 1;
    int long_side = crop_w > crop_h ? crop_w : crop_h;
    double scale = (double)MNIST_TARGET_DIGIT_SIZE / (double)long_side;
    int scaled_w = (int)(crop_w * scale + 0.5);
    int scaled_h = (int)(crop_h * scale + 0.5);
    int offset_x;
    int offset_y;
    float canvas[INPUT_SIZE];
    double sum_x = 0.0, sum_y = 0.0, sum_mass = 0.0;
    int shift_x = 0, shift_y = 0;
    int ty, tx, i;

    if (scaled_w < 1) scaled_w = 1;
    if (scaled_h < 1) scaled_h = 1;

    offset_x = (DIGIT_SIDE - scaled_w) / 2;
    offset_y = (DIGIT_SIDE - scaled_h) / 2;

    for (i = 0; i < INPUT_SIZE; i++)
    {
        canvas[i] = 0.0f;
    }

    /* Pass 1: render scaled digit, centered by bounding box, into canvas.
     * Only pixels marked in `mask` count as ink; everything else
     * becomes background (0), even if it falls inside the crop. */
    for (ty = 0; ty < scaled_h; ty++)
    {
        for (tx = 0; tx < scaled_w; tx++)
        {
            int cy = ty + offset_y;
            int cx = tx + offset_x;
            int sx, sy, sidx;

            if (cx < 0 || cx >= DIGIT_SIDE || cy < 0 || cy >= DIGIT_SIDE)
            {
                continue;
            }

            sx = min_x + (tx * crop_w) / scaled_w;
            sy = min_y + (ty * crop_h) / scaled_h;
            sidx = sy * width + sx;

            if (mask[sidx])
            {
                canvas[cy * DIGIT_SIDE + cx] = 1.0f - ((float)gray[sidx] / 255.0f);
            }
            /* else: leave as background (0.0f), already initialized */
        }
    }

    /* Pass 2: compute center of mass of the rendered canvas */
    for (ty = 0; ty < DIGIT_SIDE; ty++)
    {
        for (tx = 0; tx < DIGIT_SIDE; tx++)
        {
            float v = canvas[ty * DIGIT_SIDE + tx];
            sum_x += tx * v;
            sum_y += ty * v;
            sum_mass += v;
        }
    }

    if (sum_mass > 0.0)
    {
        double com_x = sum_x / sum_mass;
        double com_y = sum_y / sum_mass;
        shift_x = (int)((DIGIT_SIDE / 2.0) - com_x + 0.5);
        shift_y = (int)((DIGIT_SIDE / 2.0) - com_y + 0.5);
    }

    /* Pass 3: shift canvas into output by center-of-mass offset */
    for (i = 0; i < INPUT_SIZE; i++)
    {
        output[i] = 0.0f;
    }

    for (ty = 0; ty < DIGIT_SIDE; ty++)
    {
        for (tx = 0; tx < DIGIT_SIDE; tx++)
        {
            int nx = tx + shift_x;
            int ny = ty + shift_y;

            if (nx < 0 || nx >= DIGIT_SIDE || ny < 0 || ny >= DIGIT_SIDE)
            {
                continue;
            }

            output[ny * DIGIT_SIDE + nx] = canvas[ty * DIGIT_SIDE + tx];
        }
    }
}

/******************************************************************************
 * Public function
 ******************************************************************************/
int image_proc_extract(const char *filename,
                        float output[INPUT_SIZE],
                        int *digit_found)
{
    unsigned char *rgb = NULL;
    unsigned char *gray = NULL;
    unsigned char *mask = NULL;
    int *queue = NULL;
    int width;
    int height;
    int channels;
    int threshold;
    int min_x = 0, min_y = 0, max_x = 0, max_y = 0;
    int seed_x = 0, seed_y = 0;
    int i;

    *digit_found = 0;
    for (i = 0; i < INPUT_SIZE; i++)
    {
        output[i] = 0.0f;
    }

    rgb = stbi_load(filename, &width, &height, &channels, 0);
    if (!rgb)
    {
        fprintf(stderr, "image_proc_extract: failed to load %s (%s)\n",
                filename, stbi_failure_reason());
        return -1;
    }

    gray = to_grayscale(rgb, width, height, channels);
    if (!gray)
    {
        fprintf(stderr, "image_proc_extract: out of memory\n");
        stbi_image_free(rgb);
        return -1;
    }

    threshold = otsu_threshold(gray, width, height);

    if (!find_digit_component(gray, width, height, threshold,
                               &min_x, &min_y, &max_x, &max_y,
                               &seed_x, &seed_y))
    {
        free(gray);
        stbi_image_free(rgb);
        return 0;
    }

    mask = malloc((size_t)width * (size_t)height);
    queue = malloc((size_t)width * (size_t)height * sizeof(int));

    if (!mask || !queue)
    {
        fprintf(stderr, "image_proc_extract: out of memory\n");
        free(mask);
        free(queue);
        free(gray);
        stbi_image_free(rgb);
        return -1;
    }

    build_component_mask(gray, mask, width, height, threshold, seed_x, seed_y, queue);

    crop_resize_normalize(gray, mask, width, min_x, min_y, max_x, max_y, output);
    *digit_found = 1;

    free(mask);
    free(queue);
    free(gray);
    stbi_image_free(rgb);

    return 0;
}

/******************************************************************************
 * Debug helper (not used in production path)
 ******************************************************************************/
int image_proc_save_debug_png(const float vector[INPUT_SIZE],
                                const char *out_path,
                                int scale)
{
    unsigned char *pixels;
    int out_side = DIGIT_SIDE * scale;
    int ty, tx, sy, sx;
    int ret;

    if (scale < 1)
    {
        scale = 1;
        out_side = DIGIT_SIDE;
    }

    pixels = malloc((size_t)out_side * (size_t)out_side);
    if (!pixels)
    {
        return -1;
    }

    /* Nearest-neighbor upscale, inverting back to "ink = dark, like a
     * normal photo" for easier visual comparison against the original. */
    for (ty = 0; ty < DIGIT_SIDE; ty++)
    {
        for (tx = 0; tx < DIGIT_SIDE; tx++)
        {
            float v = vector[ty * DIGIT_SIDE + tx];
            unsigned char pixel = (unsigned char)((1.0f - v) * 255.0f);

            for (sy = 0; sy < scale; sy++)
            {
                for (sx = 0; sx < scale; sx++)
                {
                    int oy = ty * scale + sy;
                    int ox = tx * scale + sx;
                    pixels[oy * out_side + ox] = pixel;
                }
            }
        }
    }

    ret = stbi_write_png(out_path, out_side, out_side, 1, pixels, out_side);

    free(pixels);

    return ret ? 0 : -1;
}