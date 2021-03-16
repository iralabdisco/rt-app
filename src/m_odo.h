#ifndef M_ODO_H
#define M_ODO_H

#include "config.h"

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "a_otto.h"
#define USE_BASELINE OTTO_BASELINE

#include "h_sched.h"
#include "h_time.h"

/**
 * @brief A datatype representing an arc
 *
 */
typedef double arc_t;  // TODO formal definition of "arc" in doxygen comment

/** @struct pose
 *  @brief A structure representing a pose, and the building block for the
 * #posebuf. In our representation, which comprises an x-y cartesian system
 * oriented like this:
 * \code
 *      ▲ y
 *      │
 *      │
 * ◄────┼─
 * x    │ 0
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
    nsec_t ts;
    double x, y, th;
    struct pose* next;
} pose_t;

/** @struct posebuf
 *  @brief A structure representing a two-pose circular buffer
 *  @var posebuf::old
 *  A pointer to the pose that was not updated in the last iteration
 *  @var posebuf::a
 *  The first cell of the buffer
 *  @var posebuf::b
 *  The second cell of the buffer
 */
typedef struct posebuf {
    pose_t* old;
    pose_t a;
    pose_t b;
} posebuf_t;

typedef struct odoStats {
    uint32_t targetIterations;
    uint32_t totalIterations;
    uint32_t clockMisses;
} odoStats_t;

void* MOdo_EntryPoint(void* p);

#endif  // M_ODO_H