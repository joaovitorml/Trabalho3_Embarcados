#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"

#include <string.h>

#include "../inc/http_client.h"
#include "../inc/json_parser.h"

#define TAG "HTTP"

char string[8000];
int tamanho = 0;

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{   
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            memset(string, 0, strlen(string));
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            memcpy(string + tamanho, evt->data, evt->data_len);
            tamanho += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            printf("tamanho: %d\n",tamanho);
            string[tamanho + 1] = '\0';
            if(string[2]=='i'){
                printf("ponto de controle 1\n");
                string_to_substrings_ipstack(string);
            }
            else{
                printf("ponto de controle 2\n");
                string_to_substrings_open_weather(string);
            }
            printf("ponto de controle 3\n");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void http_request(char link[]){
    esp_http_client_config_t config = {
        .url = link,
        .event_handler = _http_event_handle,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
    ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
    }
    esp_http_client_cleanup(client);
}