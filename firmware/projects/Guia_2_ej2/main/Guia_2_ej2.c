/*! @mainpage Proyecto 2, Ejercicio 2: Medidor de distancia por ultrasonido c/interrupciones
 *
 * @section genDesc General Description
 *
 * Se diseño un dispositivo que permite leer una distancia mediante el sensor HC-SR04
 * y una pantalla LCD que muestra el valor de lectura en cm. En adicion a esto, se utilizan
 * los leds de la plaqueta EDU-ESP de forma que indiquen la distancia que esta siendo medida
 * encendiendo un led por cada 10cm de distancia, con un maximo de 30cm. Por otro lado,
 * se implementaron las teclas de la EDU-ESP, de forma que la tecla 1 active y detenga la medicion
 * y la tecla 2 mantenga el dato en pantalla sin dejar de leer. A diferencia el Ejercicio 1, se
 * se implementan interrupciones
 *
 * @section hardConn Hardware Connection
 *
 * |    Display 	 |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	D1	 		| 	GPIO_20		|
 * | 	D2	 		| 	GPIO_21		|
 * | 	D3	 		| 	GPIO_22		|
 * | 	D4	 		| 	GPIO_23		|
 * | 	SEL_1 		|	GPIO_19		|
 * | 	SEL_2 		|	GPIO_18		|
 * | 	SEL_3 		|	GPIO_9		|
 * | 	+5V	 		|	+5V			|
 * | 	GND	 		|	GND			|
 *
 *
 * |    HC-SR04  	|   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5v	 		| 	+5V			|
 * | 	GND		 	| 	GND			|
 *
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 23/05/2025 | Creacion de documentacion                      |
 *
 * @author Fernandez Dye Juan Francisco (kioru.juan@gmail.com)
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

/** @def distancia
 *  @brief Valor entero que representa la distancia en cm medida por el sensor
 */
int distancia = 0;

/** @def medir
 *  @brief Valor booleano que representa si el modo medir esta activado o desactivado
 */
bool medir = 1;

/** @def hold
 *  @brief Valor booleano que representa si el modo mantener esta activado o desactivado
 */
bool hold = 0;

/*==================[internal functions declaration]=========================*/
/** @fn void Prender_leds
 * @brief Representa el valor de distancia almacenado en un conjunto de leds, de forma que enciende un led cada 10cm.
 * @return
 */
void Prender_leds();

/** @fn void Medir_distancia
 * @brief Ordena al sensor leer la distancia en centimetros y la almacena en memoria.
 * @return
 */

void Medir_distancia(void *param)
{

	if (medir)
	{
		distancia = HcSr04ReadDistanceInCentimeters();
	}
}

/** @fn void tarea_imprimir_distancia
 * @brief Muestra el valor almacenado en memoria en el display LCD, y llama a la funcion prender_leds.
 * @return
 */
static void tarea_imprimir_distancia(void *pvParameter)
{
	while (1)
	{
		Prender_leds();
		if (!hold)
		{
			LcdItsE0803Write(distancia);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}

/** @fn void toggle_medir
 * @brief Invierte el valor de la variable medir
 * @return
 */

void toggle_medir(void *pvParameter)
{
	if (medir)
		medir = false;
	else
		medir = true;
}

/** @fn void toggle_medir
 * @brief Invierte el valor de la variable hold
 * @return
 */
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

void Prender_leds()
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