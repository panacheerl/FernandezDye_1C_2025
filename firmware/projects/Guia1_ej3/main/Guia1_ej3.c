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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

struct leds
{
      uint8_t mode;     //  ON, OFF, TOGGLE
	uint8_t n_led;      //  indica el número de led a controlar
	uint8_t n_ciclos;  // indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;   // indica el tiempo de cada ciclo
} my_leds; 

void app_main(void){





	LedsInit();
	SwitchesInit();

	struct leds programa_1;
	programa_1.mode = TOGGLE;
	programa_1.n_led = 1;
	programa_1.n_ciclos = 5; 
	struct leds programa_2;
	programa_2.mode = ON;
	programa_2.n_led = 2;

	struct &leds activo;




if(&activo.mode == ON){
	if (&my_leds.n_led == 1) {
		LedOn(LED_1);
	} else if (&my_leds.n_led == 2){
		LedOn(LED_2);
	} else if (&my_leds.n_led == 3) {
		LedOn(LED_3);
	}
} else if (&my_leds.mode == OFF){
		if (&my_leds.n_led == 1) {
		LedOff(LED_1);
	} else if (&my_leds.n_led == 2){
		LedOff(LED_2);
	} else if (&my_leds.n_led == 3) {
		LedOff(LED_3);
	}
} else if (&my_leds.mode == TOOGLE){

for (int i=0;i<&my_leds.n_ciclos; i++)

		if (&my_leds.n_led == 1) {
		LedToggle(LED_1);
	} else if (&my_leds.n_led == 2){
		LedToggle(LED_2);
	} else if (&my_leds.n_led == 3) {
		LedToggle(LED_3);
	}


}

	
	
}

/*==================[end of file]============================================*/