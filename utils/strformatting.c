#include "strformatting.h"

void format_linestr(char* str)
{
    if (str[strlen(str)-1] == '\n')
    {
        printf("message returns a line... reformatting\n");
        str[strlen(str)-1] = '\0';
    }
}

char** seperate_str_store(char* str, char* delimiter, int n)
{
    int i = 0;
    char *p = strtok (str, delimiter);
    char **array = malloc(sizeof(long long) * n);

    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, delimiter);
    }

    return array;
}