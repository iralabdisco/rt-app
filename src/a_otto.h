#ifndef A_OTTO_H
#define A_OTTO_H

#include <fcntl.h>
#include <pb_decode.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "config.h"
#include "otto_communication/otto_communication.pb.h"

// https://www.cmrr.umn.edu/~strupp/serial.html
// https://github.com/banetl/sensor_reading/blob/master/cpp/sensor_reading.cc

/** @brief Path to the character special device representing Otto's UART.
 * \n \b IMPORTANT make sure you check this before compiling. */
#define OTTO_PORT "/dev/ttyUSB0"

/** @brief The speed at which the computer and the UART communicate.
 * \n \b IMPORTANT make sure it's the same as the mobile platform's before
 * compiling. */
#define OTTO_COMMS_SPEED B115200

// The following defines were taken from Otto's code
// TODO Fix Doxygen comments

#define OTTO_BASELINE 0.435 /** @brief The baseline. Refer to otto */

#define OTTO_UNKNOWN 0
#define OTTO_WAITING_CONFIG 1
#define OTTO_READY 2
#define OTTO_RUNNING 3
#define OTTO_H_BRIDGE_FAULT_1 4
#define OTTO_H_BRIDGE_FAULT_2 5
#define OTTO_UNKNOWN_ERROR 6

int AOtto_Init(void);
void AOtto_ReadDeserialize(int fd, StatusMessage* msg);

#endif  // A_OTTO_H