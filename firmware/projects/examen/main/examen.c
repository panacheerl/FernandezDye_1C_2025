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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

TaskHandle_t humedad_temp_tarea_handle = NULL;
TaskHandle_t radiacion_tarea_handle = NULL;

char Temp_Humedad[30] = "Temperatura: %d ºC - Húmedad: %d % \n";
char Riesgo_Nevada[30] = "RIESGO DE NEVADA\n";
char Radiacion[30] = "Radiación %d mR/h \n";
char Riesgo_Rad[30] = "RADIACIÓN ELEVADA\n";

float temp = 0.0;
float hum = 0.0;
uint16_t rad = 0;

serial_config_t mi_uart = {
	.port = UART_PC,
	.baud_rate = 115200,
	.func_p = NULL,
	.param_p = NULL};

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

analog_input_config_t entrada_analogica = {
	.func_p = NULL,
	.input = CH0,
	.mode = ADC_SINGLE,
	.sample_frec = 0,
	.param_p = NULL};

/*==================[internal functions declaration]=========================*/

void enviar_hyt_UART(void *param)
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		Medir_Temp_Hum();
		char msg[30];
		sprintf(msg, Temp_Humedad, temp, hum);
		UartSendString(msg);
		if (hum > 85 and temp < 2)
		{
			LedOn(LED_3);
			UartSendString(" - ");
			UartSendString(Riesgo_Nevada);
		}
		else
		{
			LedOff(LED_3);
		}
	}
}

void enviar_rad_UART(void *param)
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		Medir_Temp_Hum();
		char msg[30];
		sprintf(msg, Radiacion, rad);
		UartSendString(msg);

		if (rad > 40)
		{
			LedOn(LED_2);
			UartSendString(" - ");
			UartSendString(Riesgo_Rad);
		}
		else
		{
			LedOff(LED_3);
		}
	}
}


void Notificacion_Timer_1seg(void *param)
{
	vTaskNotifyGiveFromISR(enviar_hyt_UART, pdFALSE);
}
void Notificacion_Timer_5seg(void *param)
{
	vTaskNotifyGiveFromISR(enviar_rad_UART, pdFALSE);
}

void Medir_Temp_Hum(void *param)
{
	dht11Read(&hum, &temp);
}

void Medir_Radiacion(void *param)
{
	// lee 3300 si el valor es 100mR/h
	uint16_t aux = 0;
	AnalogInputReadSingle(entrada_analogica.input, &aux);
	rad = aux * 100 / 3300;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

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
}

/*==================[end of file]============================================*/