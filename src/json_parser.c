#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

void string_to_json_ipstack(const char * const string)
{
    const cJSON * continent_name = NULL;
    const cJSON * country_name = NULL;
    const cJSON * region_name = NULL;
    const cJSON * city = NULL;
    cJSON *string_json = cJSON_Parse(string);

    char *string_ipstack = cJSON_Print(string_json);
    //printf("\n%s\n",string_ipstack);
    if (string_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    }

    continent_name = cJSON_GetObjectItemCaseSensitive(string_json, "continent_name");
    
}