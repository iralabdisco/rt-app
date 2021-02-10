#ifndef ODO_H
#define ODO_H

#define DBG_TEST_TRACE

//rant
// WHY DO I HAVE TO CHEAT INTELLISENSE!?!!
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
//rantover

#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "schedutil.h"

typedef double arc_t;  // TODO arc, what is this?

typedef struct pose pose_t;

struct pose {
    double ts;
    double x, y, th;  // TODO naming
    pose_t* next;
};

typedef struct posebuf {
    pose_t* old;
    pose_t a;
    pose_t b;
} posebuf_t;

void* O_Worker(void* p);

#endif  // ODO_H