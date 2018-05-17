/* 
 * File:   sampleTask.h
 * Author: Tincho
 *
 * Created on 2 de mayo de 2018, 18:35
 */

#include    <stdint.h>
#include    <stdlib.h>

#include    "typedef.h"
#include    "perifericos/adc.h"
#include    "perifericos/led.h"
#include    "utilities.h"

#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/timers.h"
#include "freeRTOSConfig.h"

#ifndef SAMPLETASK_H
#define	SAMPLETASK_H

//Software TIMERS
#define T_MUESTREO_PASIVO_S                 6
#define T_MUESTREO_ACTIVO_S                 3
#define T_MUESTREO_DATO_S                   1
#define T_ANTIREBOTE_LLUVIA_MS              100


/* El siguiente ENUM representa los diferentes estados de la FSM de la tarea */
typedef enum
{
    SYNC_SERVER_TIME,
    ASYNC_SAMPLING,
    SYNC_SAMPLING,
    SAVE_AND_PACKAGE        
} SAMPLING_STATUS;

/* El siguiente ENUM representa los diferentes sensores que notificarán a la 
 * tarea vTaskSample que necesitan ser leídos/sensados.
 * Estos se pueden agrupar según su periodicidad. */
typedef enum
{
    ASYNC_SENSOR1,
    ASYNC_SENSOR2,
    ASYNC_SENSOR3,
    ASYNC_SENSOR4,        
    SENSOR_1,
    SENSOR_2,
    SENSOR_3,
    SINCRONICAS,
    CLOSE_SAMPLE        
} EVENT_ID;

typedef enum
{
	muestras = 1,
	reservado,
	configuracion,
	registro
}putDataSecuence_t;

void vTaskSample( void *pvParameters );
void startSampleTask();

uint16_t getData_Sensor_1();
uint16_t getData_Sensor_2();

uint8_t     bcd2dec( uint8_t bcd );
uint16_t	swapBytes( uint16_t var );

void init_sample(muestra_t *muestra);
void assembleSample(muestra_t *muestra);

void prepareSample(trama_muestra_t *tramaMuestra, muestra_t *muestraAlmacenada);
uint8_t prepareSampleToSend(trama_muestra_t *tramaMuestra, char *tramaGPRS);

static void softwareTimers_init();



#endif	/* SAMPLETASK_H */

