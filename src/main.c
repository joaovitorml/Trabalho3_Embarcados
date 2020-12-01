#include <stdio.h>
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

xSemaphoreHandle conexaoWifiSemaphore;

TaskHandle_t led_handler = NULL;

void RealizaHTTPRequest(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      ESP_LOGI("Main Task", "Realiza HTTP Request");
      http_request("http://api.ipstack.com/189.6.35.88?access_key=12b592c8b219620b2b8dbbd0ddc3f3a5");
      //https_request();
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
          printf("Pisca LED\n");
          gpio_set_level(LED, estado);
          estado = !estado;
        }
        else{
          while(!ulTaskNotifyTake(pdTRUE, 5000 / portTICK_PERIOD_MS)){
            printf("LED aceso, está conectado\n");
            estado = 1;
            gpio_set_level(LED, estado);
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

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    wifi_start();
    xTaskCreate(&RealizaHTTPRequest,  "Processa HTTP", 20000, NULL, 1, NULL);
}
