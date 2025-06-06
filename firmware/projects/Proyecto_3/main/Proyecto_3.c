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



#include "led.h"
#include "neopixel_stripe.h"
#include "ble_mcu.h"
/*==================[macros and definitions]=================================*/

#define pulgar 0
#define indice 1
#define numero_dedos 2

#define CONFIG_BLINK_PERIOD 500
#define LED_BT	LED_1

/*==================[internal data definition]===============================*/

bool enviar_datos = true;
uint16_t maxima_extension = 3300;
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
			
			// UartSendString(UART_PC, "$");

			// for (int i = 0; i < numero_dedos; i++)
			// {
			// 	if (i > 0)
			// 		UartSendString(UART_PC, " ");
			// 	UartSendString(UART_PC, (char *)UartItoa(mediciones[i], 10));
			// }
			// UartSendString(UART_PC, ";");

			    sprintf(msg, "*I%d* *M%d*\n", mediciones[0], mediciones[1]);
    		BleSendString(msg);
		}
	}
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
}

// void read_data(uint8_t * data, uint8_t length){
// 	uint8_t i = 1;
//     static uint8_t red = 0, green = 0, blue = 0;
// 	char msg[30];

// 	if(data[0] == 'R'){
//         /* El slidebar Rojo envía los datos con el formato "R" + value + "A" */
// 		red = 0;
// 		while(data[i] != 'A'){
//             /* Convertir el valor ASCII a un valor entero */
// 			red = red * 10;
// 			red = red + (data[i] - '0');
// 			i++;
// 		}
// 	}else if(data[0] == 'G'){   
//         /* El slidebar Verde envía los datos con el formato "G" + value + "A" */
// 		green = 0;
// 		while(data[i] != 'A'){
//             /* Convertir el valor ASCII a un valor entero */
// 			green = green * 10;
// 			green = green + (data[i] - '0');
// 			i++;
// 		}
// 	}else if(data[0] == 'B'){
//         /* El slidebar Azul envía los datos con el formato "B" + value + "A" */
// 		blue = 0;
// 		while(data[i] != 'A'){
//             /* Convertir el valor ASCII a un valor entero */
// 			blue = blue * 10;
// 			blue = blue + (data[i] - '0');
// 			i++;
// 		}
// 	}
//     NeoPixelAllColor(NeoPixelRgb2Color(red, green, blue));
//     /* Se envía una realimentación de los valores actuales de brillo del LED */
//     sprintf(msg, "R: %d, G: %d, B: %d pan\n ", red, green, blue);
//     BleSendString(msg);
// }

/*==================[external functions definition]==========================*/
void app_main(void)
{

	// serial_config_t mi_uart = {
	// 	.port = UART_PC,
	// 	.baud_rate = 9600,
	// 	.func_p = NULL,
	// 	.param_p = NULL};

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

		

	TimerInit(&timer_medicion);

	AnalogInputInit(&input_dedo_1);
	AnalogInputInit(&input_dedo_2);

	//UartInit(&mi_uart);
	xTaskCreate(&medir_dedos, "medir", 2048, NULL, 5, &timer_medicion_handle);
	xTaskCreate(&tarea_enviar_datos, "DATOS", 2048, NULL, 5, &enviar_datos_tarea_handle);

	TimerStart(timer_medicion.timer);





 
    ble_config_t ble_configuration = {
        "ESP_J_F",
        BLE_NO_INT
    };

    LedsInit();
    BleInit(&ble_configuration);
 
    while(1){
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleStatus()){
            case BLE_OFF:
                LedOff(LED_BT);
            break;
            case BLE_DISCONNECTED:
                LedToggle(LED_BT);
            break;
            case BLE_CONNECTED:
                LedOn(LED_BT);
            break;
        }
    }



}
/*==================[end of file]============================================*/