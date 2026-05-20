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

#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
#define REFRESH_TIME_US    20000

#define BAUD_RATE		   115200

/*==================[internal data definition]===============================*/

uint16_t adc_value;

unsigned char ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17
} ;

/*==================[internal functions declaration]=========================*/
void FuncTimerA(void *param)
{
    static uint16_t value=0;

    AnalogOutputWrite(ECG[value]);

    AnalogInputReadSingle(CH1, &adc_value);

    char msg[30];

    sprintf(msg, ">adc:%d\r\n", adc_value);

    UartSendString(UART_PC, msg);

    value++;

    if (value >= sizeof(ECG))
    {
        value = 0;
    }
}
/*==================[external functions definition]==========================*/
void app_main(void)
{
    /*----------- Configuración ADC -----------*/

    analog_input_config_t adc_config = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL
    };

    AnalogInputInit(&adc_config);

    /*----------- Configuración UART -----------*/

    serial_config_t uart_config = {
        .port = UART_PC,
        .baud_rate = BAUD_RATE,
        .func_p = NULL,
        .param_p = NULL
    };

    UartInit(&uart_config);

    /*----------- Configuración Timer -----------*/

    timer_config_t timer_adc = {
        .timer = TIMER_A,
        .period = REFRESH_TIME_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    AnalogOutputInit();

    TimerInit(&timer_adc);

    TimerStart(timer_adc.timer);

    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/*==================[end of file]============================================*/