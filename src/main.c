#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../include/wifi.h"
#include "../include/http_client.h"
#include "../include/led_control.h"

#define IPSTACK_KEY CONFIG_IPSTACK_API_KEY
#define OPEN_WEATHER_KEY CONFIG_OPEN_WEATHER_API_KEY

xSemaphoreHandle conexaoWifiSemaphore;

TaskHandle_t led_handler = NULL;

extern double lati;
extern double longi;

void RealizaHTTPRequest(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      ESP_LOGI("Main Task", "Realiza HTTP Request");
      while(1){

        vTaskDelay(4000 / portTICK_PERIOD_MS);
        char ipstack_url[100] = "";
        strcat(ipstack_url, "http://api.ipstack.com/189.6.35.88?access_key=");
        strcat(ipstack_url, IPSTACK_KEY);
        printf("\nConectando a: %s\n\n",ipstack_url);
        xTaskNotifyGive(led_handler);
        http_request(ipstack_url);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        char open_weather_url[200] = "";
        sprintf(open_weather_url,"http://api.openweathermap.org/data/2.5/weather?lat=%lf&lon=%lf&appid=%s",
            lati, longi, OPEN_WEATHER_KEY);
        printf("\nConectando a: %s\n\n",open_weather_url);
        xTaskNotifyGive(led_handler);
        http_request(open_weather_url);
      }
    }
  }
}

void configure_led(void *params)
{
    led_control();
}

void app_main(void)
{
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    xTaskCreate(&configure_led, "Configura LED", 10000, NULL, 1, &led_handler);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    wifi_start();
    xTaskCreate(&RealizaHTTPRequest,  "Processa HTTP", 10000, NULL, 1, NULL);
}