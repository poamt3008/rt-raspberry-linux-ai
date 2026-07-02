#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"
#include "input_loader.h"

int main(int argc, char *argv[])
{
  

    float input[INPUT_SIZE];
    float out[OUTPUT_SIZE];

    int digit;

    if (argc != 2)
    {
        printf("Usage:\n");
        printf("    %s <input_file.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (input_loader_load(argv[1], input) != 0)
    {
        printf("Error loading input file.\n");
        return EXIT_FAILURE;
    }

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