#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_control.h"

#define LED 2

void led_control(void *params)
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
                printf("LED piscando. Requisição feita.\n");
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