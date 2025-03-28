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
uint8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number);

void datoAPantalla(uint32_t dato, uint8_t digitos, gpioConf_t *configuracionGpioDigito, gpioConf_t *configuracionGpioCifras);

/*==================[external functions definition]==========================*/
void app_main(void)
{

	GPIOInit(GPIO_20, GPIO_OUTPUT);
	GPIOInit(GPIO_21, GPIO_OUTPUT);
	GPIOInit(GPIO_22, GPIO_OUTPUT);
	GPIOInit(GPIO_23, GPIO_OUTPUT);
	GPIOInit(GPIO_19, GPIO_OUTPUT);
	GPIOInit(GPIO_18, GPIO_OUTPUT);
	GPIOInit(GPIO_9, GPIO_OUTPUT);

	gpioConf_t vector_pines[4];
	vector_pines[0].pin = GPIO_20;
	vector_pines[1].pin = GPIO_21;
	vector_pines[2].pin = GPIO_22;
	vector_pines[3].pin = GPIO_23;

	gpioConf_t vector_cifras[3];
	vector_cifras[0].pin = GPIO_19;
	vector_cifras[1].pin = GPIO_18;
	vector_cifras[2].pin = GPIO_9;

	datoAPantalla(325, 3, vector_pines, vector_cifras);
}

void convertirBcdAGpio(uint8_t numeroBCD, gpioConf_t *configuracionGpio)
{

	if (numeroBCD & 1)
	{
		GPIOOn(configuracionGpio[3].pin);
		
	}
	else
	{
		
		GPIOOff(configuracionGpio[3].pin);
	}
	if (numeroBCD & 2)
	{
		
		GPIOOn(configuracionGpio[2].pin);
	}
	else
	{
		
		GPIOOff(configuracionGpio[2].pin);
	}
	if (numeroBCD & 4)
	{
		
		GPIOOn(configuracionGpio[1].pin);
	}
	else
	{
		
		GPIOOff(configuracionGpio[1].pin);
	}
	if (numeroBCD & 8)
	{
		
		GPIOOn(configuracionGpio[0].pin);
	}
	else
	{
		
		GPIOOff(configuracionGpio[0].pin);
	}
}

uint8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	// data : dato de 32 bits
	// digits : numero de digitos
	// bcd_number : arreglo donde se guarda el dato

	uint8_t aux1 = data;
	uint8_t aux2;
	for (int i = 0; i < digits; i++)
	{

		aux2 = aux1 % 10;
		bcd_number[i] = aux2;
		aux1 = aux1 / 10;
	}

	return 0;
}

void datoAPantalla(uint32_t dato, uint8_t digitos, gpioConf_t *configuracionGpioDigito, gpioConf_t *configuracionGpioCifras)
{

	uint8_t numeroBCD[digitos];

	convertToBcdArray(dato, digitos, numeroBCD);

	for (int i = 0; i < digitos; i++)
	{

		convertirBcdAGpio(numeroBCD[i], configuracionGpioDigito);
		// for (int j = 0; j < 4; j++)
		// {
		// 	if (configuracionGpioDigito[i].dir == 1)
		// 	{
		// 		GPIOOn(configuracionGpioDigito[i].pin);
		// 	} else {
		// 		GPIOOff(configuracionGpioDigito[i].pin);
		// 	}
		// }
	
		GPIOOn(configuracionGpioCifras[i].pin);
		GPIOOff(configuracionGpioCifras[i].pin);
	}
}

/*==================[end of file]============================================*/