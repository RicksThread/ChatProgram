#include "buffer_utils.h"

void set_buffer_header(char* buffer, size_t buffersz, buffer_header header)
{
    memcpy(&buffer[buffersz-sizeof(buffer_header)-1], &header, sizeof(buffer_header));
}

buffer_header extract_buffer_header(char* buffer, size_t buffersz)
{
    buffer_header header;
    memcpy(&header, &buffer[buffersz-sizeof(buffer_header)-1], sizeof(buffer_header));
    memset(&buffer[buffersz-sizeof(buffer_header)-1], 0, sizeof(buffer_header));

    return header;
}