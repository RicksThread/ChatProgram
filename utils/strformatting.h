#include <stdlib.h>

#ifndef STRFORMATTING
#define STRFORMATTING

/*
formats a string that ends with a \n\0 which is usually returned by input streams;
the result is a string with the \n removed
*/
void format_linestr(char* str);

char** seperate_str_store(char* str, char* delimiter, int n);
#endif