#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"

int main(int argc, char *argv[])
{
    FILE *fp;

    float input[INPUT_SIZE];
    float out[OUTPUT_SIZE];

    int digit;
    int i;

    if (argc != 2)
    {
        printf("Uso:\n");
        printf("    %s <archivo.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    for (i = 0; i < INPUT_SIZE; i++)
    {
        if (fscanf(fp, "%f", &input[i]) != 1)
        {
            printf("Error leyendo el archivo.\n");
            fclose(fp);
            return EXIT_FAILURE;
        }
    }

    fclose(fp);

    digit = network_predict(input, out);

    /*----------------------------------------------------------
    * Obtener el dígito esperado desde el nombre del archivo
    *----------------------------------------------------------*/
    char *filename;
    int expected_digit;

    filename = strrchr(argv[1], '/');

    if (filename != NULL)
    {
        filename++;
    }
    else
    {
        filename = argv[1];
    }

    if (sscanf(filename, "image_%d.txt", &expected_digit) != 1)
    {
        printf("No fue posible obtener el numero esperado.\n");
        return EXIT_FAILURE;
    }
    expected_digit /= 100;

    printf("\n=========================================\n");
    printf("        Test de Red Neuronal\n");
    printf("=========================================\n");

    printf("Archivo            : %s\n", filename);
    printf("Esperado           : %d\n", expected_digit);
    printf("Identificado       : %d\n", digit);

    if (digit == expected_digit)
    {
        printf("Resultado          : OK\n");
    }
    else
    {
        printf("Resultado          : ERROR\n");
    }

    printf("=========================================\n\n");

    return EXIT_SUCCESS;
}