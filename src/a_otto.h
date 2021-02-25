#ifndef A_OTTO_H
#define A_OTTO_H

#define OTTO_BASELINE 0.435
/* Federica Di Lauro, [06.02.21 15:08]
 * [In reply to Federica Di Lauro]                    \
 * questo è quello di otto, non ricordo cosa venisse \
 *fuori per l'esercizio di matlab                     \
 **/

#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "deps/nanopb-0.4.4/pb_decode.h"
#include "deps/nanopb-0.4.4/pb_encode.h"

#ifdef ENABLE_AOTTO

// https://github.com/banetl/sensor_reading/blob/master/cpp/sensor_reading.cc
int AOtto_InitSerialComms(const char* port, int baud, int timeout) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    struct termios settings;
    speed_t baudrate = baud;

    tcgetattr(fd, &settings);
    cfsetispeed(&settings, baudrate);

    settings.c_cflag &= ~PARENB; /* no parity */
    settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
    settings.c_cflag &= ~CSIZE;
    settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
    /* echo off, echo newline off, canonical mode off,
     * extended input processing off, signal chars off
     */
    settings.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    settings.c_oflag = 0; /* Turn off output processing */
    settings.c_cflag &= ~CRTSCTS;
    /* Read settings :
     * To understand the settings have a look at man 3 termios
     * ``Canonical and noncanonical mode`` section
     */
    settings.c_cc[VMIN] = 1;             /* read doesn't block */
    settings.c_cc[VTIME] = timeout * 10; /* read timeout in ds */
    settings.c_cflag |= CREAD;

    /* Flush Port, then applies attributes */
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &settings) < 0) /* apply the settings */
        return -1;

    return fd;
}

void* AOtto_RecvMsg() {
    char* port = "/dev/ttyUSB1000";
    int baud = 9600;
    int timeout = 0;
    int fd = open_port(port, baud, timeout);
    if (fd == -1) {
        perror("A_Otto_RecvMsg():");
        exit(EXIT_FAILURE);
    }
}

#endif

#endif  // A_OTTO_H