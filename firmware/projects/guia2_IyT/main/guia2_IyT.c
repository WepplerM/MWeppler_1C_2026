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
 * |  EDU-CIAA-NXP  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO    	| 	GPIO_3		|
 * | 	TRIGGER    	| 	GPIO_2		|
 * | 	+5V     	| 	+5V 		|
 * | 	GND     	| 	GND 		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 15/04/2026 | Document creation		                         |
 *
 * @author Weppler Matias (matias.weppler@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hc_sr04.h"
#include "led.h"
#include "switch.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/

// Distancias (cm)
#define DIST_MIN        10
#define DIST_MED        20
#define DIST_MAX        30

// Tiempos (ms)
#define REFRESH_TIME_US 1000000
#define MUX_DELAY       10000

// Estados GPIO
#define DIGIT_OFF       0
#define DIGIT_ON        1

// Cantidad de dígitos
#define DIGITS          3

bool medir = false;
bool mantener = false;

uint16_t distancia = 0;
uint16_t distancia_hold = 0;

/*==================[internal data definition]===============================*/
gpio_t selGpio[3] = {
    GPIO_19,
    GPIO_18,
    GPIO_9
};

gpio_t bcdGpio[4] = {
    GPIO_20,
    GPIO_21,
    GPIO_22,
    GPIO_23
};

TaskHandle_t task_medicion_handle = NULL;

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

void setBcdToGpio(uint8_t bcd, gpio_t *gpioArray)
{
    for(int i = 0; i < 4; i++)
    {
        uint8_t bit = (bcd >> i) & 0x01;
        GPIOState(gpioArray[i], bit);
    }
}

void MostrarDisplay(uint8_t *bcdArray){

    for(int i = 0; i < DIGITS; i++) {

        // Apagar todos los dígitos
        for(int j = 0; j < DIGITS; j++){
            GPIOState(selGpio[j], DIGIT_OFF);
        }

        // Enviar BCD
        setBcdToGpio(bcdArray[i], bcdGpio);

        // Activar dígito
        GPIOState(selGpio[i], DIGIT_ON);

        // Retardo de multiplexado
        for(volatile int k = 0; k < MUX_DELAY; k++);
    }
}

void FuncTimerMedir(void* param){
    vTaskNotifyGiveFromISR(task_medicion_handle, pdFALSE);    /* Envía una notificación a la tarea de medición */
}



static void Task_Medicion(void *pvParameter){

    uint8_t bcdArray[DIGITS];

    while(true){

        // Espera interrupción del timer (cada 1 segundo)
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (medir){

            distancia = HcSr04ReadDistanceInCentimeters();

            if (!mantener){
                distancia_hold = distancia;
            }

            // LEDs
            if (distancia_hold < DIST_MIN){
                LedsOffAll();
            }
            else if (distancia_hold < DIST_MED){
                LedsOffAll();
                LedOn(LED_1);
            }
            else if (distancia_hold < DIST_MAX){
                LedsOffAll();
                LedOn(LED_1);
                LedOn(LED_2);
            }
            else{
                LedsOffAll();
                LedOn(LED_1);
                LedOn(LED_2);
                LedOn(LED_3);
            }

            // BCD
            Convertir_Num_a_BCD(distancia_hold, DIGITS, bcdArray);
        }
        else{
            LedsOffAll();

            for(int i = 0; i < DIGITS; i++){
                GPIOState(selGpio[i], DIGIT_OFF);
            }
        }

        // Mostrar display SIEMPRE (rápido)
        MostrarDisplay(bcdArray);
    }
}

void teclaMedicion(){
    medir = !medir;
}
void teclaMantener(){
    mantener = !mantener;
}
/*==================[external functions definition]==========================*/



void app_main(void){

    LedsInit();
    SwitchesInit();
    HcSr04Init(GPIO_3, GPIO_2);

    SwitchActivInt(SWITCH_1, teclaMedicion, NULL);
    SwitchActivInt(SWITCH_2, teclaMantener, NULL);

    for(int i = 0; i < DIGITS; i++){
        GPIOInit(selGpio[i], GPIO_OUTPUT);
    }

    for(int i = 0; i < 4; i++){
        GPIOInit(bcdGpio[i], GPIO_OUTPUT);
    }
    
    timer_config_t timer_LED_1 = {
    .timer = TIMER_A,
    .period =REFRESH_TIME_US,
    .func_p = FuncTimerMedir,
    .param_p = NULL
    };
    TimerInit(&timer_LED_1);

    xTaskCreate(Task_Medicion, "Medicion", 2048, NULL, 2, &task_medicion_handle);

    TimerStart(timer_LED_1.timer);
}

/*==================[end of file]============================================*/