/******************************************************************************
 * File: task_inference.c
 *
 * Consumer of inference_mailbox. Reactive task (not periodic): blocks
 * until task_process signals a new vector, runs network_predict(),
 * computes a simple confidence score, and reports the result.
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "task_inference.h"
#include "network.h"
#include "config.h"
#include "inference_log.h"
#include "time_utils.h"
#include "shared_result_client.h"

#include <stdio.h>
#include <time.h>

/******************************************************************************
 * Private helper functions
 ******************************************************************************/

/*
 * Simple confidence score: proportion of the total positive evidence
 * that belongs to the winning class. Negative raw outputs are treated
 * as "no evidence" (clamped to 0) since network_predict() does not
 * produce true probabilities.
 */
static float compute_confidence(const float output[OUTPUT_SIZE], int winner)
{    
    float sum = 0.0f;
    float winner_value;
    int k;
    
    for (k = 0; k < OUTPUT_SIZE; k++)
    {
        float v = output[k] > 0.0f ? output[k] : 0.0f;
        sum += v;
    }

    if (sum <= 0.0f)
    {
        return 0.0f;
    }

    winner_value = output[winner] > 0.0f ? output[winner] : 0.0f;

    return winner_value / sum;
}

/******************************************************************************
 * Task implementation
 ******************************************************************************/
void *task_inference(void *arg)
{
    /**************************************************************************
     * Local variables
     **************************************************************************/
    inference_mailbox_t *mb = (inference_mailbox_t *)arg;
    float vector[INPUT_SIZE];
    float output[OUTPUT_SIZE];
    char source_filename[FILENAME_MAX_LEN];
    int predicted_digit;
    float confidence;
    struct timespec t_start, t_end; /*to measure the time period*/

    printf("[INFERENCE] Task started.\n");

    while (1)
    {
        /* Block until task_process has a new vector ready */
        inference_mailbox_get(mb, vector, source_filename);

        /******* Start Measuring Computation Time **********/
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        predicted_digit = network_predict(vector, output);
        confidence = compute_confidence(output, predicted_digit);

        printf("[INFERENCE] %-20s --> Digit: %d (confidence: %.2f)\n",
       source_filename, predicted_digit, confidence);

        if (inference_log_add(source_filename, predicted_digit, confidence) != 0)
        {
            printf("[INFERENCE] Failed to write inference log.\n");
        }
        shared_result_client_write(predicted_digit);

        /******* End Measuring Computation Time **********/
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        printf("[INFERENCE] Task execution time: %.2f ms\n",
           timespec_diff_ms(t_start, t_end));
    }

    return NULL;
}