#include "a_otto.h"

/**
 * @brief Initialize the Otto mobile platform adapter, opening and configuring
 * the serial port.
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
    fd = open(OTTO_PORT, O_RDONLY | O_NOCTTY | O_SYNC);
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
    tty.c_cc[VMIN] = 20;  // Number of chars to before read() can return
    tty.c_cc[VTIME] = 0;  // Delay between chars

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
    return fd;
}

/**
 * @brief Read a data packet from the Otto mobile platform and deserialize it,
 * turning it into a StatusMessage struct.
 *
 * @param fd The character special device representing Otto's UART.
 * @param msg The decoded message.
 *
 * @throw read() If the peripheral is detached, it will stop responding
 * and the read() call will fail. Since we have no hardware recovery method
 * the application will be terminated but be noted that this is not an
 * acceptable recovery behavior in a real-world scenario.
 *
 * @throw pb_decode() Due to timing issues, a message might not get
 * decoded correctly. If that happens, clear the StatusMessage struct and set
 * the StatusMessage::status field to OTTO_UNKNOWN_ERROR and return.
 */
void AOtto_ReadDeserialize(int fd, StatusMessage* msg) {
    static pb_byte_t buf[128];
    int res = read(fd, buf, 128);
    if (res < 0) {
        // TODO recovery, the system might stop responding and we need
        // some fallback logic
#ifdef CONFIG_PRINT_ERRORS
        perror("AOtto_ReadDeserialize");
#endif  // CONFIG_PRINT_ERRORS
        exit(EXIT_FAILURE);
    }
    buf[res] = 0;

    pb_istream_t stream = pb_istream_from_buffer(buf, 20);

    if (!pb_decode(&stream, StatusMessage_fields, msg)) {
        msg->angular_velocity = 0;
        msg->delta_millis = 0;
        msg->linear_velocity = 0;
        msg->status = OTTO_UNKNOWN_ERROR;
#if CONFIG_ENABLE_LOGGING
        syslog(LOG_ERR, "%s%s", "(A_OTTO) NanoPB: ", PB_GET_ERROR(&stream));
#endif  // CONFIG_ENABLE_LOGGING
    }
    return;
}