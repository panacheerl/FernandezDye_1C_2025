/*! @mainpage Examen
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
 * | 	Entrada analogica 	| 		CH3		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 09/06/2023 | Creacion del documento e inicio del proyecto   |
 *
 * @author Fernandez Dye Juan Francisco (kioru.juan@gmail.com)
 *
 */
/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "dht11.h"


#include <uart_mcu.h>

#include "timer_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <analog_io_mcu.h>

#include "led.h"

#include "switch.h"


/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

TaskHandle_t uart_tarea_handle = NULL;
TaskHandle_t timer1s_tarea_handle = NULL;
TaskHandle_t timer5s_tarea_handle = NULL;

/*==================[internal functions declaration]=========================*/


void enviar_dato_UART(void *param)
{
	while (true)
	{
		// importante agregar una notificacion para cuando enviar el dato
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Envio el dato como tal segun la estructura que desee o necesite
		uint8_t valor = 0;
		UartSendString(UART_PC, "$");
		UartSendString(UART_PC, (char *)UartItoa(valor, 10)); // convierte el valor de int a ascii
		UartSendString(UART_PC, ";");
	}
}


void Notificacion_Timer_1seg(void *param)
{
	vTaskNotifyGiveFromISR(timer_task_handle, pdFALSE); 
}
void Notificacion_Timer_5seg(void *param)
{
	vTaskNotifyGiveFromISR(timer_task_handle, pdFALSE); 
}

/*==================[external functions definition]==========================*/
void app_main(void){


	serial_config_t mi_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};

	UartInit(&mi_uart);

	xTaskCreate(&enviar_dato_UART, "UART", 1024, &mi_uart, 5, &uart_tarea_handle);

	

	timer_config_t timer1seg = {
		.timer = TIMER_A,
		.period = 10000,
		.func_p = Notificacion_Timer_1seg,
		.param_p = NULL};

			timer_config_t timer5seg = {
		.timer = TIMER_B,
		.period = 50000,
		.func_p = Notificacion_Timer_5seg,
		.param_p = NULL};

	TimerInit(&timer1seg);
	TimerStart(timer1seg.timer);
		TimerInit(&timer5seg);
	TimerStart(timer5seg.timer);



}
/*==================[end of file]============================================*/