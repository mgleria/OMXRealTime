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

///*FreeRTOS Include*/
//#include "semphr.h" 


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
#include "funciones/memory.h"
#include "funciones/sampling.h"
#include "timers.h"

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

//Software TIMERS
#define T_MUESTREO_ACTIVO_MIN               1
#define T_MUESTREO_PASIVO_MIN               9
#define T_MUESTREO_DATO_SEG                 1


//***********************Prototipo de tareas************************************

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

void vTaskFunction( void *pvParameters );

void vTaskSensorADC( void *pvParameters );

void vTaskShell( void *pvParameters );

void vTaskI2C( void *pvParameters );

void vTaskSample( void *pvParameters );


//***********************Prototipo de funciones*********************************

//static void vReceiverTask( void *pvParameters );

//static void vSenderTask( void *pvParameters );
//The vTaskDelay() API function prototype
void vTaskDelay( TickType_t xTicksToDelay ); 
//Funci�n que reemplaza la MACRO provista por freeRTOS para convertir ms a ticks
TickType_t xMsToTicks( TickType_t xTimeInMs);
TickType_t xSegToTicks( TickType_t xTimeInMs);
TickType_t xMinToTicks( TickType_t xTimeInMs);

static void prvPasiveCallback (TimerHandle_t xTimer);
static void prvActiveCallback (TimerHandle_t xTimer);
static void prvDataCallback (TimerHandle_t xTimer);

static void  sensorsConfig();

static void init_sample(muestra_t *muestra);
static void assembleSample(muestra_t *muestra);
//******************************Globales****************************************

//The queue used to send messages to the LCD task.
static QueueHandle_t xLCDQueue;

//Cola de respuesta generada a los comandos procesados por vTaskShell
//QueueHandle_t xQueueShell;
//xQueueShell = xQueueCreate(1, MAX_RESP_LENGHT);
 
TaskHandle_t xShellHandle;
TaskHandle_t xSampleHandle;

TimerHandle_t xSamplePasive;
TimerHandle_t xSampleActive;
TimerHandle_t xSampleData;

TickType_t  xTimePasive, xTimeActive, xTimeData;

static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

//uint8_t respuestaUART[MAX_RESP_LENGHT];

//SemaphoreHandle_t mutexSample;

int main( void )
{
    SYSTEM_Initialize();
    sensorsConfig();
//    vLedInitialise();
    
    
//    xTimePasive = xMinToTicks(1);
//    xTimeActive = xSegToTicks(10);
//    xTimeData   = xSegToTicks(1);
    xTimePasive = xSegToTicks(6);
    xTimeActive = xSegToTicks(3);
    xTimeData   = xSegToTicks(1);
    
//    TimerHandle_t xTimerCreate(     const char * const pcTimerName,
//                                    TickType_t xTimerPeriodInTicks,
//                                    UBaseType_t uxAutoReload,
//                                    void * pvTimerID,
//                                    TimerCallbackFunction_t pxCallbackFunction );
    
    xSamplePasive   = xTimerCreate("pasivePeriod",xTimePasive,pdTRUE,0,prvPasiveCallback);
    xSampleActive   = xTimerCreate("activePeriod",xTimeActive,pdTRUE,0,prvActiveCallback);
    xSampleData     = xTimerCreate("dataPeriod",xTimeData,pdTRUE,0,prvDataCallback) ;
        
    xTaskCreate(    vTaskSample,
                    "vTaskSample",
                    1000,
                    NULL,
                    configMAX_PRIORITIES-1,
                    &xSampleHandle);
    
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
//    
//    
//    /* Start the task that will control the LCD.  This returns the handle
//	to the queue used to write text out to the task. */
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
    
    xDelayMs = xMsToTicks(1000);

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

void vTaskSample( void *pvParameters ){
// Seccion de inicializacion
    uint32_t status;
    xTimerStart(xSamplePasive,0);
    BaseType_t samplingFinished = pdFALSE;
    muestra_t sample, sampleReturned;
    
    uint16_t sensor_1;
    uint16_t sensor_2;
    
    uint8_t returnPutSample,returnGetSample;
    uint16_t ptrEscritura, ptrLectura;
    
    uint16_t readed, writed;
    
    uint8_t arraySample[sizeof(muestra_t)], arraySampleReturned[sizeof(muestra_t)];
    int i;
    for(i=0;i<sizeof(muestra_t);i++){
        arraySample[i] = 0;
        arraySampleReturned[i] = 0;
    }
    
    
    init_sample(&sample);
    init_sample(&sampleReturned);
    
//    ptrLectura = 1;
//    resetSamplesPtr();
    
    // Cuerpo de la tarea
    for( ;; ){
        status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before
                                                exiting. */
                                    portMAX_DELAY ); /* Block indefinitely. */
        //Se adquieren los valores y se guardan en 'sample'.
        switch(status){
            case SENSOR_1:
                sensor_1 = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
                sample.bateria = sensor_1;
//                writed = sensor_1;
//                arraySample[63] = sensor_1 >> 8;
//                arraySample[64] = sensor_1 ;
//                arraySample[0] = 0xFF;
                samplingFinished = pdTRUE; //colocar en el case correspondiente
                break;
            case SENSOR_2:
           
//                setSamplesRead(ptrLectura);
                returnGetSample = getSample(&sampleReturned,nextSample);
//                returnGetSample = MCHP_24LCxxx_Read_array(_24LC512_0, ptrLectura,arraySampleReturned,sizeof(muestra_t));
                sensor_2 = 0;
//                sensor_2 = getData_Sensor_2();
                break;
            case CLOSE_SAMPLE:
               samplingFinished = pdTRUE; 
        }
        //Si la mustra ya cerr�, hay que ensamblarla y almacenarla en la EEPROM
        if(samplingFinished){
           assembleSample(&sample);
//           ptrEscritura = 0x0025;
//           setSamplesWrite(ptrEscritura);
           returnPutSample = putSample(&sample);
            
//           arraySample[25] = 89;
//           arraySample[sizeof(muestra_t)-1] = 11;
//           returnPutSample = MCHP_24LCxxx_Write_array(_24LC512_0,ptrLectura,arraySample,sizeof(muestra_t));
           samplingFinished = pdFALSE;
        }
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
TickType_t xMsToTicks( TickType_t xTimeInMs){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = ((uint32_t)xTimeInMs*(uint32_t)configTICK_RATE_HZ)/(uint32_t)1000;
    
    return (TickType_t) xTimeInTicks;
}

TickType_t xSegToTicks( TickType_t xTimeInSeg){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = xMsToTicks(xTimeInSeg*1000);
    
    return (TickType_t) xTimeInTicks;
}

TickType_t xMinToTicks( TickType_t xTimeInMin){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = xSegToTicks(xTimeInMin*60);
    
    return (TickType_t) xTimeInTicks;
}

void flushComando(uint8_t *comando)
{
    int i;
    for (i = 0; i < MAX_COMMAND_LENGHT ; i++){
        comando[i] = 0;
    }
}

static void init_sample(muestra_t *muestra)
{
//    muestra->_reserved_ = {0,0,0,0,0,0};
    muestra->anio=0x00;
    muestra->bateria=0x0000;
    muestra->clima.hum = 0x0000;
    muestra->clima.humHoja = 0x0000;
    muestra->clima.humSuelo1 = 0x0000;
    muestra->clima.humSuelo2 = 0x0000;
    muestra->clima.humSuelo3 = 0x0000;
    muestra->clima.lluvia = 0x0000;
    muestra->clima.luzDia = 0x0000;
    muestra->clima.presion = 0;
    muestra->clima.radiacionSolar = 0x0000;
    muestra->clima.tempHoja = 0x0000;
    muestra->clima.tempSuelo1 = 0x0000;
    muestra->clima.tempSuelo2 = 0x0000;
    muestra->clima.tempSuelo3 = 0x0000;
    muestra->clima.temper = 0x0000;
    muestra->clima.viento.direccionM = 0x0000;
    muestra->clima.viento.direccionP = 0x00;
    muestra->clima.viento.velocidadM = 0x0000;
    muestra->clima.viento.velocidadP = 0x0000;
    muestra->cmd=0x00;
    muestra->corriente1=0x0000;
    muestra->corriente2=0x0000;
    muestra->dia=0x00;
    muestra->mes=0x00;
    muestra->hora=0x00;
    muestra->minutos=0x00;
    muestra->nullE = NULL;
    muestra->num_serie=0x0000;
    muestra->periodo=0x00;
    muestra->senial =0x00;
    muestra->sensorHab1=0x00;
    muestra->sensorHab2=0x00;
    muestra->sensorHab3=0x00;
    muestra->tipo=0x00;
      
}

static void assembleSample(muestra_t *muestra)
{
    rtcc_t rtcc;
    
    get_rtcc_datetime(&rtcc);
    
    muestra->anio = rtcc.anio;
    muestra->mes = rtcc.mes;
    muestra->dia = rtcc.dia;
    muestra->hora = rtcc.hora;
    muestra->minutos = rtcc.minutos;
   
}

static void prvPasiveCallback (TimerHandle_t xTimer){
    
    vLedToggleLED(0);
    xTimerStop(xSamplePasive,0);
    xTimerStart(xSampleActive,0);
    xTimerStart(xSampleData,0);
    xTaskNotify(xSampleHandle,SENSOR_1,eSetValueWithOverwrite);
}

static void prvActiveCallback (TimerHandle_t xTimer){
    vLedToggleLED(1);
    
    xTimerStop(xSampleData,0);
    xTimerStop(xSampleActive,0);
    xTimerStart(xSamplePasive,0);
    
    xTaskNotify(xSampleHandle,SENSOR_2,eSetValueWithOverwrite);
}

static void prvDataCallback (TimerHandle_t xTimer){
    vLedToggleLED(2);
}

static void  sensorsConfig(){
    //Potenci�metro - ADC
    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
}