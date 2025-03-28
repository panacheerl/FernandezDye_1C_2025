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
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "esp_mac.h"

/*==================[macros and definitions]=================================*/

#define CONFIG_BLINK_PERIOD 1000
struct leds
{
      uint8_t mode;     //  ON, OFF, TOGGLE  // 1,0, 2
	uint8_t n_led;      //  indica el número de led a controlar
	uint8_t n_ciclos;  // indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;   // indica el tiempo de cada ciclo
} my_leds; 
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void leer_programa(struct leds* parametro);
void parpadeo_de_leds();

/*==================[external functions definition]==========================*/



void app_main(void){





	LedsInit();
	SwitchesInit();


uint8_t teclas;


	
	struct leds programa_1;
	programa_1.mode = 2;
	programa_1.n_led = 1;
	programa_1.n_ciclos = 5; 
	programa_1.periodo = 1000;
	struct leds programa_2;
	programa_2.mode = 1;
	programa_2.n_led = 2;
	struct leds programa_3;
	programa_3.n_led = 3;
	programa_3.mode = 1;
	struct leds programa_4;
	programa_4.mode = 2;
	programa_4.n_led = 3;
	programa_4.n_ciclos = 10;
	programa_4.periodo = 500;

	struct leds* arreglo[4];
	arreglo[0] = &programa_2;
	arreglo[1] = &programa_1;
	arreglo[2] = &programa_3;
	arreglo[3] = &programa_4;

	struct leds* puntero = arreglo[0];
	int indice = 0;

	



	    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
					LedsOffAll();
					indice++;
    				puntero = arreglo[indice];
    		break;
    		case SWITCH_2:
					LedsOffAll();
					indice--;
    				puntero = arreglo[indice];

    		break;
    	}
	    
	leer_programa(puntero);
	vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);


	}
}

void leer_programa(struct leds* parametro){



if(parametro->mode == 1){
	if (parametro->n_led == 1) {
		LedOn(LED_1);
	} else if (parametro->n_led == 2){
		LedOn(LED_2);
	} else if (parametro->n_led == 3) {
		LedOn(LED_3);
	}
} else if (parametro->mode == 0){
		if (parametro->n_led == 1) {
		LedOff(LED_1);
	} else if (parametro->n_led == 2){
		LedOff(LED_2);
	} else if (parametro->n_led == 3) {
		LedOff(LED_3);
	}
} else if (parametro->mode == 2){

parpadeo_de_leds();

for (int i=0 ; i<(2*parametro->n_ciclos) ; i++) {


			if (parametro->n_led == 1) {
			LedToggle(LED_1);
			vTaskDelay(parametro->periodo / portTICK_PERIOD_MS);
	} else if (parametro->n_led == 2){
			LedToggle(LED_2);
			vTaskDelay(parametro->periodo / portTICK_PERIOD_MS);
	} else if (parametro->n_led == 3) {
			LedToggle(LED_3);
			vTaskDelay(parametro->periodo / portTICK_PERIOD_MS);
	}


}
parpadeo_de_leds();


}
}

void parpadeo_de_leds(){
LedsOffAll();
vTaskDelay(10 / portTICK_PERIOD_MS);
LedToggle(LED_1);
LedToggle(LED_2);
LedToggle(LED_3);
vTaskDelay(10 / portTICK_PERIOD_MS);
LedsOffAll();

}







/*==================[end of file]============================================*/