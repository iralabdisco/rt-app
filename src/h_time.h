#ifndef H_TIME_H
#define H_TIME_H

#include <time.h>

// XXX Importante, vedi http://projects.ira.disco.unimib.it/issues/1123
// - Risoluzione Apparente (ad es. ms) vs. Risoluzione Effettiva (ad es. 55ms
// DOS)
// - Velocità della richiesta https://elinux.org/High_Resolution_Timers
// dà una risposta sommaria, il timer è basato sullo "jiffy", che è un'unità
// di misura di tempo interna del kernel che varia in base all'architettura.
// Possiamo inoltre fare `cat /proc/timer_list` per vedere una lista dei
// timer e delle loro capacità.

#define USE_CLOCK CLOCK_MONOTONIC_RAW

// Stolen from kernel's time.h
/* Parameters used to convert the timespec values: */
#define MSEC_PER_SEC 1000L
#define USEC_PER_MSEC 1000L
#define NSEC_PER_USEC 1000L
#define NSEC_PER_MSEC 1000000L
#define USEC_PER_SEC 1000000L
#define NSEC_PER_SEC 1000000000L
#define FSEC_PER_SEC 1000000000000000L

typedef struct timespec timespec_t;
typedef long long int nsec_t;

static nsec_t _timebase;  // TODO The implementation of timebase is yanky...
                          // it could use a refactor, as I'd like to
                          // initialize it just before launching the threads

/** @brief Convert timespec to nanoseconds
 *  @param ts pointer to the timespec variable to be converted
 *
 *  Returns the scalar nanosecond representation of the timespec
 *  parameter.
 */
static inline nsec_t HTime_TsToNs(const timespec_t* ts) {
    return ((nsec_t)ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static inline nsec_t HTime_GetNs(timespec_t* ts) {
    if (clock_gettime(USE_CLOCK, ts) == -1) {
        perror("HTime_GetNs:");
        exit(EXIT_FAILURE);
    }
    return HTime_TsToNs(ts);
}

static inline nsec_t HTime_GetNsDelta(timespec_t* ts) {
    return HTime_GetNs(ts) - _timebase;
}

static inline void HTime_InitBase(void) {
    timespec_t ts;
    _timebase = HTime_GetNs(&ts);
}

#endif  // H_TIME_H