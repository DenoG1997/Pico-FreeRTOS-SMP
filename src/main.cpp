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
#include "event_groups.h"
#include "timers.h"

#include <iostream>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "pico/util/queue.h"
#include "hardware/i2c.h"

#include "servo.h"
#include "distance_sensor.h"

#define SERVO_PIN		2
#define MOTOR_PIN		3
#define SERVO_FREQ		100

const int task_delay = 500;
const int task_size = 128;

const EventBits_t EV_THREAD_1 = (1 << 0);
const EventBits_t EV_THREAD_2 = (1 << 1);
const EventBits_t EV_THREAD_3 = (1 << 2);
const EventBits_t EV_THREAD_4 = (1 << 4);

Servo servo1(SERVO_PIN, 1500, SERVO_FREQ);
Servo motor1(MOTOR_PIN, 1500, SERVO_FREQ);

DistanceSensor sensor{pio0, 0, 5};

SemaphoreHandle_t mutex;
EventGroupHandle_t eventGroup;

void vGuardedPrint(char *out){
	xSemaphoreTake(mutex, portMAX_DELAY);
	puts(out);
	xSemaphoreGive(mutex);
}

void vTaskSMP_print_core(void *pvParameters) {
	char *task_name = pcTaskGetName(NULL);
	char out[12];

	for (;;) {
		sprintf(out, "%s %d", task_name, get_core_num());
		vGuardedPrint(out);
		vTaskDelay(task_delay);
	}
}

void led_task(void *pvParameter) {

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
		vTaskDelay(600);

		gpio_put(LED_PIN, 0);
		uILedValueToSend = 0;
		sprintf(out, "%s %d LED val:%d", task_name, get_core_num(), uILedValueToSend);
		vGuardedPrint(out);
		vTaskDelay(600);
	}
}

void hc_distance_sensor(void *pvParameter) {

	while (true) {
		char out[12];
		sensor.TriggerRead();
		while (sensor.is_sensing) {
			vTaskDelay(10);
		}
		// printf("Got reading: %u\n", sensor.distance);
		sprintf(out, "Got reading: %u", sensor.distance);
		vGuardedPrint(out);
		vTaskDelay(800);
	}
	// const uint TRIGGER_PIN = 3;
	// const uint ECHO_PIN = 4;
	// const uint LED_PIN = PICO_DEFAULT_LED_PIN;
	// uint uILedValueToSend = 0;
	// gpio_init(LED_PIN);
	// gpio_set_dir(LED_PIN, GPIO_OUT);
	// char out[12];
	// char *task_name = pcTaskGetName(NULL);
	// DistanceSensor sensor(pio0, 0, TRIGGER_PIN, ECHO_PIN);
	// while (true) {
	// 	gpio_put(LED_PIN, 1);
	// 	uILedValueToSend = 1;
	// 	sprintf(out, "%s %d LED val:%d", task_name, get_core_num(), uILedValueToSend);
	// 	vGuardedPrint(out);
	// 	vTaskDelay(600);

	// 	gpio_put(LED_PIN, 0);
	// 	uILedValueToSend = 0;
	// 	sprintf(out, "%s %d LED val:%d", task_name, get_core_num(), uILedValueToSend);
	// 	vGuardedPrint(out);
	// 	vTaskDelay(600);
	// }
}

void motor_pwm(void *pvParameter) {
	while(1) {
		motor1.setDuty(1500);
		servo1.setDuty(1500);
		vTaskDelay(2000);
		motor1.setDuty(1600);
		servo1.setDuty(1300);
		vTaskDelay(2000);
		motor1.setDuty(1000);
		servo1.setDuty(1500);
		vTaskDelay(2000);
		motor1.setDuty(1500);
		servo1.setDuty(1700);
		vTaskDelay(2000);
		motor1.setDuty(1400);
		servo1.setDuty(1500);
		vTaskDelay(2000);
	}
}

int main(){
	stdio_init_all();

	// servo1.setDuty(1800);
	mutex = xSemaphoreCreateMutex(); // Create the mutex
	// char out[12];
	// sensor.TriggerRead();
	// while (sensor.is_sensing) {                                                                                                                                                                 
	// 	sleep_us(10);
	// }
	// // printf("Got reading: %u\n", sensor.distance);
	// printf("Got reading: %u\n", sensor.distance);
	
	// Define the task handles
	TaskHandle_t handleA;
	TaskHandle_t handleB;
	//  Create 4x tasks with different names & 2 with handles
	xTaskCreate(vTaskSMP_print_core, "A", task_size, NULL, 1, &handleA);
	xTaskCreate(vTaskSMP_print_core, "B", task_size, NULL, 1, &handleB);
	xTaskCreate(vTaskSMP_print_core, "C", task_size, NULL, 1, NULL);
	xTaskCreate(vTaskSMP_print_core, "D", task_size, NULL, 1, NULL);
	xTaskCreate(led_task, "E", task_size, NULL, 1, NULL);
	xTaskCreate(hc_distance_sensor, "HC", task_size, NULL, 1, NULL);
	xTaskCreate(motor_pwm, "M", task_size, NULL, 1, NULL);

	// Pin Tasks
	vTaskCoreAffinitySet(handleA, (1 << 0)); // Core 0
	vTaskCoreAffinitySet(handleB, (1 << 1)); // Core 1
	
	// Start the scheduler
	vTaskStartScheduler();
}



/* / Task-Funktionen
void task1_handler(void *pvParameters) {
	while (true) {
		xEventGroupWaitBits(eventGroup, EV_THREAD_1, pdTRUE, pdFALSE, portMAX_DELAY);
		hc_distance_sensor(pvParameters);
	}
}

void task2_handler(void *pvParameters) {
	while (true) {
		xEventGroupWaitBits(eventGroup, EV_THREAD_2, pdTRUE, pdFALSE, portMAX_DELAY);
		led_task(pvParameters);
	}
}

void task3_handler(void *pvParameters) {
	while (true) {
		xEventGroupWaitBits(eventGroup, EV_THREAD_3, pdTRUE, pdFALSE, portMAX_DELAY);
		// Task_3();
	}
}

void task4_handler(void *pvParameters) {
	while (true) {
		xEventGroupWaitBits(eventGroup, EV_THREAD_4, pdTRUE, pdFALSE, portMAX_DELAY);
		// Task_4();
	}
}

bool setFlag_1(repeating_timer_t *rt) {
	return xEventGroupSetBits(eventGroup, EV_THREAD_1);
}

bool setFlag_2(repeating_timer_t *rt) {
	return xEventGroupSetBits(eventGroup, EV_THREAD_2);
}

void setFlag_3(void) {
	xEventGroupSetBits(eventGroup, EV_THREAD_3);
}

void setFlag_4(void) {
	xEventGroupSetBits(eventGroup, EV_THREAD_4);
} */

// Ähnliche Funktionen für setFlag_2, setFlag_3, setFlag_4 ...

/* int main() {
	stdio_init_all();
	eventGroup = xEventGroupCreate();

	// Task-Erstellung
	xTaskCreate(task1_handler, "Task 1", 1000, NULL, 1, NULL);
	xTaskCreate(task2_handler, "Task 2", 1000, NULL, 1, NULL);
	xTaskCreate(task3_handler, "Task 3", 1000, NULL, 1, NULL);
	xTaskCreate(task4_handler, "Task 4", 1000, NULL, 1, NULL);

	// Timer oder Interrupts einrichten, um setFlag_x Funktionen aufzurufen...
	// schreibe mir einer Timer funktion die alle 100ms setFlag_1 aufruft
	repeating_timer_t timer;
	add_repeating_timer_us(-100000, setFlag_1, NULL, &timer);
	add_repeating_timer_us(-980000, setFlag_2, NULL, &timer);

	vTaskStartScheduler();

	return 0;
} */

/* // Task-Funktionen
void task1_handler(void *pvParameters) {
	while (true) {
		xEventGroupWaitBits(eventGroup, EV_THREAD_1, pdTRUE, pdFALSE, portMAX_DELAY);
		led_task();
	}
}

void task2_handler(void *pvParameters) {
	while (true) {
		xEventGroupWaitBits(eventGroup, EV_THREAD_2, pdTRUE, pdFALSE, portMAX_DELAY);
		// hc_distance_sensor(pvParameters);
	}
}

// Timer Callback-Funktionen
void setFlag_1(TimerHandle_t xTimer) {
	xEventGroupSetBits(eventGroup, EV_THREAD_1);
}

void setFlag_2(TimerHandle_t xTimer) {
	xEventGroupSetBits(eventGroup, EV_THREAD_2);
}

int main() {
	stdio_init_all();
	eventGroup = xEventGroupCreate();

	// Task-Erstellung
	xTaskCreate(task1_handler, "Task 1", 1000, NULL, 1, NULL);
	xTaskCreate(task2_handler, "Task 2", 1000, NULL, 1, NULL);

	// Timer-Erstellung
	TimerHandle_t timer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(100), pdTRUE, NULL, setFlag_1);
	TimerHandle_t timer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(980), pdTRUE, NULL, setFlag_2);

	if (timer1 != NULL && timer2 != NULL) {
		xTimerStart(timer1, 0);
		xTimerStart(timer2, 0);
	}

	vTaskStartScheduler();

	return 0;
} */


