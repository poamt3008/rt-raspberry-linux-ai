/******************************************************************************
 * File: image_proc.h
 * Author: Ander
 *
 * Description:
 * Encapsulates the full image-to-vector pipeline: decode a JPEG photo
 * of a handwritten digit on white paper, and produce a normalized
 * vector compatible with network_predict().
 *
 * No other module should include stb_image.h directly.
 ******************************************************************************/
#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include "weights.h"  

/**
 * image_proc_extract - Convert a captured photo into a normalized
 * digit vector.
 *
 * @filename:     path to the JPEG image to process
 * @output:       destination array, must hold at least INPUT_SIZE floats
 * @digit_found:  set to 1 if a dark region (ink) was detected against
 *                the white background, 0 otherwise. When 0, @output
 *                is left zeroed -- the caller should not run inference
 *                on it.
 *
 * Return: 0 if the file was successfully loaded and processed
 * (regardless of whether a digit was found inside it), -1 if the
 * image could not be read/decoded at all.
 */
int image_proc_extract(const char *filename,
                        float output[INPUT_SIZE],
                        int *digit_found);

/**
 * image_proc_save_debug_png - Save a 28x28 input vector as a viewable
 * PNG image, upscaled for easy visual inspection (debugging only).
 *
 * @vector:   normalized [0,1] vector, same format as image_proc_extract() output
 * @out_path: destination .png path
 * @scale:    integer upscale factor (e.g. 10 -> 280x280 output)
 *
 * Return: 0 on success, -1 on failure.
 */
int image_proc_save_debug_png(const float vector[INPUT_SIZE],
                                const char *out_path,
                                int scale);

#endif /* IMAGE_PROC_H */