/* #include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"



void led_task()
{   
	const uint LED_PIN = PICO_DEFAULT_LED_PIN;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	while (true) {
		gpio_put(LED_PIN, 1);
		vTaskDelay(100);
		gpio_put(LED_PIN, 0);
		vTaskDelay(100);
	}
}

int main()
{
	stdio_init_all();
	xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
	
	vTaskStartScheduler();

    while(1){};
} */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <iostream>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "servo.h"

int servopin = 12;
int millis = 500;

Servo servo1(servopin, 60);



const int task_delay = 500;
const int task_size = 128;

SemaphoreHandle_t mutex;

void vGuardedPrint(char *out){
	xSemaphoreTake(mutex, portMAX_DELAY);
	puts(out);
	xSemaphoreGive(mutex);
}

void vTaskSMP_print_core(void *pvParameters){
	char *task_name = pcTaskGetName(NULL);
	char out[12];

	for (;;) {
		sprintf(out, "%s %d", task_name, get_core_num());
		vGuardedPrint(out);
		vTaskDelay(task_delay);
	}

}

void led_task(void *pvParameters) {   
	const uint LED_PIN = PICO_DEFAULT_LED_PIN;
	uint uILedValueToSend = 0;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	char out[12];
	char *task_name = pcTaskGetName(NULL);
	while (true) {
		gpio_put(LED_PIN, 1);
		uILedValueToSend = 1;
		sprintf(out, "%s %d LED val:%d", task_name, get_core_num(), uILedValueToSend);
		vGuardedPrint(out);
		vTaskDelay(task_delay);

		gpio_put(LED_PIN, 0);
		uILedValueToSend = 0;
		sprintf(out, "%s %d LED val:%d", task_name, get_core_num(), uILedValueToSend);
		vGuardedPrint(out);
		vTaskDelay(task_delay);
	}
}

int main(){
	stdio_init_all();

	servo1.setFrequency(100);
	mutex = xSemaphoreCreateMutex(); // Create the mutex

	// Define the task handles
	TaskHandle_t handleA;
	TaskHandle_t handleB;

	// Create 4x tasks with different names & 2 with handles
	xTaskCreate(vTaskSMP_print_core, "A", task_size, NULL, 1, &handleA);
	xTaskCreate(vTaskSMP_print_core, "B", task_size, NULL, 1, &handleB);
	xTaskCreate(vTaskSMP_print_core, "C", task_size, NULL, 1, NULL);
	xTaskCreate(vTaskSMP_print_core, "D", task_size, NULL, 1, NULL);
	xTaskCreate(led_task, "L", task_size, NULL, 1, NULL);

	// Pin Tasks
	vTaskCoreAffinitySet(handleA, (1 << 0)); // Core 0
	vTaskCoreAffinitySet(handleB, (1 << 1)); // Core 1
	
	// Start the scheduler
	vTaskStartScheduler();
}



