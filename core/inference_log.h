/******************************************************************************
 * File: inference_log.h
 * Author: Ander
 *
 * Description:
 * Circular log of the last LOG_MAX_ENTRIES inference results, written
 * as a CSV file that gets fully rewritten on each new entry. Designed
 * so an external process (e.g. a display script) can read the current
 * state at any time without needing to parse an ever-growing file.
 ******************************************************************************/
#ifndef INFERENCE_LOG_H
#define INFERENCE_LOG_H

#include "config.h"

/**
 * inference_log_add - Record a new inference result.
 *
 * Internally keeps the last LOG_MAX_ENTRIES entries in memory (oldest
 * entries are dropped first) and rewrites LOG_FILE_PATH from scratch
 * on every call, so the file on disk always reflects the current
 * window of recent results.
 *
 * Thread-safe: safe to call from task_inference without any external
 * locking.
 *
 * @photo_filename:   source image filename (e.g. "image003.jpg")
 * @predicted_digit:  digit predicted by network_predict() (0-9)
 * @confidence:       confidence score in [0,1]
 *
 * Return: 0 on success, -1 if the file could not be written.
 */
int inference_log_add(const char *photo_filename,
                       int predicted_digit,
                       float confidence);

#endif /* INFERENCE_LOG_H */