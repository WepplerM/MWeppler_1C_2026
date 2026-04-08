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

#define N_BITS 4

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void setBcdToGpio(uint8_t bcd, gpioConf_t *gpioArray)
{
    for (int i = 0; i < N_BITS; i++)
    {
        uint8_t bit = (bcd >> i) & 0x01;
        GPIOWrite(gpioArray[i].pin, bit);
    }
}

void app_main(void){
    
    gpioConf_t gpioVector[4] = {
        {GPIO_20, 1},
        {GPIO_21, 1},
        {GPIO_22, 1},
        {GPIO_23, 1}
    };

    // Configurar como salida
    for(int i = 0; i < N_BITS; i++){
        GPIOInit(gpioVector[i].pin, GPIO_OUTPUT);
    }

    // Enviar BCD (ej: 5 → 0101)
    setBcdToGpio(0x05, gpioVector);
}
#include "gpio_mcu.h"

void setBcdToGpio(uint8_t bcd, gpioConf_t *gpioArray)
{
    for(int i = 0; i < 4; i++)
    {
        // Extraer bit i
        uint8_t bit = (bcd >> i) & 0x01;

        // Escribir en el GPIO correspondiente
        GPIOWrite(gpioArray[i].pin, bit);
    }
}
/*==================[end of file]============================================*/