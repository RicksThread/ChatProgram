#include "error_handling.h"

result_check get_error(char* mess)
{
    result_check check;
    check.iserror = 1;
    check.error_mess = mess;
}
result_check get_error_sys(char* mess)
{
    result_check check;
    check.iserror = 1;
    check.error_mess = mess;
    strcat(check.error_mess, strerror(errno));
    return check;
}