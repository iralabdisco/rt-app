#define _GNU_SOURCE

#define WORKERS 1  // ODO, ...

#include <assert.h>
#include <pthread.h>

#include "odo.h"

/**
 * @enum workerIds
 * @brief An enumeration of the workers' IDs in the worker definitions array
 */
enum { W_ODO };

/** @struct worker
 *  @brief A structure representing a worker thread
 *  @var worker::tid
 *  The POSIX Thread ID
 *  @var worker::entry
 *  The entry point of the worker thread
 */
typedef struct worker {
    pthread_t tid;
    void* (*entry)(void*);
} worker_t;

int main(void) { //int argc, char** argv) {  // Dispatcher
    // Set-up
    worker_t workers[WORKERS]; // Worker definitions array
    workers[W_ODO].entry = O_Worker;
    // Dispatch
    for (int i = 0; i < WORKERS; i++) {
        pthread_create(&(workers[i].tid), NULL, workers[i].entry, NULL);
    }
    // Wait for worker termination
    for (int i = 0; i < WORKERS; i++) {
        pthread_join(workers[i].tid, NULL);
    }
    pthread_exit(0);  // Failsafe
    return (EXIT_SUCCESS);
}