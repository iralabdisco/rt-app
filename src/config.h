/**
 * @file config.h
 * @author Jacopo Maltagliati (j.maltagliati@campus.unimib.it)
 * @brief A header containing defines used to change the behavior of the
 * program.
 *
 * @copyright This file is part of a project released under the European Union
 * Public License, see LICENSE and LICENSE.it for details.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

/** @brief Terminate the Odometry thread after a number of iterations. Use in
 * conjunction with `CONFIG_MAX_ITERS`. */
#define CONFIG_CUT_SHORT 0
/** @brief Define the number of iterations after which the Odometry thread
 * must be terminated. Only used if `CONFIG_CUT_SHORT` is set to `1`. */
#define CONFIG_MAX_ITERS 100

#define CONFIG_PRINT_ERRORS 1
#define CONFIG_PRINT_MSG_VALUES 0

#define CONFIG_OTTO_DESER_OLD_BEHAVIOR 1

#define CONFIG_OTTO_DEMO 1

/**
 * @brief Enable logging via `syslog()` (see `man syslog(3)` for details).
 * System logging should be more effective and fast than using `printf()` (see
 * https://bytefreaks.net/programming-2/cc-comparing-the-performance-of-syslog-vs-printf)
 * but it does still perform a potentially blocking call (it `write()`s to the
 * desired log, or the default one in our case). A potential solution could be
 * Facebook's `liblogfaf` https://github.com/facebookarchive/liblogfaf, but
 * this still needs to be confirmed.
 */
#define CONFIG_ENABLE_LOGGING 1

#endif  // CONFIG_H