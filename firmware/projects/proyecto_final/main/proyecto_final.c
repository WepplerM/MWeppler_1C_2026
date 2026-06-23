/*! @mainpage Mate Móvil - Vehículo Bluetooth para Transporte de Mate
*

* @section genDesc General Description
*
* Este proyecto consiste en el desarrollo de un vehículo móvil controlado mediante
* Bluetooth Low Energy (BLE), diseñado para transportar un mate de manera remota.
*
* El sistema está basado en un ESP32-C6 y permite controlar el movimiento del
* vehículo desde una aplicación móvil mediante comandos enviados por Bluetooth.
* El usuario puede ordenar el avance, retroceso, giro hacia ambos lados y detener
* el movimiento del vehículo.
*
* Como funcionalidad adicional, el sistema incorpora un buzzer capaz de reproducir
* melodías RTTTL. Para evitar interferencias con la comunicación Bluetooth, la
* reproducción de sonidos se ejecuta en una tarea independiente de FreeRTOS.
*
* Comandos disponibles:
*
* | Comando | Acción |
* |:--------|:--------|
* | '1' | Avanzar |
* | '2' | Retroceder |
* | '3' | Girar a la derecha |
* | '4' | Girar a la izquierda |
* | '5' | Detener giro |
* | '0' | Detener avance |
* | 'R' | Reproducir melodía |
*
* El estado de la conexión Bluetooth se indica mediante un LED:
*
* * LED apagado: Bluetooth deshabilitado.
* * LED parpadeando: dispositivo desconectado.
* * LED encendido: dispositivo conectado.
*
* @section hardConn Hardware Connection
*
* | Peripheral | ESP32 |
* |:----------:|:-----:|
* | AVANCE B-1A | GPIO18 |
* | AVANCE B-2A | GPIO19 |
* | GIRO A-1A | GPIO20 |
* | GIRO A-1B | GPIO21 |
* | BUZZER | GPIO22 |
*
* Hardware utilizado:
*
* * ESP32-C6-EDA.
* * Driver puente H L9110S.
* * Dos motores DC.
* * Buzzer.
* * Batería Li-Ion 3.7 V.
* * Aplicación Bluetooth Electronics.
*
* @section freertos FreeRTOS Tasks
*
* ### TaskBocina
*
* Tarea encargada de la reproducción de melodías mediante el buzzer.
* La ejecución de la melodía se realiza en una tarea independiente para
* evitar bloquear el procesamiento de eventos Bluetooth.
*
* @section changelog Changelog
*
* | Date | Description |
* |:-----:|:-----------|
* | 03/06/2026 | Document creation |
* | 18/06/2026 | Added BLE control and buzzer task |
*
* @author Matias Weppler ([matias.weppler@ingenieria.uner.edu.ar](mailto:matias.weppler@ingenieria.uner.edu.ar))
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
/** LED indicador del estado Bluetooth */
#define LED_BT LED_1
/** Tiempo de actualización del LED (ms) */
#define CONFIG_BLINK_PERIOD 500

#define MOTOR_POT_B1A GPIO_18
#define MOTOR_POT_B2A GPIO_19
#define MOTOR_DIR_A1A GPIO_20
#define MOTOR_DIR_A1B GPIO_21
/** GPIO conectado al buzzer */
#define BUZZER_PIN GPIO_22


/*==================[internal data definition]===============================*/
/**
 * @brief Último comando recibido por Bluetooth.
 */
volatile char comando = '0';

/**
 * @brief Indica que debe reproducirse la bocina.
 */
volatile bool tocar_bocina = false;

/**
 * @brief Indica que existe un nuevo comando pendiente de procesar.
 */
volatile bool nuevo_comando = false;

/*==================[internal functions declaration]=========================*/

/**
 * @brief Desplaza el vehículo hacia adelante.
 */
void Adelante(void)
{
    GPIOOn(MOTOR_POT_B1A);
    GPIOOff(MOTOR_POT_B2A);

}

/**
 * @brief Desplaza el vehículo hacia atrás.
 */
void Atras(void)
{
    GPIOOff(MOTOR_POT_B1A);
    GPIOOn(MOTOR_POT_B2A);
}
/**
 * @brief Gira el vehículo hacia la derecha.
 */
void Derecha(void)
{
    GPIOOn(MOTOR_DIR_A1A);
    GPIOOff(MOTOR_DIR_A1B);
}

/**
 * @brief Gira el vehículo hacia la izquierda.
 */

void Izquierda(void)
{
    GPIOOff(MOTOR_DIR_A1A);
    GPIOOn(MOTOR_DIR_A1B);
}

/**
 * @brief Detiene el movimiento de avance o retroceso.
 */
void Parar_avance(void)
{
    GPIOOff(MOTOR_POT_B1A);
    GPIOOff(MOTOR_POT_B2A);
}

/**
 * @brief Detiene el giro del vehículo.
 */
void Parar_giro(void){
    GPIOOff(MOTOR_DIR_A1A);
    GPIOOff(MOTOR_DIR_A1B);
}

/**
 * @brief Callback ejecutado al recibir datos por BLE.
 *
 * El primer byte recibido corresponde al comando enviado
 * desde la aplicación móvil.
 *
 * @param data Vector que contiene los datos recibidos.
 * @param length Cantidad de bytes recibidos.
 */

void read_data(uint8_t *data, uint8_t length)
{
    comando = data[0];

    if(comando == 'R')
    {
        tocar_bocina = true;
    }
    else
    {
        nuevo_comando = true;
    }
}

/**
 * @brief Tarea encargada de reproducir la melodía del buzzer.
 *
 * @param pvParameter Parámetro de FreeRTOS (no utilizado).
 */

void TaskBocina(void * pvParameter)
{
    while(1)
    {
        if(tocar_bocina)
        {
            tocar_bocina = false;

            BuzzerPlayRtttl(songLaCucaracha);
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}


/**
 * @brief Tarea encargada del movimiento del vehículo.
 *
 * Procesa los comandos recibidos mediante Bluetooth y ejecuta
 * la acción correspondiente sobre los motores.
 *
 * @param pvParameter Parámetro de FreeRTOS (no utilizado).
 */
void TaskMovimiento(void *pvParameter)
{
    while(1)
    {
        if(nuevo_comando)
        {
            nuevo_comando = false;

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
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/

/**
 * @brief Punto de entrada de la aplicación.
 *
 * Inicializa los periféricos, configura Bluetooth Low Energy,
 * crea las tareas de FreeRTOS y supervisa el estado de conexión
 * mediante un LED indicador.
 */
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
    
    xTaskCreate(
    TaskMovimiento,
    "TaskMovimiento",
    2048,
    NULL,
    5,
    NULL);
    
    xTaskCreate(
    TaskBocina,
    "TaskBocina",
    4096,
    NULL,
    5,
    NULL);

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
}

/*==================[end of file]============================================*/