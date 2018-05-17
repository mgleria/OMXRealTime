#include "tareas/testTask.h"

TaskHandle_t xTestHandle;

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest( void *pvParameters )
{
    #define TASK_PERIOD_MS 1000
    TickType_t taskDelay;
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
    
    //Loop principal
    for(;;)
    {
        vLedToggleLED(2);
        
//        Espera arbitraria para dormir la tarea        
        vTaskDelay(taskDelay);         
    }
}

void startTestTask(){
    //Funcion de inicialización previa
    
        xTaskCreate(    vTaskTest,
                    "vTaskTest",
                    2000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xTestHandle);
    
}