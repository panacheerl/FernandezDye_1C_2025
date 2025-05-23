/*! @mainpage Proyecto 2, Ejercicio 1: Medidor de distancia por ultrasonido
 *
 *
 * @section genDesc General Description
 *
 * Se diseño un dispositivo que permite leer una distancia mediante el sensor HC-SR04
 * y una pantalla LCD que muestra el valor de lectura en cm. En adicion a esto, se utilizan
 * los leds de la plaqueta EDU-ESP de forma que indiquen la distancia que esta siendo medida
 * encendiendo un led por cada 10cm de distancia, con un maximo de 30cm. Por otro lado,
 * se implementaron las teclas de la EDU-ESP, de forma que la tecla 1 active y detenga la medicion
 * y la tecla 2 mantenga el dato en pantalla sin dejar de leer.
 *
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
 * @section changelog Changelog
 *
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 23/05/2025 | Creacion de documentacion                      |
 *
 * @author Fernandez Dye Juan Francisco (kioru.juan@gmail.com)
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

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

TaskHandle_t medir_tarea_handle = NULL;
TaskHandle_t imprimir_tarea_handle = NULL;
TaskHandle_t teclas_tarea_handle = NULL;

/** @def medir
 *  @brief Valor booleano que representa si el modo medir esta activado o desactivado
 */
bool medir = true;

/** @def hold
 *  @brief Valor booleano que representa si el modo mantener esta activado o desactivado
 */
bool hold = false;

/** @def distancia
 *  @brief Valor entero que representa la distancia en cm medida por el sensor
 */
int distancia = 0;

/*==================[internal functions declaration]=========================*/

/** @fn void prender_leds
 * @brief Representa el valor de distancia almacenado en un conjunto de leds, de forma que enciende un led cada 10cm.
 * @return
 */

void prender_leds();

/** @fn void tarea_medir_distancia
 * @brief Ordena al sensor leer la distancia en centimetros y la almacena en memoria.
 * @return
 */

static void tarea_medir_distancia(void *pvParameter)
{
	while (1)
	{
		if (medir)
		{
			distancia = HcSr04ReadDistanceInCentimeters();
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
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
		prender_leds();
		if (!hold)
		{
			LcdItsE0803Write(distancia);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}

/** @fn void tarea_leer_teclas
 * @brief Lee si alguna tecla se encuentra apretada. En caso verdadero, cambia el valor de la variable que corresponda segun la que se encuentre apretada.
 * @return
 */
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
			
			if (medir)
				medir = false;
			else
				medir = true;
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

	xTaskCreate(&tarea_medir_distancia, "medir_distancia", 512, NULL, 5, &medir_tarea_handle);
	xTaskCreate(&tarea_imprimir_distancia, "imprimir_distancia", 512, NULL, 5, &imprimir_tarea_handle);
	xTaskCreate(&tarea_leer_teclas, "leer_teclas", 512, NULL, 5, &teclas_tarea_handle);
}

void prender_leds()
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