#include "a_otto.h"

// TODO Doxygen for AOtto_ReadDeserialize()
void AOtto_ReadDeserialize(int fd, pb_byte_t* buf, StatusMessage msg) {
    int res = read(fd, buf, 128);
    if (res < 0) {
        // TODO recovery, the system might stop responding and we need
        // some fallback logic
        perror("AOtto_ReadDeserialize");
        // exit(EXIT_FAILURE);
    }
    buf[res] = 0;

    pb_istream_t stream = pb_istream_from_buffer(buf, 20);

    if (!pb_decode(&stream, StatusMessage_fields, &msg)) {
        // TODO recovery, the message might be garbled and we need
        // some fallback logic
        printf("AOtto_ReadDeserialize: %s\n", PB_GET_ERROR(&stream));
        // exit(EXIT_FAILURE);
    }
    return;
}