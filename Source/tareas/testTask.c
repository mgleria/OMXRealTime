#include "tareas/testTask.h"
#include "tareas/sampleTask.h"

#include "ezbl.h"
#include "tmr5.h"
#include "funciones/rtcc.h"
#include <stdio.h>
#include <string.h>

//Handle referenciado en tmr5.c para uso de xTaskNotify()
TaskHandle_t xTestHandle;

bool timeout;
//unsigned int (*onReadCallback)(unsigned int bytesRead, void *readData, unsigned int dataLen, EZBL_FIFO *readFIFO));

unsigned int UART_RX_FIFO_OnRead (unsigned int bytesRead, void *readData, unsigned int dataLen, EZBL_FIFO *readFIFO);

static uint32_t notificationFromModem;

TickType_t delayModem;


/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest( void *pvParameters )
{
    #define TASK_PERIOD_MS          3000
    #define MODEM_OFF_TIME_S        15
    #define MODEM_WAIT_RESPONSE_MS  250

    TickType_t taskDelay;
    UBaseType_t uxHighWaterMark1;
  
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
 
    rtcc_t time;
   
    //Loop principal
    for(;;)
    {  
        uxHighWaterMark1 = uxTaskGetStackHighWaterMark( NULL );
        // Espera arbitraria para dormir la tarea   
        vTaskDelay(taskDelay);
        
//        get_rtcc_datetime(&time);
//        printRTCCTime(&time);
        printCurrentRTCCTime();
        
        
        
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
        
//        EZBL_printf("vTaskTest2\n");
        debugUART1("vTaskTest2");
        
//        EZBL_FIFOWriteStr(EZBL_STDOUT,"PRUEBA EZBL_FIFOWriteStr()");
    }
}
unsigned int UART_RX_FIFO_OnRead (unsigned int bytesRead, void *readData, unsigned int dataLen, EZBL_FIFO *readFIFO)
{
    char s[100];
    char data[50];
    sprintf(s, "bytesRead:%i, dataLen:%i", bytesRead, dataLen);
    snprintf(data,bytesRead,"%s\n",readFIFO->tailPtr) ;
    
//    if(bytesRead>0)
//        snprintf(data,bytesRead,"%s\n",readFIFO->tailPtr) ;
//    else
//        strcpy(data,"(0 bytes readed)\n");
    

    debugUART1("--------1--------");
    debugUART1("onReadCallback:");
    debugUART1(s);
    debugUART1(data);
    debugUART1("-------2---------");
    
    return bytesRead;
    
//    void EZBL_FIFOReset(EZBL_FIFO *fifo, void *buffer, unsigned int bufferLen, unsigned int (*onWriteCallback)(unsigned int bytesWritten, void *writeData, unsigned int dataLen, EZBL_FIFO *writeFIFO), unsigned int (*onReadCallback)(unsigned int bytesRead, void *readData, unsigned int dataLen, EZBL_FIFO *readFIFO));
}


//ESTO ESTABA PROVOCANDO UNA EXCEPCION CODIGO 2
//void _U2RXInterrupt(void)
//{
//    char tx[2] = {0,0};
//    
//    tx[0] = U2RXREG;
//    
//    debugUART1(tx);
//}

void startTestTask(){
    //Funcion de inicialización previa
    
    xTaskCreate(    vTaskTest,
                    "vTaskTest",
                    1000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xTestHandle);
        
//    xTaskCreate(    vTaskTest2,
//                    "vTaskTest2",
//                    1000,
//                    NULL,
//                    MAX_PRIORITY-2,
//                    NULL);
}