#define _GNU_SOURCE

#include <assert.h>
#include <pthread.h>

<<<<<<< HEAD
#include "m_odo.h"
=======
#include "odo.h"
>>>>>>> parent of 1a05121... New build model, fixed minor issue in posebuf's brief, initial otto adapter skel

/** @enum workerIds
 * @brief An enumeration of the workers' IDs in the worker definitions array.
 * Note: insert all of the workers _before_ W_WORKERS, as it is used to
 * determine the workers array size.
 */
enum wid { W_ODO, W_WORKERS };

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

int main(void) {  // Dispatcher
    worker_t workers[W_WORKERS];
    workers[W_ODO].entry = MOdo_EntryPoint;
    for (int i = 0; i < W_WORKERS; i++) {
        // Dispatch workers
        pthread_create(&(workers[i].tid), NULL, workers[i].entry, NULL);
    }
    for (int i = 0; i < W_WORKERS; i++) {
        // Wait for worker termination
        pthread_join(workers[i].tid, NULL);
    }
    pthread_exit(0);  // Failsafe, not really needed
    return (EXIT_SUCCESS);
}