#include "network.h"

#include <math.h>

/***********************************************************************
 * Realiza la inferencia de la red neuronal.
 *
 * Entrada:
 *      input : Vector normalizado de 784 elementos.
 *
 * Salida:
 *      Retorna el dígito reconocido (0-9).
 ***********************************************************************/
int network_predict(
    const float input[INPUT_SIZE],
    float out[OUTPUT_SIZE])
{
    float n[HIDDEN_SIZE];

    float m;

    int i;
    int j;
    int k;

    int digit;

    /*----------------------------------------------------------
     * Primera capa + función de activación
     *
     * MATLAB:
     * m = v' * x;
     * n = 2./(1+exp(-(m-c)./a))-1;
     *----------------------------------------------------------*/

    for (j = 0; j < HIDDEN_SIZE; j++)
    {
        /* Peso asociado al bias */
        m = V[INPUT_SIZE][j];

        /* Multiplicación entrada-pesos */
        for (i = 0; i < INPUT_SIZE; i++)
        {
            m += input[i] * V[i][j];
        }

        /* Función de activación */
        n[j] = 2.0f /
               (1.0f + expf(-(m - C[j]) / A[j]))
               - 1.0f;
    }

    /*----------------------------------------------------------
     * Segunda capa
     *
     * MATLAB:
     * out = w' * n;
     *----------------------------------------------------------*/

    for (k = 0; k < OUTPUT_SIZE; k++)
    {
        out[k] = 0.0f;

        for (j = 0; j < HIDDEN_SIZE; j++)
        {
            out[k] += n[j] * W[j][k];
        }
    }

    /*----------------------------------------------------------
     * Buscar la salida máxima
     *
     * MATLAB:
     * [~, clase] = max(out);
     *----------------------------------------------------------*/

    digit = 0;

    for (k = 1; k < OUTPUT_SIZE; k++)
    {
        if (out[k] > out[digit])
        {
            digit = k;
        }
    }

    return digit;
}