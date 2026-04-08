/*! @mainpage Template
 *
 * @section genDesc Conversión de número a BCD y visualización en display 7 segmentos
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_1	 	| 	GPIO_20		|
 * | 	PIN_2	 	| 	GPIO_21		|
 * | 	PIN_3	 	| 	GPIO_22		|
 * | 	PIN_4	 	| 	GPIO_23		|
 * | 	SEL_1	 	| 	GPIO_19		|
 * | 	PIN_2	 	| 	GPIO_18		|
 * | 	PIN_3	 	| 	GPIO_9		|
 * | 	+5V  	 	| 	+5V	    	|
 * | 	GND	 	    | 	GND		    |
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 8/04/2026  | Document creation		                         |
 *
 * @author Weppler Matias (matias.weppler@ingenieriea.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"
typedef struct
{
    gpio_t pin;
    io_t dir;
} gpioConf_t;
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t Convertir_Num_a_BCD(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
    for(int i = digits - 1; i >= 0; i--)
    {
        bcd_number[i] = data % 10;
        data = data / 10;
    }
    return 0;
}

void setBcdToGpio(uint8_t bcd, gpioConf_t *gpioArray)
{
    for(int i = 0; i < 4; i++)
    {
        uint8_t bit = (bcd >> i) & 0x01;
        GPIOState(gpioArray[i].pin, bit);
    }
}

void displayNumber(uint32_t data, uint8_t digits, 
                   gpioConf_t *bcdGpio, 
                   gpioConf_t *selGpio);

/*==================[external functions definition]==========================*/
/**
 * @brief Muestra un número en un display multiplexado usando BCD
 *
 * Esta función recibe un número de 32 bits, lo convierte a un arreglo
 * de dígitos decimales (BCD) utilizando la función convertToBcdArray,
 * y luego muestra cada dígito en un display LCD multiplexado utilizando
 * los GPIOs configurados.
 *
 * @param data Número de 32 bits a mostrar
 * @param digits Cantidad de dígitos a visualizar
 * @param bcdGpio Vector de GPIOs para los bits BCD (D1-D4)
 * @param selGpio Vector de GPIOs para selección de dígitos del display
 */
void mostrar_numero(uint32_t data, uint8_t digits, 
                   gpioConf_t *bcdGpio, 
                   gpioConf_t *selGpio)
{
    uint8_t bcdArray[10];

    // Convertir número a arreglo de dígitos (Ejercicio 4)
    Convertir_Num_a_BCD(data, digits, bcdArray);

    // Multiplexado de display
    for(int i = 0; i < digits; i++)
    {
        // Apagar todos los dígitos
        for(int j = 0; j < digits; j++){
            GPIOState(selGpio[j].pin, 0);
        }

        // Enviar el dígito actual (Ejercicio 5)
        setBcdToGpio(bcdArray[i], bcdGpio);

        // Activar el dígito correspondiente
        GPIOState(selGpio[i].pin, 1);

        // Retardo simple para visualización
        for(volatile int k = 0; k < 10000; k++);
    }

}
gpioConf_t bcdGpio[4] = {
    {GPIO_20, 1},
    {GPIO_21, 1},
    {GPIO_22, 1},
    {GPIO_23, 1}
};
gpioConf_t selGpio[3] = {
    {GPIO_19, 1}, // Dígito 1
    {GPIO_18, 1}, // Dígito 2
    {GPIO_9, 1}   // Dígito 3
};
void app_main(void)
{
    // Inicializar BCD
    for(int i = 0; i < 4; i++){
        GPIOInit(bcdGpio[i].pin, GPIO_OUTPUT);
    }

    // Inicializar selección
    for(int i = 0; i < 3; i++){
        GPIOInit(selGpio[i].pin, GPIO_OUTPUT);
    }
    mostrar_numero(100, 3, bcdGpio, selGpio);

}
/*==================[end of file]============================================*/