/*! @mainpage Proyecto 1, Ejercicio 6:
 *	Mostrar un numero de formato decimal de 3 digitos en un display LCD
 *
 * @section genDesc General Description
 *
 * Esta aplicacion permite mostrar un numero de formato decimal de 3 cifras
 * en un display LCD, el cual consiste en cifras individuales conformadas 
 * mediante 7 segmentos. Para realizar esta tarea se utiliza una estructura de
 * funciones. Una primera funcion principal es la que recibe el numero a mostrar,
 * el numero de cifras, un vector de pines en los cuales se enviara el numero en
 * formato bcd y un vector de cifras, el cual controla en que posicion se dibujara
 * el numero enviado.
 * Esta funcion, a su vez, utiliza otras 2 en forma complementaria. Una convierte 
 * el numero inicial en un arreglo de 3 cifras individuales. Una vez obtenido este
 * arreglo, comienza un bucle el cual llama a la funcion convertir a GPIO, traduciendo
 * a la configuracion de pines requerida por el display. Luego, se envia un pulso
 * en el pin de cifra correspondiente. Al realizarse este proceso 3 veces, se obtiene
 * el numero solicitado en el display. 
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Display  	|   ESP32   	|
 * |:--------------:|:--------------|
 * | 	D1	 		| 	GPIO_20		|
 * | 	D2	 		| 	GPIO_21		|
 * | 	D3	 		| 	GPIO_22		|
 * | 	D4	 		| 	GPIO_23		|
 * | 	SEL_1 		|	GPIO_19		|
 * | 	SEL_2 		|	GPIO_18		|
 * | 	SEL_3 		|	GPIO_9		|
 * | 	+5V	 		|	+5V			|	
 * | 	GND	 		|	GND			|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/04/2025 | Creacion de documentacion                      |
 *
 * @author Fernandez Dye Juan Francisco (kioru.juan@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/


/*==================[internal data definition]===============================*/

/** @def gpioConf_t
 *  @brief Estructura que configura los pines como entradas o salidas
 */

typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir;	/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/

/** @fn void convertirBcdAGpio
 * @brief Convierte un digito en formato BCD a una configuracion de pines definida por el usuario
 * @param numeroBCD Representa el digito en formato BCD almacenado como un entero sin signo de 8 bits
 * @param configuracionGpio Vector que almacena la configuracion de pines de salida del digito
 * @return
 */

void convertirBcdAGpio(uint8_t numeroBCD, gpioConf_t *configuracionGpio);
/** @fn uint8_t ConvertirBCDAArray
 * @brief Convierte un numero decimal de 32bits a un arreglo de digitos que conforman ese numero
 * @param data numero en formato decimal almacenado como un entero sin signo de 32 bits
 * @param digits numero en formato decimal que representa la cantidad de digitos que conforman el numero
 * @param bcd_number Arreglo de numeros enteros de 8bits, donde sera almacenado cada digito del numero original
 * @return
 */
uint8_t convertirBcdAArray(uint32_t data, uint8_t digits, uint8_t *bcd_number);


/** @fn void datoAPantalla
 * @brief Funcion que muestra un entero decimal de 32bits en un display LCD
 * @param dato Numero en formato decimal almacenado como un entero sin signo de 32 bits
 * @param digitos Numero en formato decimal que representa la cantidad de digitos que conforman el numero
 * @param configuracionGpioDigito Vector que almacena la configuracion de pines de salida de BCD del display
 * @param configuracionGpioCifras Vector que almacena la configuracion de pines de salida referidos a la posicion del digito
 * @return
 */
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

		for (int i = 0; i < 4; i++)
	{
		GPIOOff(vector_pines[i].pin);
	}
			for (int i = 0; i < 3; i++)
	{
		GPIOOff(vector_cifras[i].pin);
	}

	uint32_t numero = 256;
	uint8_t digitos = 3;

	datoAPantalla(numero, digitos, vector_pines, vector_cifras);

}

void convertirBcdAGpio(uint8_t numeroBCD, gpioConf_t *configuracionGpio)
{

	if (numeroBCD & 1)
	{
		GPIOOn(configuracionGpio[0].pin);
	}
	else
	{

		GPIOOff(configuracionGpio[0].pin);
	}
	if (numeroBCD & 2)
	{

		GPIOOn(configuracionGpio[1].pin);
	}
	else
	{

		GPIOOff(configuracionGpio[1].pin);
	}
	if (numeroBCD & 4)
	{

		GPIOOn(configuracionGpio[2].pin);
	}
	else
	{

		GPIOOff(configuracionGpio[2].pin);
	}
	if (numeroBCD & 8)
	{

		GPIOOn(configuracionGpio[3].pin);
	}
	else
	{

		GPIOOff(configuracionGpio[3].pin);
	}
}

uint8_t convertirBcdAArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	// data : dato de 32 bits
	// digits : numero de digitos
	// bcd_number : arreglo donde se guarda el dato

	uint8_t aux1 = data;
	uint8_t aux2;
	for (int i = 0; i < digits; i++)
	{

		aux2 = aux1 % 10;
		bcd_number[digits-i-1] = aux2;
		aux1 = aux1 / 10;
	}

	for (int i = digits - 1; i >= 0; i--)
	{
		printf("%d", bcd_number[i]);
	}

	return 0;
}

void datoAPantalla(uint32_t dato, uint8_t digitos, gpioConf_t *configuracionGpioDigito, gpioConf_t *configuracionGpioCifras)
{

	uint8_t numeroBCD[digitos];

	convertirBcdAArray(dato, digitos, &numeroBCD);

	for (int i = 0; i < digitos; i++)
	{

		convertirBcdAGpio(numeroBCD[i], configuracionGpioDigito);
		GPIOOn(configuracionGpioCifras[i].pin);
		GPIOOff(configuracionGpioCifras[i].pin);
	}
}

/*==================[end of file]============================================*/