/*! @mainpage Osciloscopio con señal ECG usando ADC y DAC
 *
 * @section genDesc General Description
 *
 * Este programa implementa una prueba de funcionamiento del osciloscopio
 * desarrollado previamente.
 *
 * Se utiliza una tabla de datos digitales correspondiente a una señal ECG
 * provista por la cátedra. Los valores son enviados periódicamente al DAC
 * para reconstruir una señal analógica.
 *
 * La salida analógica generada se conecta físicamente a la entrada analógica
 * CH1 utilizando el potenciómetro de la placa.
 *
 * El ADC adquiere la señal presente en CH1 y transmite los valores medidos
 * por UART para su visualización en el Serial Plotter.
 *
 * @section hardConn Hardware Connection
 *
 * |  EDU-CIAA-NXP  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	DAC     	| 	GPIO_0		|
 * | 	AD      	| 	GPIO_1		|
 * | 	+5V     	| 	+5V 		|
 * | 	GND     	| 	GND 		|
 *
 * @section changelog Changelog
 *
 * | Date | Description |
 * |:-----:|:-----------|
 * | 20/05/2026 | Implementación prueba ECG con DAC + ADC |
 *
 * @author Matias Weppler matias.weppler@ingenieria.uner.edu.ar
 */

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/

/** @brief Período de muestreo del timer en microsegundos */
#define REFRESH_TIME_US    20000

/** @brief Velocidad de comunicación UART */
#define BAUD_RATE          115200

/*==================[internal data definition]===============================*/

/** @brief Variable donde se almacena el valor leído por ADC */
uint16_t adc_value;

/** @brief Tabla de muestras digitales correspondientes a un ECG */
unsigned char ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,
18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,
29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,
12,10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,
22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,
34,36,38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,
79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,
139,137,133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,
34,31,30,28,27,26,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,
25,25,25,24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,
20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,
18,17,17,17,17,17,17,17
};

/*==================[internal functions declaration]=========================*/

/**
 * @brief Callback ejecutado por TIMER_A.
 *
 * Envía una muestra del arreglo ECG al DAC,
 * lee la señal reconstruida desde CH1 usando el ADC
 * y transmite el valor leído por UART.
 *
 * @param param Parámetro del callback (no utilizado).
 */
void FuncTimerA(void *param)
{
    static uint16_t value = 0;

    /* Generación señal analógica por DAC */
    AnalogOutputWrite(ECG[value]);

    /* Lectura ADC en CH1 */
    AnalogInputReadSingle(CH1, &adc_value);

    /* Envío UART */
    char msg[30];

    sprintf(msg, ">adc:%d\r\n", adc_value);

    UartSendString(UART_PC, msg);

    /* Avance al siguiente punto del ECG */
    value++;

    /* Reinicio del índice al terminar la tabla */
    if (value >= sizeof(ECG))
    {
        value = 0;
    }
}

/*==================[external functions definition]==========================*/

/**
 * @brief Función principal del firmware.
 *
 * Inicializa:
 * - ADC en CH1.
 * - UART para visualización.
 * - DAC para reconstrucción analógica.
 * - TIMER_A para actualización periódica de muestras.
 */
void app_main(void)
{
    /* Configuración ADC */

    analog_input_config_t adc_config = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL
    };

    AnalogInputInit(&adc_config);

    /* Configuración UART */

    serial_config_t uart_config = {
        .port = UART_PC,
        .baud_rate = BAUD_RATE,
        .func_p = NULL,
        .param_p = NULL
    };

    UartInit(&uart_config);

    /* Configuración TIMER */

    timer_config_t timer_adc = {
        .timer = TIMER_A,
        .period = REFRESH_TIME_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    /* Inicialización DAC */

    AnalogOutputInit();

    TimerInit(&timer_adc);

    TimerStart(timer_adc.timer);

    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*==================[end of file]============================================*/