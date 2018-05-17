/* 
 * File:   testTask.h
 * Author: Tincho
 *
 * Created on 2 de mayo de 2018, 18:32
 */

#include "tareas/sampleTask.h"
#include "perifericos/led.h"

#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/timers.h"
#include "freeRTOSConfig.h"

#ifndef TESTTASK_H
#define	TESTTASK_H

void vTaskTest( void *pvParameters ); 
void startTestTask();

#endif	/* TESTTASK_H */

