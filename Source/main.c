/*
 * File:   main.c
 * Author: Tincho
 *
 * Created on 20 de septiembre de 2017, 18:56
 */


#include "xc.h"
#define FCY 16000000UL
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


/* Application includes. */
#include "perifericos/lcd.h"
#include "perifericos/adc.h"
#include "perifericos/led.h"
#include "tmr2.h"
#include "uart1.h"
#include "i2c1.h"
#include "procesos/modem.h"

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

#define MAX_PRIORITY    configMAX_PRIORITIES-1



#define mainMAX_STRING_LENGTH				( 20 )
#define bufLen                              ( 15 )
#define DEFAULT_STACK_SIZE                  (1000)  

//Software TIMERS
#define T_MUESTREO_PASIVO_S               6
#define T_MUESTREO_ACTIVO_S               3
#define T_MUESTREO_DATO_S                 1


//***********************Prototipo de tareas************************************

void vTaskShell( void *pvParameters );

void vTaskSample( void *pvParameters );

void vTaskModem( void *pvParameters ); 

void vTaskTest( void *pvParameters ); 

//***********************Prototipo de funciones externas************************
//The vTaskDelay() API function prototype
void vTaskDelay( TickType_t xTicksToDelay ); 

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

//***********************Prototipo de funciones propias*************************
//Función que reemplaza la MACRO provista por freeRTOS para convertir tiempo
//en diferentes unidades a ticks
TickType_t xMsToTicks( TickType_t xTimeInMs);
TickType_t xSegToTicks( TickType_t xTimeInMs);
TickType_t xMinToTicks( TickType_t xTimeInMs);
//Handler de las funciones que se ejecutan cuando los respectivos software 
//timers expiran
static void prvPasiveCallback (TimerHandle_t xTimer);
static void prvActiveCallback (TimerHandle_t xTimer);
static void prvDataCallback (TimerHandle_t xTimer);
//Inicialización software timers
static void softwareTimers_init();
//Función que configura todos los sensores que intervienen el el proceso vTaskSample
static void  sensorsConfig();
//Funciones relativas al proceso vTaskSample
static void init_sample(muestra_t *muestra);
static void assembleSample(muestra_t *muestra);

//******************************Globales****************************************

//The queue used to send messages to the LCD task.
static QueueHandle_t xLCDQueue;

QueueHandle_t   xModemRequests;
QueueHandle_t   xModemResponses;

//Handlers tareas
TaskHandle_t xShellHandle;
TaskHandle_t xSampleHandle;
TaskHandle_t xModemHandle;
TaskHandle_t xTestHandle;
//Handlers software timers
TimerHandle_t xSamplePasive;
TimerHandle_t xSampleActive;
TimerHandle_t xSampleData;
//Tiempos usados en software timers
TickType_t  xTimePasive, xTimeActive, xTimeData;

static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

uint8_t flagDataUartReady = 0;

int main( void )
{
    SYSTEM_Initialize();
//    sensorsConfig();
//    rtc_init();
//    vLedInitialise();
//    softwareTimers_init();
    
    
//    xTaskCreate(    vTaskModem,
//                    "vTaskModem",
//                    2000,
//                    NULL,
//                    MAX_PRIORITY,
//                    &xModemHandle);
    
    xTaskCreate(    vTaskTest,
                    "vTaskTest",
                    2000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xTestHandle);
        
//    xTaskCreate(    vTaskSample,
//                    "vTaskSample",
//                    1000,
//                    NULL,
//                    MAX_PRIORITY,
//                    &xSampleHandle);
//    
//    xTaskCreate(    vTaskShell,
//                    "Shell",
//                    2000,
//                    NULL,
//                    2,
//                    &xShellHandle);
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
/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest( void *pvParameters )
{
    #define TASK_PERIOD_MS  2000
    #define DELAY 0;
    #define END_OF_FRAMES "\r\nOK\r\n";
    #define N_FRAMES 1;
    #define SENDER SMS;
    #define TIMEOUT 100;
    
    //Variables
//    uint32_t status;
    uint16_t receivedBytes, sendedBytes, bufferSizeGet;
    TickType_t taskDelay;
//    BaseType_t response;
//    cmdQueue_t request;
    char command[10] = "AT+GMR\r\n";
//    
    char respuestaModem[50]= "";
    
//    char *pc = command;
//    
//    request.cmd = pc;
//    request.delay = DELAY;
//    request.expextedEndOfFrame = OK; //Despues matchearlo con END_OF_FRAMES
//    request.nFrames = 1;
//    request.sender = SMS; //Despues matchear con SENDER
//    request.timeout = TIMEOUT;
    
    receivedBytes, sendedBytes, bufferSizeGet = 0;
    
//    uint8_t cmdLenght = 0;
    
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
    
    
    
    int i=0;
    
//    for(i=0;i<50;i++) 
    //Loop principal
    for(;;)
    {
        
 
        //Envío comando al MODEM
        sendedBytes = UART2_WriteBuffer(command,8);
        
        vLedToggleLED(2);
//              
//        //Espera arbitraria para dormir la tarea
        vTaskDelay(taskDelay);
        
//        for(i=0;i<TASK_PERIOD_MS;i++){}
//        

         
        receivedBytes = UART2_ReadBuffer(respuestaModem, 50);
        
        
        i++;
        i--;
        
   
            
        
//        if(i==200) i=0;

        
//        status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before exiting. */
//                                    portMAX_DELAY ); /* Block indefinitely. */
//        
//        switch(status){
//            case DATA_READY:
//                //Acciones cuando la respuesta está lista y tiene la finalización esperada
//                
//                bufferSizeGet = UART1_ReceiveBufferSizeGet();
//        
//                receivedBytes = UART1_ReadBuffer(respuestaModem,8);
//        
//                cmdLenght = strlen(request.cmd);
//                
//                
//                break;
//            case DATA_ERROR:
//                //Acciones cuando NO recibo la respuesta esperada.
//                //Debo avisarle al remitente que hubo un error
//                break;
//        }
        
        
    }
}

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se encarga de arbitrar los pedidos de SMS_PROCESS y GRPS_PROCESS hacia el Modem.
 * Posee una cola de recepci?n de mensajes hacia el modem y otra de recepci?n de respuestas desde el modem.
 * Se le da prioridad a los mensajes de GRPS_PROCESS sobre los mensajes de SMS_PROCESS.
 * @return	void
 */
void vTaskModem( void *pvParameters )
{
    uint32_t status;
    uint16_t receivedBytes, sendedBytes;
    cmdQueue_t request;
    uint8_t response[MODEM_BUFFER_SIZE];
    
    
    xModemRequests   = xQueueCreate(REQUEST_QUEUE_SIZE, sizeof(cmdQueue_t));
    //La cola de respuesta solo contiene el string proveniente del modem
    xModemResponses  = xQueueCreate(RESPONSE_QUEUE_SIZE, MODEM_BUFFER_SIZE);
    
    uint16_t contadorSMS, contadorGPRS, contadorSHELL = 0;
    
    uint8_t cmdLenght = 0;
    
    receivedBytes, sendedBytes = 0;
    
    for(;;)
    {
        //Si la cola fue creada correctamente
        if(xModemRequests != NULL){
            /*Si no hay elementos en la cola xModemRequests, la tarea se bloquea 
            esperando la llegada de comandos*/
            if(xQueueReceive( xModemRequests, &( request ), portMAX_DELAY)){
                //Demora en el envio del comando
                if(request.delay>0) vTaskDelay(xSegToTicks(request.delay));
                //Envio del comando a la UART2 (modem)
                //expexted end of frame
                cmdLenght = strlen(request.cmd);
//                sendedBytes = UART2_WriteBuffer(request.cmd, cmdLenght ,request.expextedEndOfFrame);
            }       
        }
//        //Se despertará cuando la UART2 termine de recibir la respuesta del modem y notifique
//        //desde el la función CALLBACK de TMR3. 
//        status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before exiting. */
//                                    portMAX_DELAY ); /* Block indefinitely. */
//
//        switch(status){
//            case DATA_READY:
//                //Acciones cuando la respuesta está lista y tiene la finalización esperada
//                
//                receivedBytes = UART2_ReceiveBufferSizeGet(); //DEBUG
//                
//                receivedBytes = UART2_ReadBuffer(response,receivedBytes);
//                if(receivedBytes){
//                    //Analizar si hay que hacer alguna evaluación de response
//                    
//                    switch (request.sender){
//                    case GPRS:
//                        contadorGPRS++;
////                        if(!xQueueSend(xGPRS_Response,response,0)){
////                            /*Realizar alguna acci?n si no se pudo enviar una 
////                             * respuesta porque la cola estaba llena*/
////                        }
//                        break;
//                    case SMS:
//                        contadorSMS++;
////                        if(!xQueueSend(xSMS_Response,response,0)){
////                            /*Realizar alguna acci?n si no se pudo enviar una 
////                             * respuesta porque la cola estaba llena*/
////                        }
//                        break;
//                    case SHELL:
//                        contadorSHELL++;
////                        if(!xQueueSend(xSHELL_Response,response,0)){
////                            /*Realizar alguna acci?n si no se pudo enviar una 
////                             * respuesta porque la cola estaba llena*/
////                        }
//                            break;
//                    }
//                }
//                break;
//            case DATA_ERROR:
//                //Acciones cuando NO recibo la respuesta esperada.
//                //Debo avisarle al remitente que hubo un error
//                break;
//
//        }
    }
}

void vTaskSample( void *pvParameters ){
    UBaseType_t uxHighWaterMarkSample;
// Seccion de inicializacion
    uint32_t status;
    xTimerStart(xSamplePasive,0);
    BaseType_t samplingFinished = pdFALSE;
    muestra_t sample, sampleReturned;
    
    uint16_t sensor_1;
    uint16_t sensor_2;
    
    uint8_t returnPutSample,returnGetSample;
    uint16_t ptrEscritura, ptrLectura, totalSamples;
    
    uint16_t readed, writed;
    
    bool firstGet = true;
    
    
    uint8_t arraySample[sizeof(muestra_t)], arraySampleReturned[sizeof(muestra_t)];
    int i;
    for(i=0;i<sizeof(muestra_t);i++){
        arraySample[i] = 0;
        arraySampleReturned[i] = 0;
    }
    
    totalSamples = 0;
    
    init_sample(&sample);
    init_sample(&sampleReturned);
    
//    ptrLectura = 1;
    resetSamplesPtr();
    
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
                samplingFinished = pdTRUE; //colocar en el case correspondiente
                break;
            case SENSOR_2:
                totalSamples = getSamplesTotal();   
                if(firstGet || !totalSamples){ //Primera vez o no hay muestras nuevas
                    returnGetSample = getSample(&sampleReturned,lastSample);
                    firstGet = false;
                }
                else{
                    returnGetSample = getSample(&sampleReturned,nextSample);
                }
                break;
                
            case CLOSE_SAMPLE:
               samplingFinished = pdTRUE; 
        }
        
        //Si la mustra ya cerró, hay que ensamblarla y almacenarla en la EEPROM
        if(samplingFinished){
           assembleSample(&sample);
           returnPutSample = putSample(&sample);          
           samplingFinished = pdFALSE;
        }
        
        uxHighWaterMarkSample = uxTaskGetStackHighWaterMark( NULL );
    }
}

void vTaskShell( void *pvParameters ){
    UBaseType_t uxHighWaterMarkShell;
    // Seccion de inicializacion
    uint8_t comando[MAX_COMMAND_LENGHT] ={0};
    string respuesta[MAX_RESP_LENGHT]={0};
    portBASE_TYPE bytesEnviados, bytesRecibidos;
    uint16_t sizeRespuesta;
    bytesEnviados = bytesRecibidos = -1;
    
    // Cuerpo de la tarea
    for( ;; ){   
            uxHighWaterMarkShell = uxTaskGetStackHighWaterMark( NULL );
            //Detengo el TMR2 que inició al final de _U1RXInterrupt().
            TMR2_Stop();
            //Leemos el comando recibido por la UART
            bytesRecibidos = UART1_ReadBuffer(comando,MAX_COMMAND_LENGHT);
            //Si se recibió algo, procesamos el comando para obtener una respuesta
            if(bytesRecibidos>0) {
                bytesRecibidos=0;
                strcpy( respuesta, processCmdLine( &comando ) );
                //Envío la respuesta por la UART hacia la PC.
//                stringToIntArray(respuestaUART,respuesta);
                sizeRespuesta = strlen(respuesta);
                bytesEnviados = UART1_WriteBuffer(respuesta,sizeRespuesta);
            }
            //Si se envió la respuesta, se suspende la tarea hasta que llegue el próximo comando.
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

static void softwareTimers_init(){
    
    xTimePasive = xSegToTicks(T_MUESTREO_PASIVO_S);
    xTimeActive = xSegToTicks(T_MUESTREO_ACTIVO_S);
    xTimeData   = xSegToTicks(T_MUESTREO_DATO_S);
    
    xSamplePasive   = xTimerCreate("pasivePeriod",xTimePasive,pdTRUE,0,prvPasiveCallback);
    xSampleActive   = xTimerCreate("activePeriod",xTimeActive,pdTRUE,0,prvActiveCallback);
    xSampleData     = xTimerCreate("dataPeriod",xTimeData,pdTRUE,0,prvDataCallback) ;
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
    //Potenciómetro - ADC
    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
}