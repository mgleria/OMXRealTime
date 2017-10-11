/*
 * File:   main.c
 * Author: Tincho
 *
 * Created on 20 de septiembre de 2017, 18:56
 */


#include "xc.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "freeRtos/FreeRTOS.h"
#include "freeRtos/task.h"
#include "freeRtos/queue.h"

/* Demo application includes. */
#include "perifericos/lcd.h"
#include "perifericos/adc.h"
#include "perifericos/led.h"
#include "tmr2.h"
#include "uart1.h"

#include "mcc.h"
#include "typedef.h"

#include "funciones/shell.h"
#include "sistema/ext_rtcc.h"

/**********************************************************************************************/
/*	texto del modelo de equipo	*/
#if		defined	(__OMX_S__)
string model[] = "OMX-S";
#elif	defined	(__OMX_S_C__)
string model[] = "OMX-Li";
#elif	defined	(__OMX_T__)
string model[] = "OMX-Tec";
#elif defined (__OMX_N__)
string model[] = "OMX-N";
#else
#error	No se define el modelo de equipo
#endif

/**	Definicion de la version de firmware */
#define	FW_DEVICE_VERSION	(2.00)	///<	version y revision del firmware (entero.fraccional)

/**	Macro para la version y revision de firmware	*/
#define	deviceVersion(ver)	(ver*100)

string version[] = "2.00";


#define mainMAX_STRING_LENGTH				( 20 )
#define bufLen                              ( 15 )
#define DEFAULT_STACK_SIZE                  (1000)  

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

void vTaskFunction( void *pvParameters );

void vTaskSensorADC( void *pvParameters );

void vTaskShell( void *pvParameters );

void vTaskRTC( void *pvParameters ); 

//static void vReceiverTask( void *pvParameters );

//static void vSenderTask( void *pvParameters );
//The vTaskDelay() API function prototype
void vTaskDelay( TickType_t xTicksToDelay ); 
//Funci�n que reemplaza la MACRO provista por freeRTOS para convertir ms a ticks
TickType_t xMainMsToTicks( TickType_t xTimeInMs);

//------------------------------------------------------------------------------
//The queue used to send messages to the LCD task.
static QueueHandle_t xLCDQueue;

//Cola de respuesta generada a los comandos procesados por vTaskShell
//QueueHandle_t xQueueShell;
//xQueueShell = xQueueCreate(1, MAX_RESP_LENGHT);
 
TaskHandle_t xShellHandle;
TaskHandle_t xRTCTaskHandle;

static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

//uint8_t respuestaUART[MAX_RESP_LENGHT];

int main( void )
{
    SYSTEM_Initialize();
    vLedInitialise();
    
    xTaskCreate(    vTaskRTC,
                    "RTC",
                    DEFAULT_STACK_SIZE,
                    NULL,
                    3,
                    &xRTCTaskHandle);
    
//    xTaskCreate(    vTaskShell,
//                    "Shell",
//                    2000,
//                    NULL,
//                    2,
//                    &xShellHandle);
//    
//    xTaskCreate(    vTaskSensorADC, /* Pointer to the function that implements the task. */
//                    "Sensor", /* Text name for the task. This is to facilitate debugging only. */
//                    DEFAULT_STACK_SIZE, /* Stack depth - small microcontrollers will use much less stack than this. */
//                    (void*)pcSensor, /* Pass the text to be printed into the task using the task parameter. */
//                    1, /* This task will run at priority 1. */
//                    NULL ); /* The task handle is not used in this example. */
    
    
    /* Start the task that will control the LCD.  This returns the handle
	to the queue used to write text out to the task. */
//	xLCDQueue = xStartLCDTask();

	/* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}

void vTaskSensorADC( void *pvParameters ){
    //Periodo con el que actuar� la tarea (ms))
    TickType_t xDelayMs;
    TickType_t xLastWakeTime;
    uint8_t numBytesWritten;
    
    unsigned portBASE_TYPE wichLED;
    char *pcSensor;
    char cLcdLine[bufLen];
    uint16_t adcResult;
    
    uint8_t contador =0;
    
    xDelayMs = xMainMsToTicks(1000);

    numBytesWritten = 0;
    xLastWakeTime = xTaskGetTickCount();
    pcSensor = ( char * ) pvParameters;
    wichLED = 0;
    
    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
    /* The message that is sent on the queue to the LCD task.  The first
    parameter is the minimum time (in ticks) that the message should be
    left on the LCD without being overwritten.  The second parameter is a pointer
    to the message to display itself. */
    xLCDMessage xMessage = { 0, cStringBuffer };
    
    for( ;; ){
        vTaskDelay(xDelayMs);
        
        contador++;
        
        adcResult = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
        
//        sprintf(cLcdLine, "%s: %4d \n",pcSensor,adcResult);
        
//        numBytesWritten = UART1_WriteBuffer(cLcdLine,bufLen);
        
        
        sprintf(cLcdLine, "%s: %d : %d\n",pcSensor,adcResult,contador);
        sprintf( cStringBuffer, cLcdLine );
		xQueueSend( xLCDQueue, &xMessage, portMAX_DELAY );
     
        vLedToggleLED(wichLED);
    }
}

void vTaskRTC( void *pvParameters ){
     // Seccion de inicializacion
//    uint8_t dataToWrite = 0;
//    uint8_t dataReceived = 0;
    rtc_init();
    
    TickType_t xDelayMs;
    auto rtcc_t t;
    
    xDelayMs = xMainMsToTicks(1000);
    

    // Cuerpo de la tarea
    for( ;; ){
        vTaskDelay(xDelayMs);
        
        get_rtcc_time(&t);
    }
}

void vTaskShell( void *pvParameters ){
    
    // Seccion de inicializacion
    uint8_t comando[MAX_COMMAND_LENGHT] ={0};
    string respuesta[MAX_RESP_LENGHT]={0};
    portBASE_TYPE bytesEnviados, bytesRecibidos;
    uint16_t sizeRespuesta;
    bytesEnviados = bytesRecibidos = -1;
    

    // Cuerpo de la tarea
    for( ;; ){
            
            
            //Detengo el TMR2 que inici� al final de _U1RXInterrupt().
            TMR2_Stop();
            //Leemos el comando recibido por la UART
            bytesRecibidos = UART1_ReadBuffer(comando,MAX_COMMAND_LENGHT);
            //Si se recibi� algo, procesamos el comando para obtener una respuesta
            if(bytesRecibidos>0) {
                bytesRecibidos=0;
                strcpy( respuesta, processCmdLine( &comando ) );
                //Env�o la respuesta por la UART hacia la PC.
//                stringToIntArray(respuestaUART,respuesta);
                sizeRespuesta = strlen(respuesta);
                bytesEnviados = UART1_WriteBuffer(respuesta,sizeRespuesta);
            }
            //Si se envi� la respuesta, se suspende la tarea hasta que llegue el pr�ximo comando.
            flushComando(comando); 
            vTaskSuspend( NULL );
    }
}

//static void vSenderTask( void *pvParameters ){
//    //PRODUCTOR DE DATOS
//    // Seccion de incializacion
//    long lValueToSend;
//    portBASE_TYPE xStatus;
//    lValueToSend = ( long ) pvParameters;
//    
//    // Cuerpo de la tarea
//    for( ;; ){
//        //Escribo 
//        xQueueSendToBack( xQueue, &lValueToSend, 0 );
//        taskYIELD(); // Cedo el resto de mi timeslice
//    }
//}


void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationIdleHook( void )
{
	/* Schedule the co-routines from within the idle task hook. */
	vCoRoutineSchedule();
}

//#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
TickType_t xMainMsToTicks( TickType_t xTimeInMs){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = ((uint32_t)xTimeInMs*(uint32_t)configTICK_RATE_HZ)/(uint32_t)1000;
    
    return (TickType_t) xTimeInTicks;
}

void flushComando(uint8_t *comando)
{
    int i;
    for (i = 0; i < MAX_COMMAND_LENGHT ; i++){
        comando[i] = 0;
    }
}



void ejemploI2C (void){
    #define MCHP24AA512_RETRY_MAX       100  // define the retry count
    #define MCHP24AA512_ADDRESS         0x50 // slave device address
    #define MCHP24AA512_DEVICE_TIMEOUT  50   // define slave timeout 


    uint8_t MCHP24AA512_Read(
                                    uint16_t address,
                                    uint8_t *pData,
                                    uint16_t nCount)
    {
        I2C1_MESSAGE_STATUS status;
        uint8_t     writeBuffer[3];
        uint16_t    retryTimeOut, slaveTimeOut;
        uint16_t    counter;
        uint8_t     *pD;

        pD = pData;

        for (counter = 0; counter < nCount; counter++)
        {

            // build the write buffer first
            // starting address of the EEPROM memory
            writeBuffer[0] = (address >> 8);                // high address
            writeBuffer[1] = (uint8_t)(address);            // low low address

            // Now it is possible that the slave device will be slow.
            // As a work around on these slaves, the application can
            // retry sending the transaction
            retryTimeOut = 0;
            slaveTimeOut = 0;

            while(status != I2C1_MESSAGE_FAIL)
            {
                // write one byte to EEPROM (2 is the count of bytes to write)
                I2C1_MasterWrite(    writeBuffer,
                                        2,
                                        MCHP24AA512_ADDRESS,
                                        &status);

                // wait for the message to be sent or status has changed.
                while(status == I2C1_MESSAGE_PENDING)
                {
                    // add some delay here

                    // timeout checking
                    // check for max retry and skip this byte
                    if (slaveTimeOut == MCHP24AA512_DEVICE_TIMEOUT)
                        return (0);
                    else
                        slaveTimeOut++;
                }

                if (status == I2C1_MESSAGE_COMPLETE)
                    break;

                // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                //               or I2C1_DATA_NO_ACK,
                // The device may be busy and needs more time for the last
                // write so we can retry writing the data, this is why we
                // use a while loop here

                // check for max retry and skip this byte
                if (retryTimeOut == MCHP24AA512_RETRY_MAX)
                    break;
                else
                    retryTimeOut++;
            }

            if (status == I2C1_MESSAGE_COMPLETE)
            {

                // this portion will read the byte from the memory location.
                retryTimeOut = 0;
                slaveTimeOut = 0;

                while(status != I2C1_MESSAGE_FAIL)
                {
                    // write one byte to EEPROM (2 is the count of bytes to write)
                    I2C1_MasterRead(     pD,
                                            1,
                                            MCHP24AA512_ADDRESS,
                                            &status);

                    // wait for the message to be sent or status has changed.
                    while(status == I2C1_MESSAGE_PENDING)
                    {
                        // add some delay here

                        // timeout checking
                        // check for max retry and skip this byte
                        if (slaveTimeOut == MCHP24AA512_DEVICE_TIMEOUT)
                            return (0);
                        else
                            slaveTimeOut++;
                    }

                    if (status == I2C1_MESSAGE_COMPLETE)
                        break;

                    // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                    //               or I2C1_DATA_NO_ACK,
                    // The device may be busy and needs more time for the last
                    // write so we can retry writing the data, this is why we
                    // use a while loop here

                    // check for max retry and skip this byte
                    if (retryTimeOut == MCHP24AA512_RETRY_MAX)
                        break;
                    else
                        retryTimeOut++;
                }
            }

            // exit if the last transaction failed
            if (status == I2C1_MESSAGE_FAIL)
            {
                return(0);
                break;
            }

            pD++;
            address++;

        }
        return(1);

    }
}