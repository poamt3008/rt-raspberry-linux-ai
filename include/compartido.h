#ifndef COMPARTIDO_H
#define COMPARTIDO_H

#include <pthread.h>
#include <semaphore.h>

// ─────────────────────────────────────────────
//  Configuración general
// ─────────────────────────────────────────────
#define INTERVALO_SEGUNDOS  3    // cada cuántos segundos cambia el número
#define NUM_MIN             0    // número mínimo a mostrar
#define NUM_MAX             9    // número máximo a mostrar

// ─────────────────────────────────────────────
//  Datos en Shared Memory (entre procesos)
//  Este struct lo ven AMBOS procesos
// ─────────────────────────────────────────────
typedef struct {
    int numero_mostrado;      // Proceso 1 escribe: "estoy mostrando este número"
    int numero_reconocido;    // Proceso 2 escribe: "la IA reconoció este número"
    int resultado_listo;      // flag: 1 = hay resultado nuevo, 0 = nada nuevo
    sem_t semaforo;           // sincroniza acceso entre los dos procesos
} SharedData;

// ─────────────────────────────────────────────
//  Estado interno del Proceso 1
//  Este struct lo ven los hilos DE TU PROCESO
// ─────────────────────────────────────────────
typedef struct {
    int numero_actual;        // número elegido por el generador
    int hay_cambio;           // flag: 1 = el generador eligió un número nuevo
    pthread_mutex_t mutex;    // protege el acceso entre tus hilos
    SharedData *shared;       // puntero a la shared memory (proceso 2)
} EstadoInterno;

#endif