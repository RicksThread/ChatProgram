#include "strformatting.h"

void format_linestr(char* str)
{
    if (str[strlen(str)-1] == '\n')
    {
        str[strlen(str)-1] = '\0';
    }
}

str_array split_str(char* str, const char* delimiter)
{
    str_array strarr;
    strarr.length = -1;
    if (str[0] == NULL)
        return strarr;
    int i = 0;
    char* save_ptr;

    char* tmp_str = malloc((strlen(str)+1));
    strcpy(tmp_str, str);
    
    char* p = strtok_r(tmp_str, delimiter, &save_ptr);

    int length_limiter = strlen(delimiter);

    //get the number of tokens
    int offset = 0;
    int ntokens = 1;
    while(*(str+offset) != NULL)
    {
        if (strncmp(str+offset, delimiter, length_limiter) == 0)
            ntokens++;
        
        offset++;
    }

    //create the array of strings and populate it
    char** array = malloc(sizeof(char*) * ntokens);

    while (p != NULL)
    {
        array[i] = malloc(strlen(p)+1);
        strcpy(array[i], p);
        i++;
        p = strtok_r (NULL, delimiter, &save_ptr);
    }

    strarr.length = ntokens;
    strarr.strs = array;
    
    free(tmp_str);
    return strarr;
}

u_int8_t is_str_array_empty(str_array* str_arr)
{
    if (str_arr == NULL)  
        return 1;
    
    if (str_arr->length <= 0)
        return 1;
    
    return 0;
}

void free_str_array(str_array* str_array)
{
    if (is_str_array_empty(str_array))
        return;
    str_array->length = -1;

    for(int i = 0; i < str_array->length; i++)
    {
        free(str_array->strs[i]);
    }
    free(str_array->strs);
}