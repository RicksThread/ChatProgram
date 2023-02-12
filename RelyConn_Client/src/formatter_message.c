#include "formatter_message.h"

//these functions are not reentrant
void build_message(builder_message* builder, char* message, int type)
{
    switch(type)
    {
        case CMD_MESSAGE_VALUE:
        break;
        case CMD_KICK_VALUE:
        break;
    }


    char tmp_message[MESSAGE_USER_LENGTH];
    strcpy(tmp_message, message);
    message[0] = type;
    strcpy(message+1, tmp_message);
}

void format_message(formatter_message* formatter, char* kickmessage)
{

}
