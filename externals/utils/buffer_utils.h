#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

#include <stdlib.h>
#include <bits/types.h>
#include <string.h>

typedef struct
{
    size_t message_length;

}buffer_header;

void set_buffer_header(char* buffer, size_t buffersz, buffer_header header);
buffer_header extract_buffer_header(char* buffer, size_t buffersz);

#endif