/*
* FreeRTOS Producer Consumer
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"

// Declare functions
void Producer_Task(void *);
void Consumer_Task(void *);

// Declare Semaphore to communicate between tasks
bool Mutex_Flag = false;

// Declare pointer to store allocated memory location
void *Memory_Pointer = NULL;
// Declare handles for tasks
TaskHandle_t Producer_Task_Handle, Consumer_Task_Handle;

void app_main(void)
{
    printf("app_main Started\n");
    printf("app_main Running on Core: %d\n", xPortGetCoreID());

    // Init LED GPIO pin
    gpio_reset_pin(2);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    // Start and pin consumer task to second core
    xTaskCreatePinnedToCore(Consumer_Task, "Consumer", 10000, NULL, 0, &Consumer_Task_Handle, 1);
    // Start and pin producer task to first core
    xTaskCreatePinnedToCore(Producer_Task, "Producer", 10000, NULL, 0, &Producer_Task_Handle, 0);
}

void Producer_Task(void *pvParameters)
{
    for (;;)
    {
        if (!Mutex_Flag)
        {
            // Raise the busy flag
            Mutex_Flag = true;
            // Turn on the LED
            gpio_set_level(2, 1);

            printf("\nProducer Task Started\n");
            printf("Producer Task Running on Core: %d\n", xPortGetCoreID());
            printf("Allocating 1K Memory\n");
            Memory_Pointer = malloc(0x400);
            printf("Current Heap Size: %d bytes\n", esp_get_free_heap_size());
            printf("Producer Task Ended\n");
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            // Turn off the LED
            gpio_set_level(2, 0);
        }
        // Lower the busy flag
        Mutex_Flag = false;
    }
}

void Consumer_Task(void *pvParameters)
{
    for (;;)
    {
        if (!Mutex_Flag)
        {
            // Raise the busy flag
            Mutex_Flag = true;
            // Turn on the LED
            gpio_set_level(2, 1);

            printf("\nConsumer Task Started\n");
            printf("Consumer Task Running on Core: %d\n", xPortGetCoreID());
            if (Memory_Pointer == NULL)
            {
                printf("Memory is Not Allocated Yet...\n");
            }
            else
            {
                printf("Deallocationg Memory\n");
                free(Memory_Pointer);
                Memory_Pointer=NULL;
            }
            printf("Current Heap Size: %d bytes\n", esp_get_free_heap_size());
            printf("Consumer Task Ended\n");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            // Turn off the LED
            gpio_set_level(2, 0);
        }
        // Lower the busy flag
        Mutex_Flag = false;
    }
}
