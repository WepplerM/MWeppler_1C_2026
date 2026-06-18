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

#include "freertos/FreeRTOS.h"
#include"freertos/task.h"

#include "ble_mcu.h"
#include "led.h"
#include "gpio_mcu.h"

#include "buzzer.h"
#include "buzzer_melodies.h"
/*==================[macros and definitions]=================================*/

#define LED_BT LED_1
#define CONFIG_BLINK_PERIOD 500

#define MOTOR_POT_B1A GPIO_18
#define MOTOR_POT_B2A GPIO_19
#define MOTOR_DIR_A1A GPIO_20
#define MOTOR_DIR_A1B GPIO_21

#define BUZZER_PIN GPIO_22


/*==================[internal data definition]===============================*/

volatile char comando = '0';

/*==================[internal functions declaration]=========================*/
void Adelante(void)
{
    GPIOOn(MOTOR_POT_B1A);
    GPIOOff(MOTOR_POT_B2A);

    //GPIOOn(MOTOR_R_A);
    //GPIOOff(MOTOR_L_B);
}

void Atras(void)
{
    GPIOOff(MOTOR_POT_B1A);
    GPIOOn(MOTOR_POT_B2A);

    //GPIOOff(MOTOR_R_A);
    //GPIOOn(MOTOR_L_B);
}

void Derecha(void)
{
    GPIOOn(MOTOR_DIR_A1A);
    GPIOOff(MOTOR_DIR_A1B);

    //GPIOOff(MOTOR_R_A);
    //GPIOOn(MOTOR_L_B);
}

void Izquierda(void)
{
    GPIOOff(MOTOR_DIR_A1A);
    GPIOOn(MOTOR_DIR_A1B);

    //GPIOOn(MOTOR_R_A);
    //GPIOOff(MOTOR_L_B);
}

void Parar_avance(void)
{
    GPIOOff(MOTOR_POT_B1A);
    GPIOOff(MOTOR_POT_B2A);
}

void Parar_giro(void){
    GPIOOff(MOTOR_DIR_A1A);
    GPIOOff(MOTOR_DIR_A1B);
}

void read_data(uint8_t * data, uint8_t length){

    comando = data[0];

    switch(comando)
    {
        case '1':
            Adelante();
            break;

        case '2':
            Atras();
            break;

        case '3':
            Derecha();
            break;

        case '4':
            Izquierda();
            break;
		
		case '5':
			Parar_giro();
			break;

		case '0':
			Parar_avance();
			break;

        case 'R':
            BuzzerPlayRtttl(songLaCucaracha);
            break;
}
}

/*==================[external functions definition]==========================*/

void app_main(void){
    ble_config_t ble_configuration = {
        "MATE_MOVIL",
        read_data
    };

    GPIOInit(MOTOR_POT_B1A, GPIO_OUTPUT);
    GPIOInit(MOTOR_POT_B2A, GPIO_OUTPUT);
    GPIOInit(MOTOR_DIR_A1A, GPIO_OUTPUT);
    GPIOInit(MOTOR_DIR_A1B, GPIO_OUTPUT);
    Parar_avance();
    Parar_giro();
	
    LedsInit();
    BuzzerInit(BUZZER_PIN);
	BleInit(&ble_configuration);

	while(1)
    {
        switch(BleStatus())
        {
            case BLE_OFF:
                LedOff(LED_BT);
                break;

            case BLE_DISCONNECTED:
                LedToggle(LED_BT);
                break;

            case BLE_CONNECTED:
                LedOn(LED_BT);
                break;
        }

        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
/*     while(1)
    {
        Adelante();
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        Parar_avance();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    } */

}

/*==================[end of file]============================================*/