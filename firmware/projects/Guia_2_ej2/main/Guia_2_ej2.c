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

int distancia = 0;
	bool medir = 1;
	bool hold = 0;
void Medir_distancia(void *param)
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

void toggle_medir(void *pvParameter)
{
	if (medir)
		medir = false;
	else
		medir = true;
}
void toggle_hold(void *pvParameter)
{
	if (hold)
		hold = false;
	else
		hold = true;
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

	SwitchActivInt(SWITCH_1, toggle_medir, NULL);
	SwitchActivInt(SWITCH_2, toggle_hold, NULL);

	xTaskCreate(&tarea_imprimir_distancia, "imprimir_distancia", 2048, NULL, 5, &imprimir_tarea_handle);
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