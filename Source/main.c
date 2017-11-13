/*
 * File:   main.c
 * Author: Tincho
 *
 * Created on 20 de septiembre de 2017, 18:56
 */


#include "xc.h"
#define FCY 8000000UL
#include <libpic30.h>

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
#include "i2c1.h"

#include "mcc.h"
#include "typedef.h"

#include "funciones/shell.h"
#include "funciones/eeprom.h"
#include "funciones/rtcc.h"

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

//Prototipo de tareas

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

void vTaskFunction( void *pvParameters );

void vTaskSensorADC( void *pvParameters );

void vTaskShell( void *pvParameters );

void vTaskI2C( void *pvParameters );

void vTaskSample( void *pvParameters );

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
TaskHandle_t xI2CTaskHandle;

static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

//uint8_t respuestaUART[MAX_RESP_LENGHT];

int main( void )
{
    SYSTEM_Initialize();
//    vLedInitialise();
    
    #define     MEMORY_ADDRESS          0x0000
    #define     BYTES_A_LEER            16
    #define     INITIAL_VALUE           9

    uint8_t     contador;

    uint16_t    address, i;
    uint8_t     readBuffer[BYTES_A_LEER], readByte, writeByte, readBuffer2[BYTES_A_LEER];
    uint8_t     respuesta1, respuesta2, respuesta3, respuesta4; 
    TickType_t  xDelayMs;
    
    rtcc_t time;
    rtcc_t time_readed;
    
    time.anio      = 0x17; //2017 en formato de dos d�gitos
    time.mes       = 0x11; //Noviembre
    time.dia       = 0x09; //09
    time.dow       = 0x05; //jueves (domingo=1)
    time.hora      = 0x18; //18hs
    time.minutos   = 0x28; //28min
    time.segundos  = 0x00; //CH bit(7) disable = clock running
    
    
    uint8_t         sourceData[16] = {  0xB0, 0xB1, 0xB2, 0xB3, 
                                        0xB4, 0xB5, 0xB6, 0xB7, 
                                        0xB8, 0xB9, 0xBB, 0xBB, 
                                        0xBC, 0xBD, 0xBE, 0xBF }; 
    
    address = MEMORY_ADDRESS;
    
    readByte = 0;
    writeByte = 76;
    
    respuesta1,respuesta2, respuesta3, respuesta4, contador = 0;
    
    for(i=0;i<BYTES_A_LEER;i++) readBuffer[i] = INITIAL_VALUE;
    
    for(i=0;i<BYTES_A_LEER;i++) readBuffer2[i] = INITIAL_VALUE;
    

//    respuesta1 = MCHP_24LCxxx_Init_I2C1(_24LC512_0);
    respuesta2 = MCHP_24LCxxx_Init_I2C1(_24LC512_1);
    __delay_ms(2);
    respuesta1 = MCHP_24LCxxx_Write_array(_24LC512_0,address, sourceData, BYTES_A_LEER);
//        respuesta2 = write_rtcc_array_2(0, sourceData, BYTES_A_LEER);
    
//    rtc_init();
//    set_rtcc_datetime(&time);
        

    
    while(1){
//Este delay sirve para ver bien la forma de onda en el osciloscopio
//        for(i=0;i<65000;i++);
        
//        __delay_ms(10);
        
        
//        respuesta2 = MCHP_24LCxxx_Write_byte(_24LC512_0,address+20,&writeByte);

        for (i=0;i<10000;i++);
        
//        respuesta3 = read_rtcc_array_2( 0, readBuffer, BYTES_A_LEER );
        get_rtcc_datetime(&time_readed);
        
        respuesta4 = MCHP_24LCxxx_Read_array(_24LC512_1, address, readBuffer,BYTES_A_LEER);
//        __delay_ms(2);
        respuesta3 = MCHP_24LCxxx_Read_array(_24LC512_0, address, readBuffer2,BYTES_A_LEER);
                
        contador++;
        
    }
    
//    xTaskCreate(    vTaskI2C,
//                    "I2C",
//                    DEFAULT_STACK_SIZE,
//                    NULL,
//                    3,
//                    &xI2CTaskHandle);
    
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
//	vTaskStartScheduler();

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

void vTaskI2C( void *pvParameters ){
// Seccion de inicializacion
//    uint8_t dataToWrite = 0;
//    uint8_t dataReceived = 0;
    #define     MEMORY_ADDRESS          0x0020
    #define     BYTES_A_LEER            5

    uint8_t     contador;

    uint16_t    address;
    uint8_t     *pData;
    uint8_t     readBuffer[BYTES_A_LEER] = { 0 };
    uint16_t    nCount;
    uint8_t     respuesta; 
    TickType_t  xDelayMs;
   
    address = MEMORY_ADDRESS;
    pData = readBuffer;
    nCount = BYTES_A_LEER;
    
    contador = 0;
    
    xDelayMs = xMainMsToTicks(1000);
    

    // Cuerpo de la tarea
    for( ;; ){
        vTaskDelay(xDelayMs);
        
        /*uint8_t MCHP24AA512_Read(
                                uint16_t address,
                                uint8_t *pData,
                                uint16_t nCount)*/
        
//        respuesta = MCHP_24LCxxx_Read_1(address, pData, nCount);
        
        contador++;
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
