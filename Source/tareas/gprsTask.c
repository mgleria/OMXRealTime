/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */
#include <string.h>

#include "tareas/gprsTask.h"

void    vTaskGPRS( void *pvParameters );

void    SetProcessState( uint8_t * reg, uint8_t state );
uint8_t	FSM_GprsTask();

void    prepareSample(trama_muestra_t *tramaMuestra, muestra_t *muestraAlmacenada);
uint8_t prepareSampleToSend(trama_muestra_t *tramaMuestra, char *tramaGPRS);
char	setServerFrame( uint8_t frameType, uint8_t whichSample );
void    cleanDataGPRSBuffer();
const char* getStateName(enum GPRS_STATE state);
void	setDeviceDateTime( char * s );
void    processServerResponse();
char    isRegistered();

//Buffer de comunicación entrante y saliente con el modem
static char gprsBuffer[GPRS_BUFFER_SIZE]={0};
//Buffer donde se almacena la trama de datos generada para enviar al modem
static char tramaGPRS[GPRS_BUFFER_SIZE] = {0};

static char registered = false;
static uint8_t dataSecuence = registro;
static uint8_t sampleToSend = lastSample;

static uint32_t modemResponseNotification, sampleReadyNotification;

//Handle referenciado en tmr4.c para uso de xTaskNotify()
TaskHandle_t xGprsHandle;

TickType_t responseDelay, modemResetTime;

static uint8_t sendCmd = TRUE;
static uint8_t attempts = 0;

/*	variables externas a este archivo	*/
extern estacion_t estacion;

void startGprsTask(){
    
    xTaskCreate(    vTaskGPRS,
                    "vTaskGprs",
                    1000,
                    NULL,
                    MAX_PRIORITY-1, //ACOMODAR prioridades
                    &xGprsHandle);
    
    modemResponseNotification = 0;
    sampleReadyNotification = 0;
  
    debugUART1("startGprsTask()\r\n");
}

void vTaskGPRS( void *pvParameters )
{   
    #define GPRS_TASK_PERIOD_MS     1000
    #define MODEM_WAIT_RESPONSE_MS  250
    #define MODEM_OFF_TIME_S        10
//    #define MODEM_RESET_TIME_S      10

    TickType_t taskDelay;
    
    taskDelay = xMsToTicks(GPRS_TASK_PERIOD_MS);
    responseDelay = xMsToTicks(MODEM_WAIT_RESPONSE_MS);
    modemResetTime = xSegToTicks (MODEM_OFF_TIME_S);
    setupModem();
   
    debugUART1("Initial section GPRS Task\r\n");
    
    ////////////////////////////////////////////////////////////////////////////
//    muestra_t sample;
//    trama_muestra_t sampleFrame;
    
    for(;;)
    {   
    ////////////////////////////////////////////////////////////////////////////
//        printf("getSamplesTotal(): %d",getSamplesTotal());
//        printf("getSamplesRead(): %d",getSamplesRead());
//        printf("getSamplesWrite(): %d \r\n",getSamplesWrite());
//        
//        if(getSamplesTotal()){
//            printf("HAY muestras almacenadas\r\n");
//            resetSamplesPtr();
//        }
//        else
//            printf("NO hay muestras almacenadas \r\n");
        
    ////////////////////////////////////////////////////////////////////////////
        
        
        FSM_GprsTask();
        
        vTaskDelay(taskDelay);  
    }
}

void SetProcessState( uint8_t * reg, uint8_t state )
{
    printf("%s -> %s \r\n",getStateName(*reg),getStateName(state));
	*reg = state;
	sendCmd = TRUE;
    attempts = 0;
}

uint8_t	FSM_GprsTask( )
{
	/*	estados del proceso	*/
	switch ( gprsState )
	{
	/*	GprsReset: reset + deshabilitacion ECHO	*/
		case( gprsReset ):		
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{
                //Apago el modem y espero MODEM_RESET_TIME_S para encenderlo
                _LATB12 = 0;
                debugUART1("Modem OFF\r\n");
                vTaskDelay(modemResetTime);
                _LATB12 = 1;
                debugUART1("Modem ON. Waiting for complete boot...\r\n");
                vTaskDelay(modemResetTime*2);
                debugUART1("Modem initialization complete\r\n");

                registered = false;
                
                /*Si hay muestras pendientes de enviar, armo las tramas y 
                 * postergo el registro*/
                if( isThereSamplesToSend() ){
                    dataSecuence = muestras;
                }       
                //Si NO hay muestras sin enviar, registro la estación
                else{
                    dataSecuence = registro;
                }
                setServerFrame(dataSecuence,lastSample);
                
                if(SendATCommand((string*)atcmd_initialConfig,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_initialConfig); 
                    
                                    
			}
			else
			{
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                printf("modemResponseNotification: %s\r\n",modemResponseNotification);
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    attempts++;
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,disableEcho );
                    }
                    else
                    {
                        debugUART1("NOT OK: ");
                        debugUART1(gprsBuffer);
                        if(attempts>MAX_ATTEMPTS_NUMBER-1){
                            SetProcessState( &gprsState,gprsReset );
                            debugUART1("Maximo numero de intentos alcanzado. Reiniciando...");
                        }
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                    SetProcessState( &gprsState,gprsReset );
                }
			}
		break;
        
        case( disableEcho ):
			printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{
                if(SendATCommand((string*)atcmd_disableEcho,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_disableEcho);
			}
			else
			{
				modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_)){
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,initModem );
//                        SetProcessState( &gprsState,gprsReset );
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_ERROR_  )){
                        //TimeOutCounter();
                        debugUART1("_ERROR_\n\r");
                        SetProcessState( &gprsState,  gprsReset);
                    }
                    else {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1("ALGO SALIO MAL: ");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState,  gprsReset);
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
            
        case( initModem ):
			printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{           
                if(SendATCommand((string*)atcmd_STN_OFF,gprsBuffer,gprsBuffer,10,0,1)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_STN_OFF); 
			}
			else
			{
				modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_)){
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,setContext );
//                        SetProcessState( &gprsState,gprsReset );
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_ERROR_  )){
                        //TimeOutCounter();
                        debugUART1("_ERROR_\n\r");
                        SetProcessState( &gprsState,  gprsReset);
                    }
                    else {
                        debugUART1("ALGO SALIO MAL: ");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState,  gprsReset);
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
            
        case( setContext ):
			printf("----------State: %s----------\r\n",getStateName(gprsState));
            if( sendCmd )
			{
				/*	modo recepcion para espera de la respuesta	*/
                if(SendATCommand((string*)atcmd_setContextClaro,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_setContextClaro);
			}
			else
			{			
               /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,configSocket );                 
                    }
                    else
                    {
                        debugUART1("ALGO SALIO MAL: ");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState,  gprsReset);
                    }
                    
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        /*	Configuracion de Socket	*/
		case( configSocket ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{		
				if(SendATCommand((string*)atcmd_configSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_configSocketHARDCODED);
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,configExtendSocket );                 
                    }
                    else
                    {
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState,  gprsReset);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        /*	Configuracion de Socket	*/
		case( configExtendSocket ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_configExtendSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_configExtendSocketHARDCODED);
                
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,activateContext );                 
                    }
                    else
                    {
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState,  gprsReset);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        case( activateContext ):
			printf("----------State: %s----------\r\n",getStateName(gprsState));
            if( sendCmd )
			{
                if(SendATCommand((string*)atcmd_activateContextHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                    else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_activateContextHARDCODED); 
                }
			else
			{
                /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_)) {
//                        saveIPaddress( (char*)gprsBuffer );
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,socketDial );                 
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) ){
                        debugUART1("_ERROR_\n\r");
                        SetProcessState( &gprsState,gprsReset );
//                        TimeOutCounter();
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_)) {
                        debugUART1("_NOCARRIER_\n\r");
                        SetProcessState( &gprsState, gprsReset);
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) ) {
                        debugUART1("_TIMEOUT_\n\r");
                        SetProcessState( &gprsState, gprsReset);
//                        TimeOutCounter();
                    }
                    else {
                        debugUART1("ALGO SALIO MAL\n\r");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState, gprsReset);
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
        
        case( socketDial ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_socketDialHARDCODED_1,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_socketDialHARDCODED_1);
			}
			else
			{           
				/* Clear the modemResponseNotification value before exiting. & Block indefinitely*/
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,socketSend );                 
                    }
                    else
                    {
                        debugUART1("ALGO SALIO MAL\n\r");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState, gprsReset);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
            
        case( socketSend ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_socketSend,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_socketSend);
			}
			else
			{   
               /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,">"))
                    {
                        debugUART1("> RECIBIDO\n\r");
                        SetProcessState( &gprsState,putData );                 
                    }
                    else
                    {
                        debugUART1("ALGO SALIO MAL\n\r");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState, gprsReset);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        case( putData ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{
                /*A continuacion de la trama, envio el caracter especial 
                * EndOfFile (EOF)*/
//                if(SendATCommand((string*)tramaGPRS,gprsBuffer,gprsBuffer,10,0,2)>0){        
                if(SendATCommand((string*)atcmd_FRAME2,gprsBuffer,gprsBuffer,10,0,2)>0){
                    if(SendATCommand((string*)atcmd_EOF,gprsBuffer,gprsBuffer,10,0,2)>0){
                        /*	modo recepcion para espera de la respuesta	*/
                        sendCmd = FALSE;
                    }
                    else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_EOF);
                }
                else printf("ERROR no se pudo enviar la trama\r\n %s \r\nal modem.\r\n",tramaGPRS);
                
                debugUART1("Trama: ");
                debugUART1(tramaGPRS);
                debugUART1("\r\n");
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
                    if(strstr(gprsBuffer,"SRING"))
                    {		
                        debugUART1("SRING Recibido\r\n");
                        SetProcessState( &gprsState, receiveData);
                    }
                    else if( strstr(gprsBuffer, (string*)_NOCARRIER_)) {
                        debugUART1("_NOCARRIER_\n\r");
//                        SetProcessState( &gprsState, gprsReset);
                        SetProcessState( &gprsState, connectionStatus);
                    }
                    else
                    {
                        debugUART1("WAITING SRING:  ");
                        debugUART1(gprsBuffer);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;		
		
        /*	Receive data  */
		case( receiveData ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
			if( sendCmd )
			{
                if(SendATCommand((string*)atcmd_sListenHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_sListenHARDCODED);
			}
			else
			{       
               /* Aguarda por respuesta completa del modem */
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    debugUART1("Rta SERVER: ");
                    debugUART1(gprsBuffer);
                    debugUART1("\r\n");
                    
                    char *p;
                    
                    if(p = strstr(gprsBuffer,"024F")) //Quiza deberia chequear solo el encabezado de la trama para evitar falsos positivos
                    {   
                        debugUART1("Rta SERVER: 024F\r\n");
                        //Corrijo la config de sensores y la fecha-hora 
                        setDeviceSensorEnables( p + 6 ); //7
						setDeviceDateTime( p + 13 );    //14
                        debugUART1("Configuracion de sensores y RTCC actualizada.\r\n");
                                                                   
                    }
//                    else if(strstr(gprsBuffer,"004F"))
//                    {
//                        debugUART1("Rta SERVER: 004F\r\n");
//                        processServerResponse();  
//                    }
                    else{
                        debugUART1("Respuesta del server desconocida\r\n");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState, gprsReset);
                    }
                    processServerResponse();
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }  
            }
            break;
			
        /*	Close a Socket	
         * Cierra el puerto de comunicacion con el servidor.
         */
		case( closeSocket ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
            if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_closeSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_closeSocketHARDCODED);
			}
			else
			{           
				/* Clear the modemResponseNotification value before exiting. & Block indefinitely*/
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
                    if(strstr(gprsBuffer,_OK_))
                    {			
                        debugUART1("_OK_\n\r");
    //					SetProcessState( &gprsState, configExtendSocket);
                        SetProcessState( &gprsState, waitForNewRequests);
                    }
                    else
                    {
                        debugUART1("ALGO SALIO MAL\n\r");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState, gprsReset);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        case( waitForNewRequests):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
            /*Colocar mecanismo sleep para esperar notificaciones desde la tarea 
             sample y disponerse a enviar una trama*/
            
            sampleReadyNotification = ulTaskNotifyTake(   pdTRUE, portMAX_DELAY ); 
                
            if(sampleReadyNotification == NEW_SAMPLE_NOTIFICATION){
                SetProcessState( &gprsState, connectionStatus);
            }
            break;
        
   
        /* En este estado verificamos conectividad del modem*/
        case( connectionStatus ):
            printf("----------State: %s----------\r\n",getStateName(gprsState));
            //Hay que verificar si el equipo tiene si tengo ip
            if( sendCmd )
			{
                //Chequeo si tengo IP
                 if(SendATCommand((string*)atcmd_checkIP,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_checkIP);
			}
			else
			{
				//Espero que el modem termine de enviar su respuesta
                modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    //Si tengo IP, la respuesta es de la forma ?xxx.yyy.zzz.www?
                    if((!(strstr(gprsBuffer,"AT")))&&(strlen(gprsBuffer)<40)){    
                    //NO TIENE IP, ES DECIR NO ESTOY CONECTADO			
                        if( setServerFrame( dataSecuence, lastSample ) ){
                            //NO ESTOY CONECTADO Y TENGO QUE MANDAR MUESTRAS, ACTIVO CONTEXTO
                            SetProcessState( &gprsState, activateContext);	
                            printf("connectionStatus -> activateContext \r\n");
                        }
                        else{
                            printf("connectionStatus sin IP y sin muestras pendientes\r\n");
                            SetProcessState( &gprsState, waitForNewRequests);
                            printf("connectionStatus -> waitForNewRequests \r\n");
                        }
                    }
                    else if(strlen(gprsBuffer)>40){
                    //TENGO IP, ESTOY CONECTADO
                        if( setServerFrame( dataSecuence, lastSample ) ) {	
                        //TENGO MUESTRAS PARA ENVIAR
                            SetProcessState( &gprsState, putData);
                            printf("connectionStatus -> putData \r\n");
                        }
                        else {
                        //NO TENGO MUESTRAS PARA ENVIAR
                            SetProcessState( &gprsState,waitForNewRequests);
                            printf("connectionStatus con IP sin muestras para enviar \r\n");
                            printf("connectionStatus -> waitForNewRequests \r\n");
                        }
                    }   
                    else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) ) {
                        printf("connectionStatus NOCARRIER \r\n");
                        SetProcessState( &gprsState, gprsReset);
                        printf("connectionStatus -> gprsReset \r\n");
                    }
                    else{
                        debugUART1("Estado de conexion indeterminado\r\n");
                        debugUART1(gprsBuffer);
                        SetProcessState( &gprsState, gprsReset);
                        printf("connectionStatus -> gprsReset \r\n");
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
    }
    
    return	TRUE;
}

/**********************************************************************************************/
/**
 * \brief
 * Arma el dato para transmitir al servidor. y lo almacena en tramaGPRS[] global
 * En este archivo.\n
 * Dependiendo del frameType, los datos a enviar pueden ser:\n
 * 0x01	muestra\n
 * 0x02	reservado\n
 * 0x03	configuracion\n
 * 0x04	registro\n
 *
 * @param frameType		Tipo de Frame que se desee armar : muestras, configuracion o registro
 * @param whichSample	Última muestra disponible (lastSample) o las siguientes (nextSample) 
 * @return				TRUE en caso de exito o FALSE si hubo algún problema o no hay muestras para enviar
 */
char	setServerFrame( uint8_t frameType, uint8_t whichSample )
{
	//Clean buffer
    cleanDataGPRSBuffer();
	
	muestra_t muestraAlmacenada;
    trama_muestra_t tramaMuestra;
	trama_config_t configFrame;
//	trama_inicio_t tramaInicio;

//	extern rtcc_t tiempo;

	switch( frameType )
	{
		case( muestras ):
			//	levanta la proxima muestra para formatear a la trama de envio al servidor
			if( getSample( &muestraAlmacenada, whichSample ) == FALSE ){
                return	FALSE;	//	no hay mas muestras para enviar
            }
            else{
                prepareSample(&tramaMuestra, &muestraAlmacenada);
                prepareSampleToSend(&tramaMuestra,tramaGPRS);
            }
            break;

		case( configuracion ): //Revisar el struct estacion y cuando se inicia
//			configFrame.cmd = frameType;
//			configFrame.tipo = estacion.tipo;
//			configFrame.num_serie = swapBytes( estacion.num_serie );
//			configFrame.reservado1 = 0x0101;						//	Numero de grupo
//			configFrame.reservado2 = 0xFFFF;						//	Libre
//			configFrame.capMemoria = swapBytes( (uint16)(MAX_SAMPLES) );			//	Capacidad de memoria
//			configFrame.tiempoProm = 0x00;							//	Tiempo Promedio
//			configFrame.periodo = 0x0A;								//	Periodo
//            
//			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_1, &configFrame.sensorHab1 );
//			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_2, &configFrame.sensorHab2 );
//			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_3, &configFrame.sensorHab3 );
//			configFrame.nullE = NULL;								//	para indicar fin de trama
//
//			strncpy( (char*)&tramaGPRS, (char*)&string_cabecera, strlen((char*)&string_cabecera) );
//			n = 0;	m = (char*)&configFrame;
//			while( n < (sizeof(trama_config_t)-1) )
//				sprintf( (char*)t + (2*n++), (const char*)"%02X", (*m++) );
//			strcat( (char*)tramaGPRS, (char*)&string_cierre );
            
            //Envío hardcodeado una trama de registro
            strcpy(tramaGPRS,atcmd_FRAME_1);
			break;

		case( registro ):
			//	copia los datos a la trama - estacion
//			tramaInicio.cmd = frameType;
//			tramaInicio.tipo = estacion.tipo;
//			tramaInicio.num_serie = swapBytes( estacion.num_serie );
//			tramaInicio.hora = bcd2dec( tiempo.hora );
//			tramaInicio.min = bcd2dec( tiempo.minutos );
//			tramaInicio.dia = bcd2dec( tiempo.dia );
//			tramaInicio.mes = bcd2dec( tiempo.mes );
//			tramaInicio.anio = bcd2dec( tiempo.anio );
//			tramaInicio.alarmaMuestras = 0;
//			tramaInicio.tiempoMedicion = 0;
//			tramaInicio.inicioMuestras = bcd2dec( estacion.InicioMuestras );
//			tramaInicio.pEscritura = swapBytes( getSamplesWrite() );
//			tramaInicio.pLectura = swapBytes( getSamplesRead() );
//			tramaInicio.dAlmacenados = swapBytes( getSamplesTotal() );
//			tramaInicio.pMaxMin = 0;
//			tramaInicio.periodo = 0x0A;
////			i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_1, &tramaInicio.sensorHab1, sizeof(tramaInicio.sensorHab1) );
////			i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_2, &tramaInicio.sensorHab2, sizeof(tramaInicio.sensorHab2) );
////			i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_3, &tramaInicio.sensorHab3, sizeof(tramaInicio.sensorHab3) );
//			/*	caracter nulo para indicar fin de trama	*/
//			tramaInicio.nullE = NULL;
//
//			strncpy( (char*)&tramaGPRS, (char*)&string_cabecera, strlen((char*)&string_cabecera) );
//			n = 0;	m = (char*)&tramaInicio;
//			while( n < sizeof(trama_inicio_t)-1 )
//				sprintf( (char*)t + (2*n++), (const char*)"%02X", (*m++) );
//			strcat( (char*)tramaGPRS, (char*)&string_cierre );
            
            //Envío hardcodeado una trama de configuracion
            strcpy(tramaGPRS,atcmd_FRAME_3);
			break;

		default:
		{
			return	FALSE;
		}
	}
	return	TRUE;
}

void prepareSample(trama_muestra_t *tramaMuestra, muestra_t *muestraAlmacenada)
{
    //	copia los datos a la trama - estacion
    tramaMuestra->cmd = muestraAlmacenada->cmd;
    tramaMuestra->tipo = muestraAlmacenada->tipo;  //estacion.tipo; //configDevice.type = 0;
    tramaMuestra->num_serie = muestraAlmacenada->num_serie; //swapBytes( estacion.num_serie ); //configDevice.serial = 0;
    tramaMuestra->hora = bcd2dec( muestraAlmacenada->hora );
    tramaMuestra->min = bcd2dec( muestraAlmacenada->minutos );
    tramaMuestra->dia = bcd2dec( muestraAlmacenada->dia );
    tramaMuestra->mes = bcd2dec( muestraAlmacenada->mes );
    tramaMuestra->anio = bcd2dec( muestraAlmacenada->anio );
    tramaMuestra->signal = muestraAlmacenada->senial;
//    #ifdef	USE_HIH6131
    #if !defined	USE_HIH6131
    tramaMuestra->clima.temper = muestraAlmacenada->clima.temper | 0x8000;
    tramaMuestra->clima.hum = muestraAlmacenada->clima.hum | 0x8000;
    #else
    tramaMuestra->clima.temper = muestraAlmacenada->clima.temper;
    tramaMuestra->clima.hum = muestraAlmacenada->clima.hum;
    #endif
    tramaMuestra->clima.presion = muestraAlmacenada->clima.presion;
    tramaMuestra->clima.lluvia = muestraAlmacenada->clima.lluvia;
    tramaMuestra->clima.radiacionSolar = muestraAlmacenada->clima.radiacionSolar;
    //	el modelo de pluviometro chino se guarda con la mascara->
//    #if	defined	(__OMX_S_C__)
//    tramaMuestra->clima.lluvia |= 0x4000;		
//    #endif
    #if	defined	(__OMX_T__)
    tramaMuestra->clima.radiacionSolar |= 0x8000;
    tramaMuestra->clima.lluvia |= 0x8000;
    #endif
    #ifdef	USE_RK200
    tramaMuestra->clima.radiacionSolar |= 0x4000;
    #endif
    #if	defined	(__OMX_S__)	||	defined	(__OMX_S_C__) ||  defined	(__OMX_T__)
    tramaMuestra->clima.viento.direccionP = muestraAlmacenada->clima.viento.direccionP;
    tramaMuestra->clima.viento.velocidadP = muestraAlmacenada->clima.viento.velocidadP;
    tramaMuestra->clima.viento.direccionM = muestraAlmacenada->clima.viento.direccionM;
    tramaMuestra->clima.viento.velocidadM = muestraAlmacenada->clima.viento.velocidadM;
    tramaMuestra->clima.luzDia = muestraAlmacenada->clima.luzDia;
    tramaMuestra->clima.tempSuelo1 = muestraAlmacenada->clima.tempSuelo1;
    tramaMuestra->clima.humSuelo1 = muestraAlmacenada->clima.humSuelo1;
//    tramaMuestra->clima.tempSuelo2 = muestraAlmacenada->clima.tempSuelo2 | 0x4000;
    tramaMuestra->clima.tempSuelo2 = muestraAlmacenada->clima.tempSuelo2;
    tramaMuestra->clima.humSuelo2 = muestraAlmacenada->clima.humSuelo2;
    tramaMuestra->clima.tempSuelo3 = muestraAlmacenada->clima.tempSuelo3;
    tramaMuestra->clima.humSuelo3 = muestraAlmacenada->clima.humSuelo3;
    tramaMuestra->clima.humHoja = muestraAlmacenada->clima.humHoja;
    tramaMuestra->clima.tempHoja = muestraAlmacenada->clima.tempHoja;
    #elif   defined (__OMX_N__)
    //tramaMuestra->clima.luzDia = muestraAlmacenada->clima.luzDia;
    //tramaMuestra->clima.radiacionSolar = muestraAlmacenada->clima.radiacionSolar;
    tramaMuestra->clima.tempSuelo1 = muestraAlmacenada->clima.tempSuelo1;
    tramaMuestra->clima.humSuelo1 = muestraAlmacenada->clima.humSuelo1;
    tramaMuestra->clima.tempSuelo2 = muestraAlmacenada->clima.tempSuelo2;
    tramaMuestra->clima.humSuelo2 = muestraAlmacenada->clima.humSuelo2;
    tramaMuestra->clima.nivel = muestraAlmacenada->clima.nivel;               //Muestra nivel
    tramaMuestra->clima.humSuelo3 = muestraAlmacenada->clima.humSuelo3;
    tramaMuestra->clima.humHoja = muestraAlmacenada->clima.humHoja;
    tramaMuestra->clima.tempHoja = muestraAlmacenada->clima.tempHoja;
    #endif

    #if	defined	(__OMX_T__)
    tramaMuestra->clima.tempSuelo2 += 0x4000;
    #endif

    #if	defined	(__OMX_S__) 
    tramaMuestra->corriente1 = muestraAlmacenada->corriente1;
    tramaMuestra->corriente2 = muestraAlmacenada->corriente2;
    #endif
    tramaMuestra->bateria = muestraAlmacenada->bateria;
    tramaMuestra->periodo = muestraAlmacenada->periodo;
    tramaMuestra->sensorHab1 = muestraAlmacenada->sensorHab1;
    tramaMuestra->sensorHab2 = muestraAlmacenada->sensorHab2;
    tramaMuestra->sensorHab3 = muestraAlmacenada->sensorHab3;
    tramaMuestra->nullE = muestraAlmacenada->nullE;
}

uint8_t prepareSampleToSend(trama_muestra_t *tramaMuestra, char *tramaGPRS)
{
    uint8_t n,k;
    
    char *p = NULL; char *t = NULL;
//    const char string_cabecera[] = "";
//    const char string_cierre[] = "\x001A";

//    char *t = tramaGPRS + strlen((char*)&string_cabecera);
      
    // Armo el buffer a transmitir: concatenado de cadenas cabecera, datos y cierre.
    // @todo quitar el caracter null de fin de trama ya que se controla la cantidad con sizeof
//    strncpy( (char*)&tramaGPRS, (char*)&string_cabecera, strlen((char*)&string_cabecera) );

    n = 0;
	t = tramaGPRS;
    p = (char*)tramaMuestra;
    
    
//    printf("sizeof(trama_muestra_t): %d\r\n",sizeof(trama_muestra_t));
    
    while( n < sizeof(trama_muestra_t)-2 ){ //No agarra el byte nullE
        sprintf( (char*)t + (2*n), (const char*)"%02X", *(p+n));
        n++;
//        printf("n:%d | (char*)t+(2*n):%s\r\n",n,(char*)t + (2*n));
    }
    
//    printf("n*2:%d | (char*)t+(2*(n-1)):%s\r\n",n*2,(char*)t+(2*(n-1)));
    
//No es necesario el caracter de fin de trama porque el estado 'putData' lo pone
//    strncpy( tramaGPRS + n*2, string_cierre, strlen(string_cierre) );
    
    
    //Para eliminar el byte de padding generado al comienzo del struct viento
    for(k=42;k<116;k++){
        tramaGPRS[k]=tramaGPRS[k+2];
    }

    return true;        
}

void    cleanDataGPRSBuffer()
{
    int i;
    for(i=0;i<GPRS_BUFFER_SIZE;i++){
        tramaGPRS[i]=NULL;
    }
}

const char* getStateName(enum GPRS_STATE state) 
{
   switch (state) {
        case gprsReset: return "gprsReset" ;
        case disableEcho: return "disableEcho" ;
        case initModem: return "initModem" ;
        case setContext: return "setContext" ;
        case activateContext: return "activateContext" ;
        case configSocket: return "configSocket" ;
        case configExtendSocket: return "configExtendSocket" ;
        case socketDial: return "socketDial" ;
        case socketSend: return "socketSend" ;
        case closeSocket: return "closeSocket" ;
        case putData: return "putData" ;
        case receiveData: return "receiveData" ;
        case waitForNewRequests: return "waitForNewRequests" ;
        case connectionStatus: return "connectionStatus" ;
        default: return "Comando desconocido" ;
   } 
}

void	setDeviceDateTime( char * s )
{
	
	/*	formato servidor omixom	*/
	rtcc_t t;
	t.dia = 0;	t.dia |= (s[0]<<4)&0xF0;	t.dia |= (s[2]&0xF);
	t.mes = 0;	t.mes |= (s[4]<<4)&0xF0;	t.mes |= (s[6]&0xF);
	t.anio = 0;	t.anio |= (s[8]<<4)&0xF0;	t.anio |= (s[10]&0xF);
	t.hora = 0;	t.hora |= (s[12]<<4)&0xF0;	t.hora |= (s[14]&0xF);
	t.minutos = 0;	t.minutos |= (s[16]<<4)&0xF0;	t.minutos |= (s[18]&0xF);
	set_rtcc_datetime( &t );
}

void    processServerResponse()
{
    /*Si la trama anterior fue una muestra o configuracion,
    y si tengo muestras pendientes de enviar*/
    if(isThereSamplesToSend()) {
        dataSecuence = muestras;
        setServerFrame(dataSecuence,nextSample);
        SetProcessState(&gprsState, socketSend);
        debugUART1("Setting next frame to send.\r\n");
    }
    else{ 
        if(!isRegistered()){
            //No se registró. Procede a registrarse.
            dataSecuence = registro;
            setServerFrame(dataSecuence,nextSample);
            debugUART1("Sending register frame.\r\n");
            SetProcessState(&gprsState, socketSend);
        }
        else if( dataSecuence == registro){
            //La trama anterior fue de registro, ahora tiene que mandar de configuracion
            //Notifico a Sample que ya tengo la hora del servidor
//            xTaskNotify(xGprsHandle, SYNC_SERVER_TIME_NOTIFICATION,eSetValueWithOverwrite);
            registered = true;
            dataSecuence = configuracion;
            setServerFrame(dataSecuence,nextSample);
            SetProcessState(&gprsState, socketSend);
            debugUART1("Frame registro sended. Preparing configuration frame.\r\n");
        }
        else{
            //No hay muestras pendientes por enviar
            SetProcessState(&gprsState, closeSocket);
            debugUART1("No more frames. Closing socket...\r\n");
        }
    }
    
}

char isRegistered()
{
    return registered;
}