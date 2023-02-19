#include <stdlib.h>
#include <string.h>

#ifndef STRFORMATTING
#define STRFORMATTING

typedef struct str_array
{
    int length;
    char** strs;
}str_array;


/*
formats a string that ends with a \n\0 which is usually returned by input streams (I.E scanf or get).
The result is a string with the \n replaced by \0
*/
void format_linestr(char* str);

/*
splits a string where the character is the same as the delimiter; in doing so it allocates in dynamic memory (TO FREE!)
*/
str_array split_str(char* str, const char* delimiter);

u_int8_t is_str_array_empty(str_array* str_arr);

/*
free a str array
*/
void free_str_array(str_array* str_array);


#endif