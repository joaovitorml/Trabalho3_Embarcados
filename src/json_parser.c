#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

#include "../include/json_parser.h"

double temp;
double temp_min;
double temp_max;
double umi;

double lati;
double longi;

void string_to_substrings_ipstack(char * string){

    //printf("Info completa: %s\n", string);
    cJSON * json = cJSON_Parse(string);
    
    lati=cJSON_GetObjectItemCaseSensitive(json, "latitude")->valuedouble;
    longi=cJSON_GetObjectItemCaseSensitive(json, "longitude")->valuedouble;

    printf("Latitude: %lf\n",lati);
    printf("Longitude: %lf\n\n",longi);

    cJSON_Delete(json);
}

void string_to_substrings_open_weather(char * string){

    //printf("Info completa: %s\n", string);
    cJSON * json = cJSON_Parse(string);
    cJSON * main_obj = cJSON_GetObjectItem(json, "main");

    temp=cJSON_GetObjectItemCaseSensitive(main_obj, "temp")->valuedouble;
    temp_min=cJSON_GetObjectItemCaseSensitive(main_obj, "temp_min")->valuedouble;
    temp_max=cJSON_GetObjectItemCaseSensitive(main_obj, "temp_max")->valuedouble;
    umi=cJSON_GetObjectItemCaseSensitive(main_obj, "humidity")->valuedouble;

    printf("\nTemperatura Atual: %lf\n",temp);
    printf("Temperatura Mínima: %lf\n",temp_min);
    printf("Temperatura Máxima: %lf\n",temp_max);
    printf("Umidade: %lf\n\n",umi);

    cJSON_Delete(json);
}