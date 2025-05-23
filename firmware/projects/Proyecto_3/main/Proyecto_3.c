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
/*==================[macros and definitions]=================================*/

#define pulgar 0
#define indice 1
#define numero_dedos 2
#define CH0 'p'
#define CH1 'i'

/*==================[internal data definition]===============================*/

bool enviar_datos = true;
uint16_t maxima_extension = 3300;
uint16_t mediciones[numero_dedos];
char mano[numero_dedos] = {'p', 'i'};

TaskHandle_t uart_tarea_handle = NULL;
TaskHandle_t timer_medicion_handle = NULL;
TaskHandle_t main_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

void notificacion_medir()
{
	vTaskNotifyGiveFromISR(timer_medicion_handle, pdFALSE);
}
void notificacion_datos()
{
	vTaskNotifyGiveFromISR(uart_tarea_handle, pdFALSE);
}

void medir_dedo()
{
	while (1)
	{
		for (uint8_t dedo = 0; dedo<numero_dedos ; dedo++) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(mano[dedo], &mediciones[dedo]);
		dedo++;
		}
		

	}
}
//void enviar_datos(){}

void toggle_enviar_datos()
{
}
/*==================[external functions definition]==========================*/
void app_main(void)
{

	serial_config_t mi_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};

	timer_config_t timer_medicion = {
		.func_p = notificacion_medir,
		.param_p = NULL,
		.timer = TIMER_A,
		.period = 2000};

	timer_config_t timer_datos = {
		.func_p = notificacion_datos,
		.param_p = NULL,
		.timer = TIMER_B,
		.period = 2000};

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

	// xTaskCreate(&enviar_dato, "UART", 1024, &mi_uart, 5, &uart_tarea_handle);

	TimerInit(&timer_medicion);
	TimerStart(timer_medicion.timer);
}
/*==================[end of file]============================================*/