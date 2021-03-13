#ifndef A_OTTO_H
#define A_OTTO_H

// https://www.cmrr.umn.edu/~strupp/serial.html
// https://github.com/banetl/sensor_reading/blob/master/cpp/sensor_reading.cc

#define OTTO_BASELINE 0.435  // Presa dalla piattaforma OTTO

#define OTTO_PORT "/dev/ttyUSB0"
#define OTTO_COMMS_SPEED B115200

#include <fcntl.h>
#include <pb_decode.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "otto_communication/otto_communication.pb.h"

// TODO Doxygen for AOtto_InitSerialComms()
// XXX Move to h_otto.c?
static inline int AOtto_InitSerialComms(void) {
    int fd;
    struct termios tty;
    // Open the port
    fd = open(OTTO_PORT, O_RDONLY | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("AOtto_InitSerialComms");
        exit(EXIT_FAILURE);
    }
    /*
    // Require exclusive access
    if (ioctl(fd, TIOCEXCL, NULL) < 0) {
        ... error handling...
    }
    */
    // Get the port's current configuration
    if (tcgetattr(fd, &tty) < 0) {
        perror("AOtto_InitSerialComms: tcgetattr");
        exit(EXIT_FAILURE);
    }

    // IFLAGS -- Turn off input processing
    tty.c_iflag &= ~(BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    tty.c_iflag |= IGNBRK | ISTRIP;

    /* OFLAGS -- Turn off output processing
     * Equates to:
     * tty.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
     *                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);
     */
    tty.c_oflag = 0;

    // LFLAGS -- Turn off canonical mode and line processing
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    // CFLAGS -- Turn off character processing and set input width to 8 bit
    tty.c_cflag &= ~(CSIZE | PARENB);
    tty.c_cflag |= CS8;

    // CC -- Read parameters
    tty.c_cc[VMIN] = 20; // Number of chars to before read() can return
    tty.c_cc[VTIME] = 0; // Delay between chars

    // Set the port's speed
    if (cfsetispeed(&tty, OTTO_COMMS_SPEED) < 0 ||
        cfsetospeed(&tty, OTTO_COMMS_SPEED) < 0) {
        perror("AOtto_InitSerialComms: cfsetispeed");
        exit(EXIT_FAILURE);
    }

    // Set the port's config
    if (tcsetattr(fd, TCSAFLUSH, &tty) < 0) {
        perror("AOtto_InitSerialComms: tcsetattr");
        exit(EXIT_FAILURE);
    }

    return fd;
}

void AOtto_ReadDeserialize(int fd, pb_byte_t* buf, StatusMessage msg);

#endif  // A_OTTO_H