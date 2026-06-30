#ifndef NETWORK_H
#define NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "weights.h"

/**
 * @brief Realiza la inferencia de una imagen.
 *
 * @param input Vector de entrada normalizado (784 elementos).
 *
 * @return Número reconocido (0-9).
 */
int network_predict(
    const float input[INPUT_SIZE],
    float out[OUTPUT_SIZE]);

#ifdef __cplusplus
}
#endif

#endif