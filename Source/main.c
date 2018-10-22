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

/*EZBL Include*/
#include "ezbl.h"


/* Demo application includes. */
#include "perifericos/lcd.h"
#include "perifericos/adc.h"
#include "perifericos/led.h"
#include "tmr2.h"
//#include "uart1.h"
#include "i2c1.h"

#include "mcc.h"
#include "typedef.h"

#include "funciones/shell.h"
#include "funciones/eeprom.h"
#include "funciones/rtcc.h"
#include "funciones/memory.h"
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
#include "tareas/gprsTask.h"

//***********************Prototipo de funciones externas************************
//The vTaskDelay() API function prototype
void vTaskDelay( TickType_t xTicksToDelay ); 

//***********************Prototipo de funciones propias*************************
void setEstacionConfig();
void setDeviceConfig();

//******************************Globales****************************************

//The queue used to send messages to the LCD task.
static QueueHandle_t xLCDQueue;

QueueHandle_t   xModemRequests;
QueueHandle_t   xModemResponses;

//Handlers tareas
TaskHandle_t xShellHandle;
TaskHandle_t xModemHandle;

//uint8 sendCmd=TRUE;
static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

/*	Estructura con informacin del equipo	*/
estacion_t estacion;
configDevice_t configDevice;

/*	fecha y hora del sistema. Definida en ext_rtcc.c	*/
extern rtcc_t tiempo;

/*Macro para resetear los punteros de memoria en cada inicio de la app*/
#define     RESET_MEMORY     1

int main( void )
{
    unsigned int ret=0;
    SYSTEM_Initialize();
    
    EZBL_BootloaderInit();
    
//            ClrWdt();

        
    
    
    rtc_init();
////    vLedInitialise();
    setEstacionConfig();
    
    if(RESET_MEMORY) resetSamplesPtr();
    
//    printMemoryPointers();
    
    startSampleTask();
    startGprsTask();
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
//void vTaskModem( void *pvParameters )
//{
//    uint32_t status;
//    uint16_t receivedBytes, sendedBytes;
//    cmdQueue_t request;
//    uint8_t response[MODEM_BUFFER_SIZE];
//    
//    
//    xModemRequests   = xQueueCreate(REQUEST_QUEUE_SIZE, sizeof(cmdQueue_t));
//    //La cola de respuesta solo contiene el string proveniente del modem
//    xModemResponses  = xQueueCreate(RESPONSE_QUEUE_SIZE, MODEM_BUFFER_SIZE);
//    
//    uint16_t contadorSMS, contadorGPRS, contadorSHELL = 0;
//    
//    uint8_t cmdLenght = 0;
//    
//    receivedBytes, sendedBytes = 0;
//    
//    for(;;)
//    {
//        
//        //gprsProcess();
//        
//        
//        //Si la cola fue creada correctamente
//        if(xModemRequests != NULL){
//            /*Si no hay elementos en la cola xModemRequests, la tarea se bloquea 
//            esperando la llegada de comandos*/
//            if(xQueueReceive( xModemRequests, &( request ), portMAX_DELAY)){
//                //Demora en el envio del comando
//                if(request.delay>0) vTaskDelay(xSegToTicks(request.delay));
//                //Envio del comando a la UART2 (modem)
//                //expexted end of frame
//                cmdLenght = strlen(request.cmd);
////                sendedBytes = UART2_WriteBuffer(request.cmd, cmdLenght ,request.expextedEndOfFrame);
//            }       
//        }
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
//    }
//}



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
            
            //bytesRecibidos = UART1_ReadBuffer(comando,MAX_COMMAND_LENGHT);
            
            //Si se recibi� algo, procesamos el comando para obtener una respuesta
            if(bytesRecibidos>0) {
                bytesRecibidos=0;
                strcpy( respuesta, processCmdLine( &comando ) );
                //Env�o la respuesta por la UART hacia la PC.
//                stringToIntArray(respuestaUART,respuesta);
                sizeRespuesta = strlen(respuesta);
                
                //bytesEnviados = UART1_WriteBuffer(respuesta,sizeRespuesta);
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
    debugUART1("vApplicationStackOverflowHook");

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

void setDeviceConfig()
{
    configDevice.type = 3;
    configDevice.serial = 9998;
    configDevice.fw_version = 1;
    configDevice.factoryReset = 0x55;
}

void setEstacionConfig()
{
    setDeviceConfig();
    /*	actualiza las variables del RTCC	*/
	get_rtcc_datetime( &tiempo );

	/*	lee la configuracion del equipo */
	estacion.tipo = configDevice.type;
	estacion.num_serie = configDevice.serial;
	estacion.hora = tiempo.hora;		//	getTimeDate(_HOUR_);
	estacion.min = tiempo.minutos;		//	getTimeDate(_MIN_);
	estacion.dia = tiempo.dia;			//	getTimeDate(_DAY_);
	estacion.mes = tiempo.mes;			//	getTimeDate(_MON_);
	estacion.anio = tiempo.anio;		//	getTimeDate(_YEAR_);
	estacion.InicioMuestras = (estacion.min & 0xF0) + 9;	//	enviado en formato BCD
	estacion.intentosConex = getConnAttempts();
	estacion.dAlmacenados = getSamplesTotal();
	estacion.pEscritura = getSamplesWrite();
	estacion.pLectura = getSamplesRead();
	estacion.fw_version = configDevice.fw_version;

	/*	tipo de reset del equipo */
	estacion.tiporeset = RCON;
}