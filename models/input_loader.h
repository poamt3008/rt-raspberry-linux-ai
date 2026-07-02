#ifndef INPUT_LOADER_H
#define INPUT_LOADER_H

/******************************************************************************
 * File: input_loader.h
 * Author: Ander
 *
 * Description:
 * Load neural network input vectors from external files.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "network.h"

/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * Load an input vector from a text file.
 *
 * Parameters:
 *      filename : Path to the input file.
 *      input    : Output vector.
 *
 * Returns:
 *      0  Success.
 *     -1  Error.
 */
int input_loader_load(
    const char *filename,
    float input[INPUT_SIZE]);

#endif