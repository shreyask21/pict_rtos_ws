/*
* FreeRTOS Producer Consumer
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"

// Declare functions
void IRAM_ATTR Producer_Task(void *);
void IRAM_ATTR Consumer_Task(void *);

// Declare Semaphore to communicate between tasks
bool IRAM_ATTR Mutex_Flag = false;

// Declare pointer to store allocated memory location
void *IRAM_ATTR Memory_Pointer = NULL;
// Declare handles for tasks
TaskHandle_t Producer_Task_Handle, Consumer_Task_Handle;

void app_main(void)
{
    printf("app_main Started\n");
    printf("app_main Running on Core: %d\n", xPortGetCoreID());
    for (;;)
    {
        // Init LED GPIO pin
        gpio_reset_pin(2);
        gpio_set_direction(2, GPIO_MODE_OUTPUT);
        // Start and pin consumer task to second core
        xTaskCreatePinnedToCore(Consumer_Task, "Consumer", 10000, NULL, 0, &Consumer_Task_Handle, 1);
        // Start and pin producer task to first core
        xTaskCreatePinnedToCore(Producer_Task, "Producer", 10000, NULL, 0, &Producer_Task_Handle, 0);
    }
}

void IRAM_ATTR Producer_Task(void *pvParameters)
{

    if (!Mutex_Flag)
    {
        // Raise the busy flag
        Mutex_Flag = true;
        // Turn on the LED
        gpio_set_level(2, 1);

        printf("Producer Task Started\n");
        printf("Producer Task Running on Core: %d\n", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Allocating 1K Memory\n");
        Memory_Pointer = malloc(0x400);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Current Heap Size: %d bytes\n", esp_get_minimum_free_heap_size());
        printf("Producer Task Ended\n");

        // Turn off the LED
        gpio_set_level(2, 0);
    }
    // Lower the busy flag
    Mutex_Flag = false;
}

void IRAM_ATTR Consumer_Task(void *pvParameters)
{

    if (!Mutex_Flag)
    {
        // Raise the busy flag
        Mutex_Flag = true;
        // Turn on the LED
        gpio_set_level(2, 1);

        printf("Consumer Task Started\n");
        printf("Consumer Task Running on Core: %d\n", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (Memory_Pointer == NULL)
        {
            printf("Memory is Not Allocated Yet...\n");
        }
        else
        {
            printf("Deallocationg Memory\n");
            free(Memory_Pointer);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Current Heap Size: %d bytes\n", esp_get_minimum_free_heap_size());
        printf("Consumer Task Ended\n");

        // Turn off the LED
        gpio_set_level(2, 0);
    }
    // Lower the busy flag
    Mutex_Flag = false;
}
