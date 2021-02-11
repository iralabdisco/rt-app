#ifndef ODO_H
#define ODO_H

// #define DBG_TEST_TRACE 
#define MAX_ITERS 100
#define CUT_SHORT

// rant
// WHY DO I HAVE TO CHEAT INTELLISENSE!?!!
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
// rantover

#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "schedutil.h"

/**
 * @brief A datatype representing an arc
 *
 */
typedef double arc_t;  // TODO arc, what is this?

/** @struct pose
 *  @brief A structure representing a pose, and the building block for the
 * #posebuf. In our representation, which comprises an x-y cartesian system
 * oriented like this:
 * \code
 *      /\ y
 *      |
 *      |
 * <----+-
 * x    |0
 * \endcode
 *  @var pose::ts
 *  The timestamp at which the pose was created
 *  @var pose::x
 *  The current position on the \c x axis
 *  @var pose::y
 *  The current position on the \c y axis
 *  @var pose::th
 *  The angle (Theta) of attack in degrees
 *  @var pose::next
 *  A pointer to another stuct of type pose, used to simplify the construction
 * of the #posebuf
 */
typedef struct pose {
    double ts;
    double x, y, th;  // TODO naming
    struct pose* next;
} pose_t;

/** @struct posebuf
 *  @brief A structure representing a two-pose circular buffer
 *  @var posebuf::old
 *  A pointer to the pose that was not updated in the last iteration
 *  @var pose::a
 *  The first cell of the buffer
 *  @var pose::y
 *  The second cell of the buffer
 */
typedef struct posebuf {
    pose_t* old;
    pose_t a;
    pose_t b;
} posebuf_t;

typedef struct o_stats {
    uint32_t targetIterations;
    uint32_t totalIterations;
    uint32_t clockMisses;
} o_stats_t;

void* O_Worker(void* p);

#endif  // ODO_H