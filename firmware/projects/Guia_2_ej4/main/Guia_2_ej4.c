/*! @mainpage Proyecto 2, Ejercicio 4: Proyecto osciloscopio
 *
 * @section genDesc General Description
 *
 * Se diseño e implemento un dispositivo que permite leer señales analogicas mediante el ESP-EDU
 * y representarlas mediante un graficador por puerto serie en una computadora. Adicionalmente a esto
 * se implemento una salida analogica que muestre un ECG previamente muestreado almacenado en memoria.
 *
 * @section hardConn Hardware Connection
 *
 * |    Funcion				|   ESP-EDU   	|
 * |:----------------------:|:--------------|
 * | 	Entrada analogica 	| 		CH3		|
 * | 	Salida analogica 	| 		CH0		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 23/05/2025 | Creacion de documentacion                      |
 *
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
#define BUFFER_SIZE 231
/*==================[internal data definition]===============================*/
TaskHandle_t medir_tarea_handle = NULL;
TaskHandle_t uart_tarea_handle = NULL;
TaskHandle_t timer_ecg_handle = NULL;
TaskHandle_t main_task_handle = NULL;

/** @def valor
 *  @brief Valor entero que representa la tension medida por la entrada analogica
 */
uint16_t valor = 0;

/** @def ECG
 *  @brief Cadena de valores que corresponden a la forma de onda del ECG previamente muestreado
 */
unsigned char ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17} ;

/** @def contador
 *  @brief Valor entero que representa la posicion en la cadena de valores de ECG
 */
int contador = 0;

/*==================[internal functions declaration]=========================*/


/** @fn void notificacion_graficador
 * @brief Envia una notificacion a la tarea de medir y la tarea de enviar el valor por puerto serie, lo que las pasa de suspendidas a listas para ser reanudadas.
 * @return
 */
void notificacion_graficador()
{
	vTaskNotifyGiveFromISR(medir_tarea_handle, pdFALSE);
	vTaskNotifyGiveFromISR(uart_tarea_handle, pdFALSE);
}

/** @fn void tarea_medir
 * @brief Lee el valor de tension en la entrada analogica y lo almacena en memoria.
 * @return
 */
void tarea_medir(void *param)
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH3, &valor);
	}
}

/** @fn void enviar_dato
 * @brief Envia el valor almacenado de tension en memoria por el puerto serie mediante el protocolo UART.
 * @return
 */
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

/** @fn void notificacion_ecg
 * @brief Envia una notificacion a la tarea responsable de mostrar los valores de ECG por la salida analogica
 * @return
 */
void notificacion_ecg()
{
	vTaskNotifyGiveFromISR(timer_ecg_handle, pdFALSE);
}

/** @fn void tarea_timer_ecg
 * @brief Cambia los valores de la salida analogica, segun la cadena de valores almacenada en memoria.
 * @return
 */
void tarea_timer_ecg()
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (contador < 256)
		{
			AnalogOutputWrite(ECG[contador]);
			contador++;
		}

		else
		{
			if (contador >= 256)
			{
				contador = 0;
			}
		};
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
		.func_p = notificacion_graficador,
		.param_p = NULL,
		.timer = TIMER_A,
		.period = 2000};

	timer_config_t timer_ecg = {
		.func_p = notificacion_ecg,
		.param_p = NULL,
		.timer = TIMER_B,
		.period = 4320};

	analog_input_config_t input_analogico = {
		.func_p = NULL,
		.input = CH3,
		.mode = ADC_SINGLE,
		.sample_frec = 0,
		.param_p = NULL};

	AnalogInputInit(&input_analogico);
	AnalogOutputInit();
	TimerInit(&timer_2mseg);
	TimerInit(&timer_ecg);
	UartInit(&mi_uart);
	xTaskCreate(&tarea_medir, "medir", 1024, NULL, 5, &medir_tarea_handle);
	xTaskCreate(&enviar_dato, "UART", 1024, &mi_uart, 5, &uart_tarea_handle);
	xTaskCreate(&tarea_timer_ecg, "ecg", 1024, NULL, 5, &timer_ecg_handle);
	TimerStart(timer_2mseg.timer);
	TimerStart(timer_ecg.timer);
}
/*==================[end of file]============================================*/