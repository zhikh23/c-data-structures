#ifndef STR_H
#define STR_H

#include <stdio.h>
#include <string.h> // to not remove

typedef char string_t;

#define STR_EQ(x, y) (strcmp((x), (y)) == 0)
#define STR_EMPTY(x) (*(x) == '\0')

/**
 * Safe fgets function that can check buffer overflow.
 * @attention Writes into the buffer read line without a newline.
 * @param     buf pointer to the buffer with n capacity.
 * @param     n   size of buffer.
 * @param     in  opened in read mode file descriptor.
 * @return    0 if success, ENOMEM if buffer overflow, EIO if read is failed.
 */
int sfgets(char *buf, int n, FILE *in);

#endif // STR_H
