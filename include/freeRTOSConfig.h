/* 
 * File:   freeRTOSConfig.h
 * Author: Tincho
 *
 * Created on 20 de septiembre de 2017, 18:27
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <p24FJ1024GB610.h>
#include "mcc.h"


/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1
#define configUSE_TIME_SLICING          1
#define configUSE_IDLE_HOOK				1
#define configUSE_TICK_HOOK				0
//Cada cuanto interrumpe el tick de FreeRTOS (Hz)
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configCPU_CLOCK_HZ				( FCY )  /* Fosc/2 defined in mcc.h*/

#define configMAX_PRIORITIES			( 6 )
#define MAX_PRIORITY                    configMAX_PRIORITIES-2

#define configMINIMAL_STACK_SIZE		( 200 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) 15*1024 )
#define configMAX_TASK_NAME_LEN			( 4 )

#define configUSE_16_BIT_TICKS			1
#define configIDLE_SHOULD_YIELD			1
#define configCHECK_FOR_STACK_OVERFLOW  2

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES           1
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )
//Para podeer utilizar el mecanismo de notificaciones entre tareas
#define configUSE_TASK_NOTIFICATIONS    1
//Mutexes
#define configUSE_MUTEXES               1

//Software Timers Configgurations
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       configMAX_PRIORITIES-2       
#define configTIMER_TASK_STACK_DEPTH    configMINIMAL_STACK_SIZE*2
#define configTIMER_QUEUE_LENGTH        3

//TraceRecorder
#define configUSE_TRACE_FACILITY        1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               0
#define INCLUDE_vTaskDelete                     0
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskResumeFromISR              1


#define configKERNEL_INTERRUPT_PRIORITY	0x01

/* Integrates the Tracealyzer recorder with FreeRTOS */
#ifndef __LANGUAGE_ASSEMBLY
#if ( configUSE_TRACE_FACILITY == 1 )
#include "trcRecorder.h"
#endif
#endif

#endif /* FREERTOS_CONFIG_H */

