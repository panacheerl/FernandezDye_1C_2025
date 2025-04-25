/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
#include "led.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
TaskHandle_t medir_tarea_handle = NULL;
TaskHandle_t imprimir_tarea_handle = NULL;
TaskHandle_t teclas_tarea_handle = NULL;
/*==================[internal functions declaration]=========================*/
void Prender_leds(int distancia);
bool medir = true;
bool hold = false;
int distancia = 0;

void Medir_distancia(void* param)
{
	
		if (medir)
		{
			distancia = HcSr04ReadDistanceInCentimeters();
		}
	
}
static void tarea_imprimir_distancia(void *pvParameter)
{
	while (1)
	{
		if (!hold)
		{
			LcdItsE0803Write(distancia);
			Prender_leds(distancia);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}
static void tarea_leer_teclas(void *pvParameter)
{
	while (1)
	{
		int8_t lectura = SwitchesRead();
		if (lectura == SWITCH_1)
		{
			if (medir)
				medir = false;
			else
				medir = true;
		}
		if (lectura == SWITCH_2)
		{
			if (hold)
				hold = false;
			else
				hold = true;
		}
		if (lectura == (SWITCH_1 | SWITCH_2))
		{
			if (hold)
				hold = false;
			else
				hold = true;
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	SwitchesInit();
	LedsInit();
	timer_config_t timer_1seg = {
		.func_p = Medir_distancia,
		.param_p = NULL,
		.timer = TIMER_A,
		.period = 1000000};
	


	TimerInit(&timer_1seg);


	xTaskCreate(&tarea_imprimir_distancia, "imprimir_distancia", 2048, NULL, 5, &imprimir_tarea_handle);
	xTaskCreate(&tarea_leer_teclas, "leer_teclas", 2048, NULL, 5, &teclas_tarea_handle);
		TimerStart(timer_1seg.timer);
}

void Prender_leds(int distancia)
{
	if (distancia < 10)
	{
		LedsOffAll();
	}
	else if (distancia > 10)
	{
		LedOn(LED_1);

		if (distancia > 20)
		{
			LedOn(LED_2);
			if (distancia < 30)
				LedOff(LED_3);
		}
		else
			LedOff(LED_2);
		if (distancia > 30)
		{
			LedOn(LED_3);
		}
	}
}

/*==================[end of file]============================================*/