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
#include "driver/gpio.h"

#include "../inc/wifi.h"
#include "../inc/http_client.h"

#define LED 2
#define IPSTACK_KEY CONFIG_IPSTACK_API_KEY
#define OPEN_WEATHER_KEY CONFIG_OPEN_WEATHER_API_KEY

xSemaphoreHandle conexaoWifiSemaphore;

TaskHandle_t led_handler = NULL;

void RealizaHTTPRequest(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      ESP_LOGI("Main Task", "Realiza HTTP Request");
      while(1){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        char ipstack_url[100] = "";
        strcat(ipstack_url, "http://api.ipstack.com/189.6.35.88?access_key=");
        strcat(ipstack_url, IPSTACK_KEY);
        printf("%s\n",ipstack_url);
        http_request(ipstack_url);

        vTaskDelay(3000 / portTICK_PERIOD_MS);
        char open_weather_url[100] = "";
        strcat(open_weather_url, "http://api.openweathermap.org/data/2.5/weather?lat=-15.865970&lon=-47.877022&appid=");
        strcat(open_weather_url, OPEN_WEATHER_KEY);
        printf("%s\n",open_weather_url);
        http_request(open_weather_url);
      }
    }
  }
}

void configure_led(void *params)
{
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    int estado = 0;
    while (true)
    {    
        if(!ulTaskNotifyTake(pdTRUE, 1000 / portTICK_PERIOD_MS)){
          printf("LED piscando. Conectando ao WI-FI...\n");
          gpio_set_level(LED, estado);
          estado = !estado;
        }
        else{
            while(true){
              if(ulTaskNotifyTake(pdTRUE, 1000 / portTICK_PERIOD_MS)){
                printf("LED piscando\n");
                gpio_set_level(LED, estado);
                estado = !estado;
              }
              else{
                printf("LED aceso. WI-FI está conectado\n");
                estado = 1;
                gpio_set_level(LED, estado);
              }
          }
        }
    }
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

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    wifi_start();
    xTaskCreate(&RealizaHTTPRequest,  "Processa HTTP", 10000, NULL, 1, NULL);
}
