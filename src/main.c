#define _GNU_SOURCE

#include <pthread.h>

#include "odo.h"

int main(int argc, char** argv) {  // Dispatcher
    pthread_t odoThread;
    pthread_create(&odoThread, NULL, O_Worker, NULL);
    pthread_join(odoThread, NULL);  // Wait for worker termination
    pthread_exit(0); // Failsafe
    return (EXIT_SUCCESS);
}