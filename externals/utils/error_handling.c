#include "error_handling.h"

result_check get_error(char* mess)
{
    result_check check;
    check.iserror = 1;
    check.error_mess = mess;

    return check;
}
result_check get_error_sys(char* mess)
{
    result_check check;
    check.iserror = 1;

    char mess_arr[100];
    check.error_mess = mess_arr;
    fprintf(check.error_mess, "%s; %s", mess, strerror(errno));
    return check;
}