#ifndef ERROR_CHECKING_H
#define ERROR_CHECKING_H

#include "errno.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct
{
    char* error_mess;
    int iserror;
}result_check;

result_check get_error(char* mess);
result_check get_error_sys(char* mess);

#endif