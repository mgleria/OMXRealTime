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
#include "procesos/modem.h"
#include "drivers/at_cmds.h"

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




//***********************Prototipo de tareas************************************

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

void vTaskShell( void *pvParameters );

void vTaskModem( void *pvParameters ); 

#include "tareas/testTask.h"
#include "tareas/sampleTask.h"

//***********************Prototipo de funciones externas************************
//The vTaskDelay() API function prototype
void vTaskDelay( TickType_t xTicksToDelay ); 

//***********************Prototipo de funciones propias*************************
//Funci�n que reemplaza la MACRO provista por freeRTOS para convertir tiempo
//en diferentes unidades a ticks

//Handler de las funciones que se ejecutan cuando los respectivos software 
//timers expiran
//static void prvPasiveCallback (TimerHandle_t xTimer);
//static void prvActiveCallback (TimerHandle_t xTimer);
//static void prvDataCallback (TimerHandle_t xTimer);
//static void prvAntireboteCallback (TimerHandle_t xTimer);
//Inicializaci�n software timers
//static void softwareTimers_init();
//Funci�n que configura todos los sensores que intervienen el el proceso vTaskSample
//static void  sensorsConfig();
//Funciones relativas al proceso vTaskSample
//static void init_sample(muestra_t *muestra);
//static void assembleSample(muestra_t *muestra);
//MOVIENDO LAS FUNCIONES PROPIAS DE LAS TAREAS A ARCHIVOS SEPARADOS.
////////////////////////////



uint8_t	gprsProcess( char *grpsDATA );




//******************************Globales****************************************

//The queue used to send messages to the LCD task.
static QueueHandle_t xLCDQueue;

QueueHandle_t   xModemRequests;
QueueHandle_t   xModemResponses;

//Handlers tareas
TaskHandle_t xShellHandle;
TaskHandle_t xModemHandle;

uint8 sendCmd=TRUE;
static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

uint8_t flagDataUartReady;

//estacion_t estacion;

int main( void )
{
    SYSTEM_Initialize();
    sensorsConfig();
//    rtc_init();
//    vLedInitialise();
    
    startSampleTask();
//    startTestTask();
    
//    vTraceEnable(TRC_START);
    
//    xTaskCreate(    vTaskModem,
//                    "vTaskModem",
//                    2000,
//                    NULL,
//                    MAX_PRIORITY,
//                    &xModemHandle);
        
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
        
        //gprsProcess();
        
        
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
//        //desde la funci�n CALLBACK de TMR4. 
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
//                break;//                break;

//            case DATA_ERROR:
//                //Acciones cuando NO recibo la respuesta esperada.
//                //Debo avisarle al remitente que hubo un error
//                break;
//
//        }
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

void flushComando(uint8_t *comando)
{
    int i;
    for (i = 0; i < MAX_COMMAND_LENGHT ; i++){
        comando[i] = 0;
    }
}



//static void assembleSample(muestra_t *muestra)
//{
//    
////    rtcc_t rtcc;
////    
////    get_rtcc_datetime(&rtcc);
////    
////    muestra->anio = rtcc.anio;
////    muestra->mes = rtcc.mes;
////    muestra->dia = rtcc.dia;
////    muestra->hora = rtcc.hora;
////    muestra->minutos = rtcc.minutos;
//    
//    muestra->clima.luzDia = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
//   
//}

void  sensorsConfig(){
    //Potenci�metro - ADC
    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
    ADC_ChannelEnable ( ADC_CHANNEL_TEMPERATURE_SENSOR );
       
}


void SetProcessState( uint8 * reg, uint8 state )
{
	*reg = state;
	sendCmd = TRUE;
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
    
#define     GPRS_BUFFER_SIZE    120 //Duplicado ojo!
uint8_t	gprsProcess( char *grpsDATA )
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
                UART2_WriteBuffer(atcmd_setContextPersonal,strlen(atcmd_setContextPersonal));
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

                UART2_WriteBuffer(grpsDATA,strlen(grpsDATA)); //FRAME pasado por parametro
                
//                UART2_WriteBuffer(atcmd_FRAME2,strlen(atcmd_FRAME2));
                
				UART2_WriteBuffer(atcmd_EOF,strlen(atcmd_EOF)); //Fin de trama
                
                debugUART1("Trama: ");
                debugUART1(grpsDATA);
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
                    UART1_WriteBuffer("WAITING SRING:  ",strlen("WAITING SRING:  "));
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
                debugUART1("Rta SERVER: ");
                UART1_WriteBuffer(gprsBuffer,strlen(gprsBuffer));
                debugUART1("\r\n");
                
				if(strstr(gprsBuffer,"024F"))
                {		
                    debugUART1("Rta SERVER: 024F\r\n");
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
    return	TRUE;
}