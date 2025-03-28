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
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir;	/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void convertirBcdAGpio(uint8_t numeroBCD, gpioConf_t *configuracionGpio);
/*==================[external functions definition]==========================*/
void app_main(void)
{

	GPIOInit(GPIO_20, 1);
	GPIOInit(GPIO_21, 1);
	GPIOInit(GPIO_22, 1);
	GPIOInit(GPIO_23, 1);

	gpioConf_t vector_pines[4];
	vector_pines[0].pin = GPIO_20;
	vector_pines[1].pin = GPIO_21;
	vector_pines[2].pin = GPIO_22;
	vector_pines[3].pin = GPIO_23;

	convertirBcdAGpio(5, vector_pines);

	for (int i = 0; i < 4; i++)
	{

		printf("%d", vector_pines[i].dir);
		
	}

	convertirBcdAGpio(9, vector_pines);

	for (int i = 0; i < 4; i++)
	{

		printf("%d", vector_pines[i].dir);
	}

	convertirBcdAGpio(3, vector_pines);

	for (int i = 0; i < 4; i++)
	{

		printf("%d", vector_pines[i].dir);
	}

}

void convertirBcdAGpio(uint8_t numeroBCD, gpioConf_t *configuracionGpio)
{

	if (numeroBCD & 1)
	{
		configuracionGpio[3].dir = 1;
	}
	else
	{
		configuracionGpio[3].dir = 0;
	}
	if (numeroBCD & 2)
	{
		configuracionGpio[2].dir = 1;
	}
	else
	{
		configuracionGpio[2].dir = 0;
	}
	if (numeroBCD & 4)
	{
		configuracionGpio[1].dir = 1;
	}
	else
	{
		configuracionGpio[1].dir = 0;
	}
	if (numeroBCD & 8)
	{
		configuracionGpio[0].dir = 1;
	}
	else
	{
		configuracionGpio[0].dir = 0;
	}
}

/*==================[end of file]============================================*/