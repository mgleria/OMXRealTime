#include "tareas/testTask.h"
#include "tareas/sampleTask.h"

#include "ezbl.h"
#include "tmr5.h"
#include "funciones/rtcc.h"
#include <stdio.h>
#include <string.h>

//Handle referenciado en tmr5.c para uso de xTaskNotify()
TaskHandle_t xTestHandle1;
TaskHandle_t xTestHandle2;

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest1( void *pvParameters )
{
    #define TASK1_PERIOD_MS          100

//    UBaseType_t uxHighWaterMark1;
   
    //Loop principal
    for(;;)
    {  
//        uxHighWaterMark1 = uxTaskGetStackHighWaterMark( NULL );
//        Espera arbitraria para dormir la tarea   
//        vTaskDelay(taskDelay);
    } 
}

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest2( void *pvParameters )
{
    #define TASK2_PERIOD_MS          1000
//    size_t heapFree;
//    UBaseType_t uxHighWaterMark2;
    
//    heapFree = xPortGetFreeHeapSize();
    //Loop principal
    for(;;)
    {                    
//        Espera arbitraria para dormir la tarea     
//        vTaskDelay(xMsToTicks(1000));   
//        LEDToggle(0x02);
//        uxHighWaterMark2 = uxTaskGetStackHighWaterMark( NULL );   
    }
}

void startTestTask(){
    //Funcion de inicialización previa
    
    xTaskCreate(    vTaskTest1,
                    "vTaskTest1",
                    1000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xTestHandle1);

        
    xTaskCreate(    vTaskTest2,
                    "vTaskTest2",
                    1000,
                    NULL,
                    MAX_PRIORITY-2,
                    &xTestHandle2);
}