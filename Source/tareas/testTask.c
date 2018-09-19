#include "tareas/testTask.h"
#include "tareas/sampleTask.h"

#include "ezbl.h"

TaskHandle_t xTestHandle;

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest( void *pvParameters )
{
    #define TASK_PERIOD_MS          50000
    #define MODEM_OFF_TIME_S        15

//    TickType_t taskDelay, modemResetTime;
//    
//    taskDelay = xMsToTicks(TASK_PERIOD_MS);
//    modemResetTime = xSegToTicks (MODEM_OFF_TIME_S);
//    
//    string	atcmd_getModemID[]				=	"AT+GMR\r\n";
//    
//    //Buffer de comunicación entrante y saliente con el modem
//    static char gprsBuffer[GPRS_BUFFER_SIZE]={0};
//    static char header[FRAME_HEADER_SIZE]={0};
//    static headerOptions_t headerIndex; 
//    
//    const char ch = '\n';
//    char *ret;
//    
    unsigned long ledBlinkTimer;
    ledBlinkTimer = NOW_32();

    size_t heapFree;
    UBaseType_t uxHighWaterMark1;
    
    heapFree = xPortGetFreeHeapSize();
    
    
    
    //Loop principal
    for(;;)
    {
        
        
        // Every half second toggle an LED (1 Hz blink rate) to indicate we are alive
//        if(NOW_32() - ledBlinkTimer > NOW_sec/2u)
//        {
//            LEDToggle(0x01);
//            ledBlinkTimer += NOW_sec/2u;
//            ClrWdt();
//        }
        
//        //Apago el modem y espero MODEM_RESET_TIME_S para encenderlo
//        _LATB12 = 0;
//        printf("Modem OFF\r\n");
//        vTaskDelay(modemResetTime);
//        _LATB12 = 1;
//        printf("Modem ON. Waiting for complete boot...\r\n");
//        vTaskDelay(modemResetTime*2);
//        printf("Modem initialization complete\r\n");
//        
//        EZBL_FIFOWriteStr(EZBL_STDOUT,atcmd_getModemID);
//        
//        EZBL_FIFORead(gprsBuffer,EZBL_STDIN,100);
//        
//        printf("gprsBuffer:%s\r\n");
        
        
                
//        Espera arbitraria para dormir la tarea     
        vTaskDelay(xMsToTicks(500));
//        vTaskDelay(pdMS_TO_TICKS(1000));
        
        LEDToggle(0x01);
        
        uxHighWaterMark1 = uxTaskGetStackHighWaterMark( NULL );  
        
        ClrWdt();
        
        
        
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
    size_t heapFree;
    UBaseType_t uxHighWaterMark2;
    
    heapFree = xPortGetFreeHeapSize();
    //Loop principal
    for(;;)
    {                    
//        Espera arbitraria para dormir la tarea     
        vTaskDelay(xMsToTicks(1000));
        
        LEDToggle(0x02);

        uxHighWaterMark2 = uxTaskGetStackHighWaterMark( NULL );   
    }
}

void startTestTask(){
    //Funcion de inicialización previa
    
    xTaskCreate(    vTaskTest,
                    "vTaskTest",
                    1000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xTestHandle);
        
    xTaskCreate(    vTaskTest2,
                    "vTaskTest2",
                    1000,
                    NULL,
                    MAX_PRIORITY-1,
                    NULL);
}