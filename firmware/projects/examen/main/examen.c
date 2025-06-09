/*! @mainpage Examen
 *
 * @section genDesc General Description
 *
 * Se desarrollará una estacion de monitoreo ambiental que permita medir la temperatura,
 * humedad y radiación. Adicionalmente a esto se implementara una alerta a una computadora
 * por puerto serie, en conjunto con un codigo con luces LED, que advierta cuando los niveles
 * sean peligrosos.
 *
 * @section hardConn Hardware Connection
 *
 * |    Funcion				|   ESP-EDU   	|
 * |:----------------------:|:--------------|
 * | 	Entrada analogica 	| 		CH0		|
 *
 *
 * |   	DHT11		|   ESP-EDU 	|
 * |:--------------:|:--------------|
 * | 	VCC     	|	3V3     	|
 * | 	DATA	 	| 	GPIO_19		|
 * | 	GND		 	| 	GND 		|
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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

TaskHandle_t humedad_temp_tarea_handle = NULL;
TaskHandle_t radiacion_tarea_handle = NULL;

/** @def Temp_Humedad
 *  @brief Cadena de caracteres que da formato a la temperatura y la humedad medida por el sensor
 */
char Temp_Humedad[40] = "Temperatura: %d ºC - Húmedad: %d % \n";
/** @def Riesgo_Nevada
 *  @brief Cadena de caracteres que anuncia cuando hay riesgo de nevada
 */
char Riesgo_Nevada[20] = "RIESGO DE NEVADA\n";
/** @def Temp_Humedad
 *  @brief Cadena de caracteres que da formato a la radiacion por el sensor
 */
char Radiacion[20] = "Radiación %d mR/h \n";
/** @def Riesgo_Rad
 *  @brief Cadena de caracteres que anuncia cuando hay riesgo radioactivo
 */
char Riesgo_Rad[20] = "RADIACIÓN ELEVADA\n";

/** @def temp
 *  @brief Valor flotante que almacena la temperatura medida por el sensor DHT11
 */
float temp = 0.0;
/** @def hum
 *  @brief Valor flotante que almacena la humedad medida por el sensor DHT11
 */
float hum = 0.0;
/** @def rad
 *  @brief Valor entero que almacena la radiacion medida por el sensor analogico
 */
uint16_t rad = 0;

/** @def dispositivo_funcionando
 *  @brief Valor logico que indica si el dispositivo esta funcionando o no
 */
bool dispositivo_funcionando = true;

/** @def entrada_analogica
 *  @brief Estructura de configuracion de la entrada analogica
 */
analog_input_config_t entrada_analogica = {
	.func_p = NULL,
	.input = CH0,
	.mode = ADC_SINGLE,
	.sample_frec = 0,
	.param_p = NULL};

/*==================[internal functions declaration]=========================*/

/** @fn void Medir_Temp_Hum
 * @brief Funcion que lee los parametros desde el sensor DHT11 y los almacena en memoria.
 * @return
 */
void Medir_Temp_Hum()
{
	dht11Read(&hum, &temp);
}

/** @fn void Medir_Radiacion
 * @brief Funcion que lee los parametros desde el sensor analogico y los almacena en memoria.
 * @return
 */
void Medir_Radiacion()
{
	// lee 3300 si el valor es 100mR/h
	uint16_t aux = 0;
	AnalogInputReadSingle(entrada_analogica.input, &aux);
	rad = aux * 100 / 3300;
}

/** @fn void encender
 * @brief Funcion que cambia el estado del dispositivo a encendido.
 * @return
 */
void encender()
{
	dispositivo_funcionando = true;
}

/** @fn void apagar
 * @brief Funcion que cambia el estado del dispositivo a apagado.
 * @return
 */
void apagar()
{
	dispositivo_funcionando = false;
}

/** @fn void enviar_hyt_UART
 * @brief Funcion que envia los parametros de humedad y temperatura por puerto serie mediante UART.
 * @return
 */
void enviar_hyt_UART()
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		Medir_Temp_Hum();
		char msg[30];
		sprintf(msg, Temp_Humedad, temp, hum);
		UartSendString(UART_PC, msg);
		if (hum > 85)
		{
			if (temp < 2)
			{
				LedOn(LED_3);
				LedOff(LED_1);
				UartSendString(UART_PC, " - ");
				UartSendString(UART_PC, Riesgo_Nevada);
			}
		}
		else
		{
			LedOff(LED_3);
			LedOn(LED_1);
		}
	}
}

/** @fn void enviar_rad_UART
 * @brief Funcion que envia los parametros de radiacion por puerto serie mediante UART.
 * @return
 */
void enviar_rad_UART()
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		Medir_Radiacion();
		char msg[30];
		sprintf(msg, Radiacion, rad);
		UartSendString(UART_PC, msg);

		if (rad > 40)
		{
			LedOn(LED_2);
			LedOff(LED_1);
			UartSendString(UART_PC, " - ");
			UartSendString(UART_PC, Riesgo_Rad);
		}
		else
		{
			LedOff(LED_3);
			LedOn(LED_1);
		}
	}
}

/** @fn void Notificacion_Timer_1seg
 * @brief Funcion que envia cambia el estado de la tarea asignada a la humedad y temperatura.
 * @return
 */
void Notificacion_Timer_1seg(void *param)
{
	if (dispositivo_funcionando)
		vTaskNotifyGiveFromISR(enviar_hyt_UART, pdFALSE);
}

/** @fn void Notificacion_Timer_5seg
 * @brief Funcion que envia cambia el estado de la tarea asignada a la radiacion.
 * @return
 */
void Notificacion_Timer_5seg(void *param)
{
	if (dispositivo_funcionando)
		vTaskNotifyGiveFromISR(enviar_rad_UART, pdFALSE);
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	serial_config_t mi_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};

	timer_config_t timer1seg = {
		.timer = TIMER_A,
		.period = 1000000,
		.func_p = Notificacion_Timer_1seg,
		.param_p = NULL};

	timer_config_t timer5seg = {
		.timer = TIMER_B,
		.period = 5000000,
		.func_p = Notificacion_Timer_5seg,
		.param_p = NULL};

	TimerInit(&timer1seg);
	TimerStart(timer1seg.timer);
	TimerInit(&timer5seg);
	TimerStart(timer5seg.timer);
	UartInit(&mi_uart);

	GPIOInit(GPIO_19, GPIO_INPUT);
	dht11Init(GPIO_19);

	AnalogInputInit(&entrada_analogica);

	xTaskCreate(&enviar_hyt_UART, "hyt", 1024, &mi_uart, 5, &humedad_temp_tarea_handle);
	xTaskCreate(&enviar_rad_UART, "rad", 1024, &mi_uart, 5, &radiacion_tarea_handle);

	SwitchActivInt(SWITCH_1, encender(), NULL);
	SwitchActivInt(SWITCH_2, apagar(), NULL);
}

/*==================[end of file]============================================*/