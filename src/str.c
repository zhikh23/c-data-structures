#include "str.h"

#include "err.h"
#include "debug.h"

int sfgets(char *buffer, const int buf_size, FILE *in) {
    if (fgets(buffer, buf_size, in) == NULL) {
        log_errorf("unexpected EOF");
        return EIO;
    }

    const size_t len = strnlen(buffer, buf_size);
    if (buffer[len - 1] != '\n') {
        log_errorf("buffer overflow");
        return EINVAL;
    }
    buffer[len - 1] = '\0';

    return 0;
}
