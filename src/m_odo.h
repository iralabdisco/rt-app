/**
 * @file m_odo.h
 * @author Jacopo Maltagliati (j.maltagliati@campus.unimib.it)
 * @brief The header of the Odometry thread worker.
 *
 * @copyright This file is part of a project released under the European Union
 * Public License, see LICENSE and LICENSE.it for details.
 *
 */

#ifndef M_ODO_H
#define M_ODO_H

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "a_otto.h"
#include "config.h"
#include "h_sched.h"
#include "h_time.h"

#define USE_BASELINE OTTO_BASELINE
#define ODO_RUNTIME 10 * NSEC_PER_MSEC
#define ODO_PERIOD 20 * NSEC_PER_MSEC
#define ODO_DEADLINE 11 * NSEC_PER_MSEC
#define ODO_DELTA_WARN_THRESHOLD 500 * USEC_PER_MSEC

/**
 * @brief A datatype representing an arc
 * lo spostamento nello spazio del punto di contatto (in metri, letteralmente
 * l'arco di circ. (centrata in CIR) tra il punto di contatto al tempo t-1 e
 * il punto di contatto al tempo t. Se andiamo dritti il raggio è infinito)
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
 *  The current position on the `x` axis
 *  @var pose::y
 *  The current position on the `y` axis
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