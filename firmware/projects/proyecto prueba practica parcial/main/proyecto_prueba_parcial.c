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

// 1.- Agregar un timer
// 2.- agregar una interrupcion
// 3.- agregar una entrada analogica
// 4.- agregar una tarea
// 5.- agregar una salida analogica
// 6.- agregar una UART
// 7.- agregar bluetooth
// 8.- agregar un GPIO

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <stdint.h>

// 1. includes necesarios: timer, S.O, tareas //4.
#include "timer_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 2. include necesarios para una interrupcion. //8
#include "gpio_mcu.h"

// 3. includes necesarios para una entrada analogica. //5.
#include <analog_io_mcu.h>

// 6. includes necesarios para agregar una UART
#include <uart_mcu.h>

// 7. includes necesarios para agregar blutuch IMPORTANTE: CONFIGURAR EL CMAKE DE DRIVERS DESCOMENTANDO EL BLE
#include "ble_mcu.h"

/*==================[macros and definitions]=================================*/


// 8. Estruct de gpio para ESP-EDU
typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir;	/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;


/*==================[internal data definition]===============================*/

// 1. configuro el handler que voy a usar para la tarea
TaskHandle_t timer_task_handle = NULL;

// 2. configuro el handler que voy a usar para la interrupcion
TaskHandle_t interrupcion_task_handle = NULL;

// 4. configuro el handler que voy a usar para la tarea
TaskHandle_t tarea_task_handle = NULL;

// 6. configuro el handler que voy a usar en UART
TaskHandle_t uart_tarea_handle = NULL;

// 7. configuro el handler que voy a usar en BLE
TaskHandle_t BLE_tarea_handle = NULL;

/*==================[internal functions declaration]=========================*/

// 1. configuro la notificacion del timer.
void Notificacion_Timer(void *param)
{
	vTaskNotifyGiveFromISR(timer_task_handle, pdFALSE); /* Envía una notificación a la tarea asociada al timer */
}
// 2. configuro la notificacion de la interrupcion
void Notificacion_Interrupcion(void *param)
{
	vTaskNotifyGiveFromISR(interrupcion_task_handle, pdFALSE); /* Envía una notificación a la tarea asociada al timer */
}
// 4. configuro la funcion que realizara mi tarea
void funcion_tarea()
{
	while (1)
	{
		// accion tarea: debe esperar notificacion o un vtaskdelay
	}
}
// 6. configuro el enviar el dato por UART
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

// 7. Configuro el enviar el dato por bluetooth

void enviar_dato_BLE()
{

	while (1)
	{
		// importante agregar una notificacion para cuando enviar el dato
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		// envio el dato por string
		char msg[30];
		uint8_t valor=0;
		sprintf(msg, "El valor enviado es: %d\n", valor);
		BleSendString(msg);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	// 1. Configuro el timer, inicializo el timer, inicio el timer.
	uint8_t periodo_ms=1000;
	timer_config_t timerprueba = {
		.timer = TIMER_A,
		.period = periodo_ms,
		.func_p = Notificacion_Timer,
		.param_p = NULL};

	TimerInit(&timerprueba);
	TimerStart(timerprueba.timer);
	// 2. Configuro la interrupcion. Si uso los switches de la placa la funcion se llama switch active int
	GPIOActivInt(GPIO_12, Notificacion_Interrupcion, false, NULL);

	// 8. Configuro el GPIO e inicializo el GPIO
	gpioConf_t gpio_prueba = {
		.pin = GPIO_11,
		.dir = 0 /*!< GPIO direction '0' IN;  '1' OUT*/
	};
	GPIOInit(GPIO_20, GPIO_OUTPUT);

	// 3. Configuro la entrada e inicializo la entrada
	analog_input_config_t entrada_analogica = {
		.func_p = NULL,
		.input = CH0,
		.mode = ADC_SINGLE,
		.sample_frec = 0,
		.param_p = NULL};
	AnalogInputInit(&entrada_analogica);

	// 4. Configuro la tarea
	xTaskCreate(&funcion_tarea, "nombre_debug_tarea", 2048, NULL, 5, &tarea_task_handle);

	// 5. configuro la salida analogica (unica en el esp-EDU) y la inicializo
	AnalogOutputInit();
	uint8_t valor = 0;		  // de 0 a 255
	AnalogOutputWrite(valor); // escribo el valor en la salida

	// 6. configuro la uart, la inicializo y creo la tarea que la comande

	serial_config_t mi_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};

	UartInit(&mi_uart);
	xTaskCreate(&enviar_dato_UART, "UART", 1024, &mi_uart, 5, &uart_tarea_handle);

	// 7. configuro el bluetotth, lo inicializo y creo la tarea que lo comande
	ble_config_t ble_configuration = {
		"ESP_J_F",
		BLE_NO_INT // funcion que se ejecuta cuando se reciba un dato

	};
	BleInit(&ble_configuration);
	xTaskCreate(&enviar_dato_BLE, "BLE", 1024, NULL, 5, &BLE_tarea_handle);
}
/*==================[end of file]============================================*/