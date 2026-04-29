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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t Convertir_Num_a_BCD(uint32_t data, uint8_t digits, uint8_t *bcd_number);

/*==================[external functions definition]==========================*/
void app_main(void){
    uint8_t resultado[5];

    Convertir_Num_a_BCD(531, 5, resultado);

    for(int i = 0; i < 5; i++){
        printf("%d ", resultado[i]);
    }
}
/*==================[end of file]============================================*/