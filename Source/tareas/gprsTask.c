/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */
#include "tareas/gprsTask.h"

uint8_t	gprsProcess( char *grpsDATA );
void SetProcessState( uint8_t * reg, uint8_t state );
uint8_t	gprsProcessNEW();

char gprsBuffer[GPRS_BUFFER_SIZE]={0};

void vTaskGPRS( void *pvParameters );


static uint32_t notification;
//Handle referenciado en tmr4.c para uso de xTaskNotify()
TaskHandle_t xGprsHandle;

uint8_t sendCmd = TRUE;

void startGprsTask(){
    
    xTaskCreate(    vTaskGPRS,
                    "vTaskGprs",
                    1000,
                    NULL,
                    MAX_PRIORITY-1, //ACOMODAR prioridades
                    &xGprsHandle);
    
    notification = 0;
  
    debugUART1("startGprsTask()");
}

void vTaskGPRS( void *pvParameters )
{   
    #define TASK_PERIOD_MS 1000
    TickType_t taskDelay;
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
    setupModem();
    
    uint16_t contador = 0;
    debugUART1("Initial section GPRS Task");
    
    
//    SetProcessState(&gprsState,gprsReset);
    
    for(;;)
    {   
        debugUART1(contador++);
        debugUART1("\r\n");
        gprsProcessNEW();
        vTaskDelay(taskDelay);
        
        
    }
}

void SetProcessState( uint8_t * reg, uint8_t state )
{
	*reg = state;
	sendCmd = TRUE;
}


uint8_t	gprsProcess( char *grpsDATA )
{
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
                UART2_WriteBuffer(atcmd_setContextClaro,strlen(atcmd_setContextClaro));
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

uint8_t	gprsProcessNEW( )
{
	/*	estados del proceso	*/
	switch ( gprsState )
	{
	/*	GprsReset: deshabilitaci�n ECHO	*/
		case( gprsReset ):		
			if( sendCmd )
			{
                debugUART1("GprsReset:\r\n");
//				UART1_WriteBuffer("GprsReset",strlen("GprsReset"));
//                UART2_WriteBuffer(atcmd_disableEcho,strlen(atcmd_disableEcho));
//                UART2_WriteBuffer(atcmd_getModemID,strlen(atcmd_getModemID));
                if(SendATCommand((string*)atcmd_getModemID,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando al modem.\r\n");
                    
			}
			else
			{
                notification = ulTaskNotifyTake(   pdTRUE,  
                                                /* Clear the notification value 
                                                * before exiting. */
                                                portMAX_DELAY ); 
                                                /* Block indefinitely. */
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
//                    debugUART1("gprsBuffer RSP: ");
//                    debugUART1(gprsBuffer);        
//                    debugUART1("\r\n");
//                    
//                    sendCmd = TRUE;
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
//                        SetProcessState( &gprsState,setContext );
                        SetProcessState( &gprsState,gprsReset );
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1("NOT OK: ");
                        debugUART1(gprsBuffer);
                    }
                }
			}
			break;
            
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
				notification = ulTaskNotifyTake(   pdTRUE,  
                                                /* Clear the notification value 
                                                * before exiting. */
                                                portMAX_DELAY ); 
                                                /* Block indefinitely. */
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
//                    debugUART1("gprsBuffer: ");
//                    debugUART1(gprsBuffer); 
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,activateContext );                 
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1(gprsBuffer);
                    }
                    
                }
			}
			break;
    }
    
    return	TRUE;
}