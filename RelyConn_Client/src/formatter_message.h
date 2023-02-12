#ifndef STDRELY_MESSAGE_H
#define STDRELY_MESSAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdrely_message.h"

typedef struct
{
    void (*build_usermessage)(char*);
    void (*build_kickmessage)(char*);
}builder_message;

typedef struct
{
    void (*formatter_usermessage)(char*);
    void (*formatter_kickmessage)(char*);
}formatter_message;

//these functions are not reentrant
void build_message(builder_message* builder, char* message, int type);
void format_message(formatter_message* formatter, char* kickmessage);

#endif