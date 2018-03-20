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

#include "drivers/at_cmds.h"

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
//Funci�n que reemplaza la MACRO provista por freeRTOS para convertir tiempo
//en diferentes unidades a ticks
TickType_t xMsToTicks( TickType_t xTimeInMs);
TickType_t xSegToTicks( TickType_t xTimeInMs);
TickType_t xMinToTicks( TickType_t xTimeInMs);
//Handler de las funciones que se ejecutan cuando los respectivos software 
//timers expiran
static void prvPasiveCallback (TimerHandle_t xTimer);
static void prvActiveCallback (TimerHandle_t xTimer);
static void prvDataCallback (TimerHandle_t xTimer);
//Inicializaci�n software timers
static void softwareTimers_init();
//Funci�n que configura todos los sensores que intervienen el el proceso vTaskSample
static void  sensorsConfig();
//Funciones relativas al proceso vTaskSample
static void init_sample(muestra_t *muestra);
static void assembleSample(muestra_t *muestra);

uint8_t	gprsProcess( void );

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
uint8 sendCmd=TRUE;
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
    #define TASK_PERIOD_MS  500
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
    
    
    
    uint8_t i=0;
    
//    for(i=0;i<50;i++) 
    //Loop principal
    for(;;)
    {
        
 
     
        vLedToggleLED(2);
//              
//        //Espera arbitraria para dormir la tarea
        vTaskDelay(taskDelay);
        
//        for(i=0;i<TASK_PERIOD_MS;i++){}
//        

         
       
        
        i=gprsProcess();
        
        
   
            
        
//        if(i==200) i=0;

        
//        status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before exiting. */
//                                    portMAX_DELAY ); /* Block indefinitely. */
//        
//        switch(status){
//            case DATA_READY:
//                //Acciones cuando la respuesta est� lista y tiene la finalizaci�n esperada
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
//        //Se despertar� cuando la UART2 termine de recibir la respuesta del modem y notifique
//        //desde el la funci�n CALLBACK de TMR3. 
//        status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before exiting. */
//                                    portMAX_DELAY ); /* Block indefinitely. */
//
//        switch(status){
//            case DATA_READY:
//                //Acciones cuando la respuesta est� lista y tiene la finalizaci�n esperada
//                
//                receivedBytes = UART2_ReceiveBufferSizeGet(); //DEBUG
//                
//                receivedBytes = UART2_ReadBuffer(response,receivedBytes);
//                if(receivedBytes){
//                    //Analizar si hay que hacer alguna evaluaci�n de response
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
        
        //Si la mustra ya cerr�, hay que ensamblarla y almacenarla en la EEPROM
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
    //Potenci�metro - ADC
    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
}


void SetProcessState( uint8 * reg, uint8 state )
{
	*reg = state;
	sendCmd = TRUE;
}

void debugUART1(const char* s){
    UART1_WriteBuffer(s,strlen(s));
}

/*	enumeracion de los posibles estados en el manejo del gprs	*/
	static enum
	{
		noCommand = 0,				//!	no se ejecuta ningun comando
		gprsReset,					//!	reinicio del proceso GPRS
		initModem,					//!	inicializacion del modem
		getModemID,					//!	consulta identificacion de la revision del modem
		getMSN,						//!	consulta en numero de serie del fabricante
		getIMSI,					//!	consulta el numero de identificacion del abonado
		getIMEI,					//!	consulta el numero de identificacion serie del modem
		getSignal,					//!	consulta el nivel de se�al
		ipAddress,					//!	consulta la direccion de IP al modem
		setContext,					//!	configura el contexto
        activateContext,			//!	Activa el contexto previamente configurado
        configSocket,               //!	Configurar el socket
        configExtendSocket,         //!	Configuraci�n extendida el socket
        socketDial,                 //!	Apertura del socket
        socketDial_2,                
		socketStatus,               //!Nuevo estado para saber el estado del socket
		closeSocket,				//!	cierra el puerto de conexion con la red
		sendData,					//!	envia los datos almacenados usando el protocolo configurado
		putData,					//!	coloca los datos dentro del protocolo
        receiveData,				//Recibe datos desde el servido
		gprsWaitReset,				//!	estado para esperar el reinicio del equipo
        finalStateToggleLed,
	}gprsState = gprsReset, prev_gprsState = 0xFF;
    
#define     GPRS_BUFFER_SIZE    100
uint8_t	gprsProcess( void )
{

    char gprsBuffer [GPRS_BUFFER_SIZE]={0};
//    char gprsBuffer2 [GPRS_BUFFER_SIZE]={0};

	/*	estados del proceso	*/
	switch ( gprsState )
	{
	/*	GprsReset: deshabilitaci�n ECHO	*/
		case( gprsReset ):		
			if( sendCmd )
			{
                debugUART1("GprsReset:\r\n");
//				UART1_WriteBuffer("GprsReset",strlen("GprsReset"));
                UART2_WriteBuffer(atcmd_disableEcho,strlen(atcmd_disableEcho));
                //SendATCommand((string*)atcmd_disableEcho,gprsBuffer,gprsBuffer,10,0,2);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{                
                UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                debugUART1("gprsBuffer: ");
                debugUART1(gprsBuffer);                
				if(strstr(gprsBuffer,_OK_))
                {
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
                    SetProcessState( &gprsState,setContext );
                }
                else
                {
                   //strcat(gprsBuffer,"_ERROR_\n\r");
                   UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }			
			}
			break;
	
//         /*	Request Revision	*/
		case( setContext ):
			if( sendCmd )
			{
                debugUART1("setContext:\r\n");
//				UART1_WriteBuffer("setContext",strlen("setContext"));
                UART2_WriteBuffer(atcmd_setContextHARDCODED,strlen(atcmd_setContextHARDCODED));
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				//YA QUE LA RESPUESTA ES POR EJEMPLO<CR><LF>10.00.146<CR><LF>OK<CR><LF>
				//VERIFICAMOS SI EL OK ESTA EN EL STRING RECIBIDO
				UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                debugUART1("gprsBuffer: ");
                debugUART1(gprsBuffer); 
				if(strstr(gprsBuffer,_OK_))
                {
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
                    SetProcessState( &gprsState,activateContext );                 
                }
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }    
			}
			break;
			/*	Active Context	*/
		case( activateContext ):
			if( sendCmd )
			{
                debugUART1("activateContext:\r\n");
//                UART1_WriteBuffer("activateContext",strlen("activateContext"));
                UART2_WriteBuffer(atcmd_activateContextHARDCODED,strlen(atcmd_activateContextHARDCODED));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
                UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                debugUART1("gprsBuffer: ");
                debugUART1(gprsBuffer); 
				if(strstr(gprsBuffer,_OK_))
				{
                    /*COMO NOS DEVUELVE LA DIRECCI�N IP, DEBEMOS TOMARLA Y ALMACERNARLA*/			
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
                    SetProcessState( &gprsState,configSocket );
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }
			}
			break;
			
		/*	Configuraci�n de Socket	*/
		case( configSocket ):
			if( sendCmd )
			{
                debugUART1("configSocket:\r\n");
//				UART1_WriteBuffer("configSocket",strlen("configSocket"));
                UART2_WriteBuffer(atcmd_configSocketHARDCODED,strlen(atcmd_configSocketHARDCODED));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
                      
				UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                debugUART1("gprsBuffer: ");
                debugUART1(gprsBuffer); 
				if(strstr(gprsBuffer,_OK_))
				{
					 /*COMO NOS DEVUELVE LA DIRECCI�N IP, DEBEMOS TOMARLA Y ALMACERNARLA*/			
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
					SetProcessState( &gprsState, configExtendSocket);
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }
			}
			break;
            
			/*	Configuraci�n de Socket	*/
		case( configExtendSocket ):
			if( sendCmd )
			{	
                debugUART1("configExtendSocket:\r\n");
//				UART1_WriteBuffer("configExtendSocket",strlen("configExtendSocket"));
                UART2_WriteBuffer(atcmd_configExtendSocketHARDCODED,strlen(atcmd_configExtendSocketHARDCODED));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
				if(strstr(gprsBuffer,_OK_))
				{
					 /*COMO NOS DEVUELVE LA DIRECCI�N IP, DEBEMOS TOMARLA Y ALMACERNARLA*/			
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
					SetProcessState( &gprsState, socketDial);
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }
			}
			break;  
			/*	Dial Socket	*/
		case( socketDial ):
			if( sendCmd )
			{	
                debugUART1("socketDial:\r\n");
//                UART1_WriteBuffer("socketDial",strlen("socketDial"));
                UART2_WriteBuffer(atcmd_socketDialHARDCODED_1,strlen(atcmd_socketDialHARDCODED_1));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{           
				UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
				if(strstr(gprsBuffer,_OK_))
				{
					 /*COMO NOS DEVUELVE LA DIRECCI�N IP, DEBEMOS TOMARLA Y ALMACERNARLA*/			
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
					SetProcessState( &gprsState, socketDial_2);
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }			
			}
			break;
        case( socketDial_2 ):
			if( sendCmd )
			{	
                debugUART1("socketDial_2:\r\n");
//                UART1_WriteBuffer("socketDial_2",strlen("socketDial_2"));
                UART2_WriteBuffer(atcmd_socketDialHARDCODED_2,strlen(atcmd_socketDialHARDCODED_2));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{   
                UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
				if(strstr(gprsBuffer,">"))
				{		
                    UART1_WriteBuffer("> Recibido\r\n",strlen("> Recibido\r\n"));
					SetProcessState( &gprsState, putData);
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }			
			}
			break;			
				/*	Add Data to Socket	
			 * Pide la proxima trama que se tiene que enviar el servidor. Estas pueden ser 
			 * registro, configuracion o muestras. En caso de ser muestras, cuando no haya 
			 * mas para enviar la funcion getServerFrame() devuelve un NULL.
			 */
		case( putData ):
			if( sendCmd )
			{
                debugUART1("putData:\r\n");

                UART2_WriteBuffer(atcmd_FRAME,strlen(atcmd_FRAME));			
				UART2_WriteBuffer(atcmd_EOF,strlen(atcmd_EOF)); //Fin de trama
                
                debugUART1("Trama: ");
                debugUART1(atcmd_FRAME);
                debugUART1("\r\n");

				sendCmd = FALSE;
			}
			else
			{		
                UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
				if(strstr(gprsBuffer,"SRING"))
                {		
                    UART1_WriteBuffer("SRING Recibido\r\n",strlen("SRING Recibido\r\n"));
					SetProcessState( &gprsState, receiveData);
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }
			}
			break;		
			/*	Receive data  */
		case( receiveData ):
			if( sendCmd )
			{
                debugUART1("receiveData:\r\n");
//                UART1_WriteBuffer("receiveData",strlen("receiveData"));
                UART2_WriteBuffer(atcmd_sListenHARDCODED,strlen(atcmd_sListenHARDCODED));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{	
                UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
				if(strstr(gprsBuffer,"024F"))
                {		
                    debugUART1("Rta SERVER: ");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                    debugUART1("\r\n");
                    SetProcessState( &gprsState, closeSocket);
                }
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }
            }
            break;
			
        /*	Close a Socket	
         * Cierra el puerto de comunicacion con el servidor.
         */
		case( closeSocket ):
            if( sendCmd )
			{	
                debugUART1("closeSocket:\r\n");
//                UART1_WriteBuffer("closeSocket",strlen("closeSocket"));
                UART2_WriteBuffer(atcmd_closeSocketHARDCODED,strlen(atcmd_closeSocketHARDCODED));			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{           
				UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
				if(strstr(gprsBuffer,_OK_))
//                if(strstr(UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE),_OK_))
				{
					 /*COMO NOS DEVUELVE LA DIRECCI�N IP, DEBEMOS TOMARLA Y ALMACERNARLA*/			
                    UART1_WriteBuffer("_OK_\n\r",strlen("_OK_\n\r"));
					SetProcessState( &gprsState, configExtendSocket);
				}
                else
                {
                    //strcat(gprsBuffer,"_ERROR_\n\r");
                    UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                }			
			}
			break;
        
        case(finalStateToggleLed):
            debugUART1("finalStateToggleLed:\r\n");
            vLedToggleLED(1);
			break;
    }
    return	FALSE;
}

