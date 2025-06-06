/*! @mainpage Proyecto 3: Mano
 *
 * @section genDesc General Description
 *
 * Se desarrollará un dispositivo en forma de guante, que buscará transducir la
 * flexión individual de cada dedo en un valor porcentual, observable en un
 * Smartphone o Computadora. Este dispositivo permitiría medir el grado de mejora
 * o impacto en un tratamiento articular de los dedos de la mano.
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
 * | 23/05/2023 | Creacion del documento e inicio del proyecto   |
 *
 * @author Fernandez Dye Juan Francisco (kioru.juan@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <analog_io_mcu.h>
#include <uart_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"
#include "led.h"
#include "ble_mcu.h"
/*==================[macros and definitions]=================================*/

#define pulgar 0
#define indice 1
#define numero_dedos 2

#define CONFIG_BLINK_PERIOD 500
#define LED_BT LED_1

/*==================[internal data definition]===============================*/

bool enviar_datos = true;
uint16_t maxima_extension[numero_dedos];
uint16_t minima_extension[numero_dedos];
uint16_t mediciones[numero_dedos];
adc_ch_t canal = 1;

TaskHandle_t enviar_datos_tarea_handle = NULL;
TaskHandle_t timer_medicion_handle = NULL;
TaskHandle_t main_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

void notificacion_medir()
{
	vTaskNotifyGiveFromISR(timer_medicion_handle, pdFALSE);
}

void tarea_enviar_datos()
{

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (enviar_datos == true)
		{
			char msg[30];

			uint32_t porcentajes[numero_dedos];
			uint16_t salida[numero_dedos];

			for (int i = 0; i < numero_dedos; i++)
			{
				porcentajes[i] = (mediciones[i] * 100);
				salida[i] = porcentajes[i] / maxima_extension[i];
			}

			sprintf(msg, "*I%d* *M%d*\n", salida[0], salida[1]);
			BleSendString(msg);
		}
	}
}

void calibrarMax()
{
	LedToggle(LED_3);
	vTaskDelay(50000 / portTICK_PERIOD_MS);

	for (int i = 0; i < numero_dedos; i++)
	{
		adc_ch_t canal = i;
		AnalogInputReadSingle(canal, &maxima_extension[i]);
	}
	LedToggle(LED_3);
}

void calibrarMin()
{
	LedToggle(LED_3);
	vTaskDelay(50000 / portTICK_PERIOD_MS);
	for (int i = 0; i < numero_dedos; i++)
	{
		adc_ch_t canal = i;
		AnalogInputReadSingle(canal, &minima_extension[i]);
	}
	LedToggle(LED_3);
}

void medir_dedos()
{
	while (1)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		for (int i = 0; i < numero_dedos; i++)
		{
			adc_ch_t canal = i;
			AnalogInputReadSingle(canal, &mediciones[i]);
		}
		vTaskNotifyGiveFromISR(enviar_datos_tarea_handle, pdFALSE);
	}
}

void toggle_enviar_datos()
{

	if (enviar_datos == true)
		enviar_datos = false;
	else
		enviar_datos = true;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	timer_config_t timer_medicion = {
		.func_p = notificacion_medir,
		.param_p = NULL,
		.timer = TIMER_A,
		.period = 100000};

	analog_input_config_t input_dedo_1 = {
		.func_p = NULL,
		.input = CH0,
		.mode = ADC_SINGLE,
		.sample_frec = 0,
		.param_p = NULL};

	analog_input_config_t input_dedo_2 = {
		.func_p = NULL,
		.input = CH1,
		.mode = ADC_SINGLE,
		.sample_frec = 0,
		.param_p = NULL};

	ble_config_t ble_configuration = {
		"ESP_J_F",
		BLE_NO_INT};

	TimerInit(&timer_medicion);

	AnalogInputInit(&input_dedo_1);
	AnalogInputInit(&input_dedo_2);
	BleInit(&ble_configuration);
	LedsInit();
	SwitchesInit();

	xTaskCreate(&medir_dedos, "medir", 2048, NULL, 5, &timer_medicion_handle);
	xTaskCreate(&tarea_enviar_datos, "DATOS", 2048, NULL, 5, &enviar_datos_tarea_handle);
	SwitchActivInt((SWITCH_1 | SWITCH_2), toggle_enviar_datos, NULL);
	SwitchActivInt(SWITCH_1, calibrarMax, NULL);
	SwitchActivInt(SWITCH_1, calibrarMin, NULL);

	TimerStart(timer_medicion.timer);

	// while (1)
	// {
	// 	vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	// 	switch (BleStatus())
	// 	{
	// 	case BLE_OFF:
	// 		LedOff(LED_BT);
	// 		break;
	// 	case BLE_DISCONNECTED:
	// 		LedToggle(LED_BT);
	// 		break;
	// 	case BLE_CONNECTED:
	// 		LedOn(LED_BT);
	// 		break;
	// 	}
	// }
}
/*==================[end of file]============================================*/