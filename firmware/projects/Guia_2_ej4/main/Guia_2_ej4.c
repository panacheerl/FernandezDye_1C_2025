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
#include <analog_io_mcu.h>
#include <uart_mcu.h>
#include <timer_mcu.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
TaskHandle_t medir_tarea_handle = NULL;
TaskHandle_t uart_tarea_handle = NULL;
uint16_t valor=0;

/*==================[internal functions declaration]=========================*/

void notificacion()
{
	vTaskNotifyGiveFromISR(medir_tarea_handle, pdFALSE);
	vTaskNotifyGiveFromISR(uart_tarea_handle, pdFALSE);
}
void tarea_medir(void *param)
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH3, &valor);
	}
}
void enviar_dato(void *param)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		UartSendString(UART_PC, "$");
		UartSendString(UART_PC, (char *)UartItoa(valor, 10));
		UartSendString(UART_PC, ";");
		
		
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	serial_config_t mi_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};

	timer_config_t timer_2mseg = {
		.func_p = notificacion,
		.param_p = NULL,
		.timer = TIMER_A,
		.period = 2000};

	analog_input_config_t input_analogico = {
		.func_p = NULL,
		.input = CH3,
		.mode = ADC_SINGLE,
		.sample_frec = 0,
		.param_p = NULL};

	AnalogInputInit(&input_analogico);
	TimerInit(&timer_2mseg);
	UartInit(&mi_uart);
	xTaskCreate(&tarea_medir, "medir", 2048, NULL, 5, &medir_tarea_handle);
	xTaskCreate(&enviar_dato, "UART", 2048, &mi_uart, 5, &uart_tarea_handle);
	TimerStart(timer_2mseg.timer);


}
/*==================[end of file]============================================*/