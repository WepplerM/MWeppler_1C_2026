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
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/

struct leds
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;        //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
} my_leds; 

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

enum {ON, OFF, TOGGLE};

/*==================[external functions definition]==========================*/
void controladorLEDS(struct leds *leds_placa)
{
	switch (leds_placa->mode)
	{
		case ON:
			LedOn(leds_placa->n_led);
			break;

		case OFF:
			LedOff(leds_placa->n_led);
			break;
		case TOGGLE:
			for (int i = 0; i < leds_placa->n_ciclos; i++)
			{
				LedToggle(leds_placa->n_led);
				vTaskDelay(leds_placa->periodo / portTICK_PERIOD_MS);
			}
			break;
	}
}

void app_main(void){
	LedsInit();
	my_leds.mode = ON;
	my_leds.n_led = LED_1;
	controladorLEDS(&my_leds);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	my_leds.n_led = LED_2;
	controladorLEDS(&my_leds);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	my_leds.mode = TOGGLE;
	my_leds.n_led = LED_3;
	my_leds.n_ciclos = 10;
	my_leds.periodo = 1000;
	controladorLEDS(&my_leds);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	my_leds.mode = OFF;
	my_leds.n_led = LED_1;
	controladorLEDS(&my_leds);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	my_leds.n_led = LED_2;
	controladorLEDS(&my_leds);

	vTaskDelay(1000 / portTICK_PERIOD_MS);
}
/*==================[end of file]============================================*/