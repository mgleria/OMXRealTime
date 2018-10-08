#include "tareas/testTask.h"
#include "tareas/sampleTask.h"

#include "ezbl.h"
#include "tmr5.h"
#include <stdio.h>
#include <string.h>

TaskHandle_t xTestHandle;
bool timeout;
//unsigned int (*onReadCallback)(unsigned int bytesRead, void *readData, unsigned int dataLen, EZBL_FIFO *readFIFO));

unsigned int UART_RX_FIFO_OnRead (unsigned int bytesRead, void *readData, unsigned int dataLen, EZBL_FIFO *readFIFO);

void _U2RXInterrupt(void);

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest( void *pvParameters )
{
    #define TASK_PERIOD_MS          500
    #define MODEM_OFF_TIME_S        15

    timeout = false;

    TickType_t taskDelay, modemResetTime;
//    
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
    modemResetTime = xSegToTicks (MODEM_OFF_TIME_S);
//    
    string	atcmd_getModemID[]				=	"AT+GMR\n";
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
    
    uint8_t RxBuffer[100] = {0};
    
//    EZBL_STDIN->onReadCallback = UART_RX_FIFO_OnRead;
    
    //Apago el modem y espero MODEM_RESET_TIME_S para encenderlo
    _LATB12 = 0;
    debugUART1("Modem OFF\r\n");
    vTaskDelay(modemResetTime);
    _LATB12 = 1;
    debugUART1("Modem ON. Waiting for complete boot...\r\n");
    vTaskDelay(modemResetTime*2);
    debugUART1("Modem initialization complete\r\n");
    
    TMR5_Stop();
    
    
    //Loop principal
    for(;;)
    {
        //        Espera arbitraria para dormir la tarea     
        vTaskDelay(taskDelay);
        
        cleanDataBuffer(RxBuffer,100);
        
        LEDToggle(0x01);
        
        
        EZBL_printf(atcmd_getModemID);
        debugUART1(">>>>");
        debugUART1(atcmd_getModemID);
        
        uxHighWaterMark1 = uxTaskGetStackHighWaterMark( NULL );
        
        //Espera para esperar que el modem responda
//        vTaskDelay(taskDelay*2);
        
        TMR5_Start();
        
        //Espera bloqueante hasta recibir la respuesta completa del modem
        while(!timeout);
        
        timeout = false;
        TMR5_Stop();
        
        EZBL_FIFORead(RxBuffer,EZBL_STDIN,100);
        debugUART1("<<<<");
        debugUART1(RxBuffer);
        
//        if(EZBL_FIFOFlush(EZBL_STDIN, NOW_sec))
//            debugUART1("EZBL_FIFOFlush return 1");
//        else 
//            debugUART1("EZBL_FIFOFlush return 0");
        
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
                    MAX_PRIORITY-2,
                    NULL);
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

void    cleanDataBuffer(uint8_t *buffer, uint8_t bufferSize)
{
    int i;
    for(i=0;i<bufferSize;i++){
        buffer[i]=NULL;
    }
}