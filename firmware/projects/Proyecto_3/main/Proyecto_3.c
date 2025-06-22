/*! @mainpage Proyecto 3: Mano
 *
 * @section genDesc General Description
 *
 * Se desarrollará un dispositivo en forma de guante, que buscará transducir la
 * flexión individual de cada dedo en un valor numerico, observable en un
 * Smartphone o Computadora. Este dispositivo permitiría medir el grado de mejora
 * o impacto en un tratamiento articular de los dedos de la mano.
 *
 * @section hardConn Hardware Connection
 *
 * |    Funcion				|   ESP-EDU   	|
 * |:----------------------:|:--------------|
 * | 	Entrada analogica 	| 		CH0		|
 * | 	Entrada analogica 	| 		CH1		|
 * | 	Entrada analogica 	| 		CH2		|
 *
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
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"
#include "ble_mcu.h"
/*==================[macros and definitions]=================================*/

#define numero_dedos 3
#define periodo_medicion 100000

/*==================[internal data definition]===============================*/

/** @def enviar_datos
 *  @brief Valor logico que activa o desactiva la transmision de datos
 */
bool enviar_datos = true;

/** @def medir
 *  @brief Valor logico que activa o desactiva la medicion
 */
bool medir = true;

/** @def mediciones
 *  @brief Arreglo de valores numericos que almacenan el valor leido en cada entrada analogica
 */
uint16_t mediciones[numero_dedos] = {0, 0, 0};

TaskHandle_t enviar_datos_tarea_handle = NULL;
TaskHandle_t timer_medicion_handle = NULL;
TaskHandle_t main_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

/** @fn void notificacion_medir
 * @brief Envia una notificacion a la tarea de medir lo que la pasa de suspendida a lista para ser reanudadas.
 * @return
 */

void notificacion_medir()
{
	vTaskNotifyGiveFromISR(timer_medicion_handle, pdFALSE);
}

/** @fn void tarea_enviar_datos
 * @brief Envia los datos almacenados por BLE.
 * @return
 */

void tarea_enviar_datos()
{

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (enviar_datos == true)
		{
			char msg[30];

			sprintf(msg, "*I%d* *M%d* *A%d*\n", mediciones[0], mediciones[1], mediciones[2]);
			BleSendString(msg);
		}
	}
}

/** @fn void medir_dedos
 * @brief Lee el valor actual de las entradas analogicas y las almacena en memoria.
 * @return
 */

void medir_dedos()
{
	while (1)
	{

		if (medir == true)
		{
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
			for (int i = 0; i < numero_dedos; i++)
			{
				adc_ch_t canal = i;
				AnalogInputReadSingle(canal, &mediciones[i]);
			}
		}
		vTaskNotifyGiveFromISR(enviar_datos_tarea_handle, pdFALSE);
	}
}

/** @fn void toggle_enviar_datos
 * @brief Detiene o reanuda la transmision de datos por BLE.
 * @return
 */
void toggle_enviar_datos()
{

	if (enviar_datos == true)
		enviar_datos = false;
	else
		enviar_datos = true;
}

/** @fn void toggle_medir
 * @brief Detiene o reanuda la medicion de los valores actuales.
 * @return
 */
void toggle_medir()
{

	if (medir == true)
		medir = false;
	else
		medir = true;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	timer_config_t timer_medicion = {
		.func_p = notificacion_medir,
		.param_p = NULL,
		.timer = TIMER_A,
		.period = periodo_medicion};

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

	analog_input_config_t input_dedo_3 = {
		.func_p = NULL,
		.input = CH2,
		.mode = ADC_SINGLE,
		.sample_frec = 0,
		.param_p = NULL};

	ble_config_t ble_configuration = {
		"ESP_J_F",
		BLE_NO_INT};

	TimerInit(&timer_medicion);

	AnalogInputInit(&input_dedo_1);
	AnalogInputInit(&input_dedo_2);
	AnalogInputInit(&input_dedo_3);
	BleInit(&ble_configuration);
	SwitchesInit();

	xTaskCreate(&medir_dedos, "medir", 4096, NULL, 5, &timer_medicion_handle);
	xTaskCreate(&tarea_enviar_datos, "DATOS", 2048, NULL, 5, &enviar_datos_tarea_handle);

	SwitchActivInt(SWITCH_1, toggle_enviar_datos, NULL);
	SwitchActivInt(SWITCH_2, toggle_medir, NULL);

	TimerStart(timer_medicion.timer);
}
/*==================[end of file]============================================*/