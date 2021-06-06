/**
 * @file a_otto.c
 * @author Jacopo Maltagliati (j.maltagliati@campus.unimib.it)
 * @brief The source of the adapter for the Otto mobile platform.
 *
 * @copyright This file is part of a project released under the European Union
 * Public License, see LICENSE and LICENSE.it for details.
 *
 */

#include "a_otto.h"

const VelocityCommand ottoWakeupCommands[2] = {{-0.1, 0}, {0, 0}};

/**
 * @brief Initialize the Otto mobile platform adapter, opening and configuring
 * the serial port.
 * @see https://www.cmrr.umn.edu/~strupp/serial.html
 *
 * @return The file descriptor of the character special device
 * representing Otto's UART.
 *
 * @throw open() If the mobile platform might is not connected, the device
 * specified by OTTO_PORT won't be available. There is no viable recovery
 * option, so the application will be terminated.
 *
 * @throw tcgetattr() The application was unable to retrieve the port's
 * current configuration: make sure you have the right permissions. The
 * application will be terminated.
 *
 * @throw cfsetispeed() The application was unable to set the port's
 * speed: make sure you have the right permissions. The
 * application will be terminated.
 *
 * @throw tcsetattr() The application was unable to set the port's
 * new configuration: make sure you have the right permissions. The
 * application will be terminated.
 */
int AOtto_Init(void) {
    int fd;
    struct termios tty;
    // Open the port
    fd = open(OTTO_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_Init");
#endif  // CONFIG_PRINT_ERRORS
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
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_Init: tcgetattr");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      /* 8-bit characters */
    tty.c_cflag &= ~PARENB;  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &=
        ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    // CC -- Read parameters
    tty.c_cc[VMIN] =
        0;  // Number of bytes before read() and write() can return
    tty.c_cc[VTIME] = 0;  // Timeout or delay

    // Set the port's speed
    if (cfsetispeed(&tty, OTTO_COMMS_SPEED) < 0 ||
        cfsetospeed(&tty, OTTO_COMMS_SPEED) < 0) {
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_Init: cfsetispeed");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }

    // Set the port's config
    if (tcsetattr(fd, TCSAFLUSH, &tty) < 0) {
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_Init: tcsetattr");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }

#ifdef OTTO_DTR_JUMPER_PRESENT
    // Reset the STM via DTR
    while (1) {
        int status = 0xFF;
        if (ioctl(fd, TIOCMGET, &status) != 0) {
            perror("Failed to get control line status");
            return 1;
        }

        printf("status: 0x%x\n", status);
        status ^= TIOCM_DTR;

        if (ioctl(fd, TIOCMSET, status) != 0) {
            perror("Failed to set control line status");
        }

        usleep(500000);
    }
#endif
    sleep(1);  // wait for serial port to become ready

    return fd;
}

/**
 * @brief Read a specified number of bytes from a serial port, specified by
 * `fd`, into a buffer
 *
 * @param fd The character special device representing Otto's UART.
 * @param bytes The maximum number of bytes that are going to be read.
 * @param buf The buffer where the bytes received from the serial port are
 * going to be stored.
 *
 * @return The number of bytes that have been successfully read.
 *
 * @throw read() If the peripheral is detached, it will stop responding
 * and the `read()` call will fail. Since we have no recovery behavior defined
 * on the mobile platform the application will be terminated but be noted that
 * this is not an acceptable recovery behavior in a real-world scenario.
 */
int AOtto_Read(int fd, int bytes, pb_byte_t* buf) {
    int res = read(fd, buf, bytes);
    if (res < 0) {
        // TODO recovery, the system might stop responding and we need
        // some fallback logic
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_Read");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }
    return res;
}

/**
 * @brief Deserialize a data packet stored into a buffer and turn it into a
 * StatusMessage.
 *
 * @param buf The buffer where the packet is stored.
 * @param bytes The length of the encoded packet.
 * @param msg The decoded message.
 *
 * @throw pb_decode() Due to communication issues, a message might not get
 * decoded correctly. If that happens we clear the `StatusMessage` struct, set
 * the `status` field to `OTTO_UNKNOWN_ERROR`, and return. \b NOTE the
 * behavior has been changed and now the program will be killed on failure to
 * decode
 */
void AOtto_Deserialize(pb_byte_t* buf, int bytes, StatusMessage* msg) {
    // TODO document change in behavior
    pb_istream_t stream = pb_istream_from_buffer(buf, bytes);

    if (!pb_decode(&stream, StatusMessage_fields, msg)) {
#if CONFIG_OTTO_DESER_OLD_BEHAVIOR
        msg->angular_velocity = 0;
        msg->delta_millis = 0;
        msg->linear_velocity = 0;
        msg->status = OTTO_UNKNOWN_ERROR;
#if CONFIG_ENABLE_LOGGING
        syslog(LOG_ERR, "%s%s",
               "(A_OTTO_IN) NanoPB: ", PB_GET_ERROR(&stream));
#endif  // CONFIG_ENABLE_LOGGING
#else
        printf("%s%s", "(A_OTTO_IN) NanoPB: ", PB_GET_ERROR(&stream));
        printf("\nBuffer trace:\n<");
        for (int i = 0; i < bytes; i++) {
            printf("%02X ", buf[i]);
        }
        printf(">\n");
        exit(EXIT_FAILURE);
#endif  // CONFIG_OTTO_DESER_OLD_BEHAVIOR
    }
    return;
}

/**
 * @brief
 *
 * @param fd
 * @param cmd
 */
// TODO document Aotto_SerializeWrite
void AOtto_SerializeWrite(int fd, VelocityCommand cmd) {
    static pb_byte_t buf[10];
    pb_ostream_t stream = pb_ostream_from_buffer(buf, sizeof(buf));
    if (!pb_encode(&stream, VelocityCommand_fields, &cmd)) {
#if CONFIG_ENABLE_LOGGING
        syslog(LOG_ERR, "%s%s",
               "(A_OTTO_OUT) NanoPB: ", PB_GET_ERROR(&stream));
#endif  // CONFIG_ENABLE_LOGGING
    }
    int res = 0;
    while (res < 10) {
        res = write(fd, buf, stream.bytes_written);
    }
    if (res < 0) {
        // TODO recovery, the system might stop responding and we need
        // some fallback logic
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_SerializeWrite");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }
    usleep(res * 100);  // Wait for the write to go through
    // printf("W_OK %i\n", res);
}