/******************************************************************************
 * File: shared_result_client.c
 ******************************************************************************/
#include "shared_result_client.h"
#include "compartido.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/proyecto_rt"

static SharedData *shared = NULL;
static int         initialized = 0;
static int         warned_once = 0;

int shared_result_client_init(void)
{
    int fd;

    fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0)
    {
        perror("shared_result_client_init: shm_open");
        return -1;
    }

    shared = mmap(NULL, sizeof(SharedData),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, fd, 0);

    close(fd);

    if (shared == MAP_FAILED)
    {
        perror("shared_result_client_init: mmap");
        shared = NULL;
        return -1;
    }

    initialized = 1;
    printf("[SHARED_RESULT] Connected to shared memory: %s\n", SHM_NAME);

    return 0;
}

void shared_result_client_write(int digit)
{
    if (!initialized)
    {
        if (!warned_once)
        {
            printf("[SHARED_RESULT] Not connected -- skipping write "
                   "(is Process 1 running?)\n");
            warned_once = 1;
        }
        return;
    }

    sem_wait(&shared->semaforo);

    shared->numero_reconocido = digit;
    shared->resultado_listo = 1;

    sem_post(&shared->semaforo);
}

void shared_result_client_close(void)
{
    if (initialized)
    {
        munmap(shared, sizeof(SharedData));
        shared = NULL;
        initialized = 0;
    }
}