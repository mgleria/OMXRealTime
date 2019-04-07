/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */
#include <string.h>
#include <stdio.h>

#include "tareas/gprsTask.h"
//#include "funciones/rtcc.h"

void    vTaskGPRS( void *pvParameters );

void    SetProcessState( uint8_t * reg, uint8_t state );
uint8_t	FSM_GprsTask();

void    prepareSample(trama_muestra_t *tramaMuestra, muestra_t *muestraAlmacenada);
uint8_t buildHexFrame(char *trama, char *tramaGPRS, uint8 frameSize);
char	setServerFrame( uint8_t frameType, uint8_t whichSample );
void    cleanDataGPRSBuffer();
const char* getStateName(enum GPRS_STATE state);
void	setDeviceDateTime( char * s );
uint8    getNextDataSecuence();
char    isRegistered();
char    registerInProcess();
const char* getFrameType(uint8 frameType);
void    removePaddingBytes(char *tramaGPRS,uint8 paddingPosition, uint8 frameSize);
void    cleanBufferTail(uint8 frameSize);
headerOptions_t getHeaderIndex(const char* header);

//Buffer de comunicaci�n entrante y saliente con el modem
static char gprsBuffer[MODEM_BUFFER_SIZE]={0};
static char header[FRAME_HEADER_SIZE]={0};
static headerOptions_t headerIndex; 

//Buffer donde se almacena la trama de datos generada para enviar al modem
static char tramaGPRS[GPRS_BUFFER_SIZE] = {0}; //Ver si puede unificar con MODEM_BUFFER_SIZE

static char registered = false;
static char registering = false;

static uint8_t dataSecuence = registro;
static uint8_t sampleToSend = lastSample;

//static uint32_t modemResponseNotification;
static uint32_t sampleReadyNotification;
//Handle referenciado en tmr4.c para uso de xTaskNotify()
TaskHandle_t xGprsHandle;

TickType_t responseDelay, modemResetTime;

static uint8_t sendCmd = true;
uint8_t attempts = 0;

/*	variables externas a este archivo	*/
extern estacion_t estacion;

void startGprsTask(){
    
    xTaskCreate(    vTaskGPRS,
                    "vTaskGprs",
                    1000,
                    NULL,
                    MAX_PRIORITY-1, //to-do: ACOMODAR prioridades
                    &xGprsHandle);
    
    sampleReadyNotification = 0;
  
    debug("startGprsTask()");
}

void vTaskGPRS( void *pvParameters )
{   
    #define GPRS_TASK_PERIOD_MS     1000
    #define MODEM_WAIT_RESPONSE_MS  500
    #define MODEM_OFF_TIME_S        20
//    #define MODEM_RESET_TIME_S      10

    TickType_t taskDelay;
    
    taskDelay = xMsToTicks(GPRS_TASK_PERIOD_MS);
    responseDelay = xMsToTicks(MODEM_WAIT_RESPONSE_MS);
    modemResetTime = xSegToTicks (MODEM_OFF_TIME_S);
    setupModem();
   
    debug("Initial section GPRS Task");
    
    ////////////////////////////////////////////////////////////////////////////
//    muestra_t sample;
//    trama_muestra_t sampleFrame;
    
    for(;;)
    {   
//        printf("////////////////////GPRS Task\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");     
        LEDToggle(0x2);
        FSM_GprsTask();
        vTaskDelay(taskDelay);  
    }
}

void SetProcessState( uint8_t * reg, uint8_t state )
{
//    printf("%s -> %s \n",getStateName(*reg),getStateName(state));
	*reg = state;
	sendCmd = true;
    attempts = 0;
    flushBuffer(gprsBuffer, MODEM_BUFFER_SIZE);
}

uint8_t	FSM_GprsTask( )
{
    uint8_t modemResponseFlag;
    bool waitingSRING = true;
	/*	estados del proceso	*/
	switch ( gprsState )
	{
	/*	GprsReset: reset + deshabilitacion ECHO	*/
		case( gprsReset ):		
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: gprsReset");
			if( sendCmd )
			{
                registered = false;
                
                /*Si hay muestras pendientes de enviar, armo las tramas y 
                 * postergo el registro*/
                if( isThereSamplesToSend()){
                    dataSecuence = muestras;
                }       
                //Si NO hay muestras sin enviar, registro la estacion
                else{
                    dataSecuence = registro;
                }
                
                //Apago el modem y espero MODEM_RESET_TIME_S para encenderlo
                _LATB12 = 0;
                debug("Modem OFF");
                vTaskDelay(modemResetTime);
                _LATB12 = 1;
                debug("Modem ON. Waiting for complete boot...");
                vTaskDelay(modemResetTime*2);
                debug("Modem initialization complete");
                
                /*Armo el frame para la primera vez que llega al estado putData*/
                setServerFrame(dataSecuence,lastSample);
                //Envio el comando hacia el modem.
                if(SendATCommand((string*)atcmd_initialConfig,gprsBuffer,gprsBuffer,10,0,2)>0){
//                if(SendATCommand((string*)atcmd_getModemID,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                } 
			}
			else{
                if(receiveATCommand( gprsBuffer, &attempts, responseDelay ))
                {
                   if(strstr(gprsBuffer,_OK_))
                    {
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,disableEcho );
                    }
                    else
                    {
                        debug("NOT OK: ");
                        if (strlen(gprsBuffer))
                            debug(gprsBuffer);
                        else
                            debug("GPRS buffer vacio");
                        if(attempts>MAX_ATTEMPTS_NUMBER-1){
                            SetProcessState( (uint8_t *)&gprsState,gprsReset );
                            debug("Maximo numero de intentos alcanzado. Reiniciando...");
                        }
                    }
                } 
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                    SetProcessState( (uint8_t *)&gprsState,gprsReset );
                }               
			}
		break;
        
        case( disableEcho ):
//			printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: disableEcho");
			if( sendCmd )
			{
                if(SendATCommand((string*)atcmd_disableEcho,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strstr(gprsBuffer,_OK_)){
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,initModem );
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_ERROR_  )){
                        //TimeOutCounter();
                        debug("_ERROR_");
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }
                    else {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debug("ALGO SALIO MAL: ");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
			break;
            
        case( initModem ):
//			printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: initModem");
			if( sendCmd )
			{           
                if(SendATCommand((string*)atcmd_STN_OFF,gprsBuffer,gprsBuffer,10,0,1)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strstr(gprsBuffer,_OK_)){
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,setContext );
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_ERROR_  )){
                        //TimeOutCounter();
                        debug("_ERROR_");
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }
                    else {
                        debug("ALGO SALIO MAL: ");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
			break;
            
        case( setContext ):
//			printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: setContext");
            if( sendCmd )
			{
				/*	modo recepcion para espera de la respuesta	*/
                if(SendATCommand((string*)atcmd_setContextPersonal,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{			
               /* Aguarda por respuesta completa del modem */
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,configSocket );                 
                    }
                    else
                    {
                        debug("ALGO SALIO MAL: ");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }
                    
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
		break;
        
        /*	Configuracion de Socket	*/
		case( configSocket ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: configSocket");
			if( sendCmd )
			{		
				if(SendATCommand((string*)atcmd_configSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,configExtendSocket );                 
                    }
                    else
                    {
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }   
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
		break;
        
        /*	Configuracion de Socket	*/
		case( configExtendSocket ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: configExtendSocket");
			if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_configExtendSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
                
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,activateContext );                 
                    }
                    else
                    {
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState,  gprsReset);
                    }   
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
		break;
        
        case( activateContext ):
//			printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: activateContext");
            if( sendCmd )
			{
                if(SendATCommand((string*)atcmd_activateContextHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
            }
			else
			{
                /* Aguarda por respuesta completa del modem */
                vTaskDelay(xSegToTicks(3));
                if(receiveATCommand(gprsBuffer, &attempts, xSegToTicks(5))) { //
                    if(strstr(gprsBuffer,_OK_)) {
//                        saveIPaddress( (char*)gprsBuffer );
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,socketDial );                 
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) ){
                        debug("_ERROR_");
                        SetProcessState( (uint8_t *)&gprsState,gprsReset );
//                        TimeOutCounter();
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_)) {
                        debug("_NOCARRIER_");
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }
                    else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) ) {
                        debug("_TIMEOUT_");
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
//                        TimeOutCounter();
                    }
                    else {
                        debug("ALGO SALIO MAL");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
			break;
        
        case( socketDial ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: socketDial");
			if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_socketDialHARDCODED_1,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{           
				/* Clear the modemResponseNotification value before exiting. & Block indefinitely*/
                vTaskDelay(xSegToTicks(2));
                if(receiveATCommand(gprsBuffer, &attempts, xSegToTicks(5))) {
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debug("_OK_");
                        SetProcessState( (uint8_t *)&gprsState,socketSend );                 
                    }
                    else
                    {
                        debug("ALGO SALIO MAL");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }   
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
			break;
//            
        case( socketSend ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: socketSend");
			if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_socketSend,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{   
               /* Aguarda por respuesta completa del modem */
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strstr(gprsBuffer,">"))
                    {
                        debug("> RECIBIDO");
                        SetProcessState( (uint8_t *)&gprsState,putData );                 
                    }
                    else
                    {
                        debug("ALGO SALIO MAL");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }   
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
		break;
        
        case( putData ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: putData");
			if( sendCmd )
			{
                /*A continuacion de la trama, envio el caracter especial 
                * EndOfFile (EOF)*/
                if(SendATCommand((string*)tramaGPRS,gprsBuffer,gprsBuffer,10,0,2)>0){        
//                if(SendATCommand((string*)atcmd_FRAME2,gprsBuffer,gprsBuffer,10,0,2)>0){
                    if(SendATCommand((string*)atcmd_EOF,gprsBuffer,gprsBuffer,10,0,2)>0){
                        /*	modo recepcion para espera de la respuesta	*/
                        sendCmd = false;
//                        TMR5_Start();
                    }
                    else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el final de trama al modem.");
                    }
                }
                else{
//                    printf("ERROR no se pudo enviar la trama\n %s \nal modem.\n",tramaGPRS);
                    debug("ERROR no se pudo enviar la trama al modem.");
                }
                printCurrentRTCCTime();//Timestamp para evaluar delay comando-respuesta
//                printf("tramaGPRS:\n%s(%s)\n",tramaGPRS,getFrameType(dataSecuence));
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
//                vTaskDelay(xSegToTicks(5)); //Espera para recibir respuesta del server
//                modemResponseFlag = UART3_ReadBuffer(gprsBuffer, MODEM_BUFFER_SIZE);
                modemResponseFlag = receiveATCommand(gprsBuffer, &attempts, xSegToTicks(5));
                if(modemResponseFlag){
                        waitingSRING = !strstr(gprsBuffer,"SRING");
//                        debug("MDM Response:");
//                        debug(gprsBuffer);
                }
                else
                    debug("Else no controlado 1");
                
                while(waitingSRING && attempts < MAX_ATTEMPTS_NUMBER) {
                    attempts++;
                    modemResponseFlag = getServerResponse(gprsBuffer, xSegToTicks(5));
                    
                    if(modemResponseFlag)
                        waitingSRING = !strstr(gprsBuffer,"SRING");
                    else
                        debug("Sin respuesta del server.");
                }
                //Si waitingSRING es false, significa que ya lo he recibido
                if(!waitingSRING) {
                    debug("SRING Recibido");
                    SetProcessState( (uint8_t *)&gprsState, receiveData);
                }
                else {
                    debug("La respuesta del server ha tardado demasiado. Reiniciando comunicacion...");
                    SetProcessState( (uint8_t *)&gprsState,gprsReset );
                }
                
                
//                if(modemResponseFlag) {                 
//                    printCurrentRTCCTime(); //Timestamp para evaluar delay comando-respuesta
//                    if(strstr(gprsBuffer,"SRING"))
//                    {		
//                        debug("SRING Recibido\n");
//                        SetProcessState( (uint8_t *)&gprsState, receiveData);
//                    }
//                    else if( strstr(gprsBuffer, (string*)_NOCARRIER_)) {
//                        debug("_NOCARRIER_");
////                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
//                        SetProcessState( (uint8_t *)&gprsState, connectionStatus);
//                    }
//                    else
//                    {
//                        debug("WAITING SRING:  ");
//                        debug(gprsBuffer);
//                        if(attempts>MAX_ATTEMPTS_NUMBER-1){
//                            SetProcessState( (uint8_t *)&gprsState,gprsReset );
//                            debug("Maximo numero de intentos alcanzado. Reiniciando...");
//                        }
//                    }   
//                }
//                else{
//                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
//                    debug("TIMEOUT MDM RESPONSE.");
//                }
			}
		break;		
		
        /*	Receive data  */
		case( receiveData ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: receiveData");
			if( sendCmd )
			{
                if(SendATCommand((string*)atcmd_sListenHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{       
               /* Aguarda por respuesta completa del modem */
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    /*Esto es para comparar solo con el encabezado de la trama
                     asi se evitan falsos positivos*/
                    const char ch = '\n';
                    char *ret;
                    
                    ret = findNthCharacterOcurrence(gprsBuffer,ch,1);
                    ret++;//La posicion siguiente a la segunda ocurrencia de \n
                    
//                    strncpy(header,++ret,sizeof(header));
                    strncpy(header,ret,FRAME_HEADER_SIZE-1);
                    headerIndex = getHeaderIndex(header);
                    
//                    printf("Rta SERVER: %s\n",gprsBuffer);
//                    printf("dataSecuence: %s\n",getFrameType(dataSecuence));                   
//                    printf("Header Rta SERVER: %s\n",header);
//                    printf("headerIndex: %d\n",headerIndex);
//                    printf("###########\n");
                    
                    switch(headerIndex){
                        case h024F:
//                            printDeviceSensorEnables();
                            //Corrijo la config de sensores y la fecha-hora 
                            setDeviceSensorEnables( ret + 6 ); //7
                            setDeviceDateTime( ret + 13 );    //14
//                            printf("Configuracion de sensores y RTCC actualizada.\n");
//                            printDeviceSensorEnables();
                            if(dataSecuence == registro){
                                registering = true;
                                registered = false;
                            }
                                
                            
                        //El siguiente codigo se ejecutara para 024F y 004F
                        case h004F:
                            if(dataSecuence == muestras) 
                                updateMemoryReadPointer();
                            else{
                                if(dataSecuence == configuracion){
                                    registered = true;
                                    registering = false;
                                }
                                    
                            }
                            dataSecuence = getNextDataSecuence(); 
                            break;
                        case h004E:
                        case h024E:
                            //Quiza sea innecesario...
//                            setServerFrame(dataSecuence,lastSample);
                            break;
                        default:
//                            printf("Respuesta del server desconocida.\nReiniciando FSM...");
                            debug("Respuesta del server desconocida.\nReiniciando FSM...");
                            SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }
                    //Si dataSecuence es diferente de cero, tengo algo por enviar
                    if(dataSecuence){
                        setServerFrame(dataSecuence,lastSample);
//                        printf("Building a %s frame.\n",getFrameType(dataSecuence));
                        debug("Building a frame.");
                        SetProcessState( (uint8_t *)&gprsState, socketSend);
                    }
                    else{
                        SetProcessState( (uint8_t *)&gprsState, closeSocket);
                        debug("No more frames. Closing socket...");
                    }      
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }  
            }
            break;
			
        /*	Close a Socket	
         * Cierra el puerto de comunicacion con el servidor.
         */
		case( closeSocket ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: closeSocket");
            if( sendCmd )
			{	
                if(SendATCommand((string*)atcmd_closeSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{           
				/* Clear the modemResponseNotification value before exiting. & Block indefinitely*/
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {                
                    if(strstr(gprsBuffer,_OK_))
                    {			
                        debug("_OK_");
    //					SetProcessState( (uint8_t *)&gprsState, configExtendSocket);
                        SetProcessState( (uint8_t *)&gprsState, waitForNewRequests);
                    }
                    else
                    {
                        debug("ALGO SALIO MAL");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }   
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
		break;
        
        case( waitForNewRequests):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: waitForNewRequests");
            /*Colocar mecanismo sleep para esperar notificaciones desde la tarea 
             sample y disponerse a enviar una trama*/
            
            sampleReadyNotification = ulTaskNotifyTake(   pdTRUE, portMAX_DELAY ); 
                
            if(sampleReadyNotification == NEW_SAMPLE_NOTIFICATION){
                SetProcessState( (uint8_t *)&gprsState, connectionStatus);
            }
            else{
//                  printf("ERROR: se esperaba %d como notificacion pero se recibio %zu.\n",NEW_SAMPLE_NOTIFICATION,sampleReadyNotification);
                    debug("ERROR: se recibio otra notificaci�n, no la que se esperaba (sample).");
            }
           
            break;
        
   
        /* En este estado verificamos conectividad del modem*/
        case( connectionStatus ):
//            printf("----------State: %s----------\n",getStateName(gprsState));
            debug("----------State: connectionStatus");
            //Hay que verificar si el equipo tiene si tengo ip
            if( sendCmd )
			{
                //Chequeo si tengo IP
                 if(SendATCommand((string*)atcmd_checkIP,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = false;
//                    TMR5_Start();
                }
                else{
//                  printf("ERROR no se pudo enviar el comando %s al modem.\n",atcmd_initialConfig);  
                    debug("ERROR no se pudo enviar el comando al modem.");
                }
			}
			else
			{
				//Espero que el modem termine de enviar su respuesta
                if(receiveATCommand(gprsBuffer, &attempts, responseDelay)) {
                    if(strlen(gprsBuffer)<30){ 
                    /*
                     * La respuesta en caso de tener IP es de la siguiente forma:
                     * \r\n#CGPADDR: 1,"10.215.134.177"\r\n\r\nOK\r\n
                     * Dependiendo la IP, el largo total var�a entre 31 y 39 caracteres
                     * En caso negativo la respuesta es:
                     * \r\n#CGPADDR: 1,""\r\n\r\nOK\r\n (Largo total 24)
                     */
                    //NO TIENE IP, ES DECIR NO ESTOY CONECTADO			
                        if( isThereSamplesToSend() ){
                            //NO ESTOY CONECTADO Y TENGO QUE MANDAR MUESTRAS, ACTIVO CONTEXTO
//                            printf("Sin IP y nueva muestra por enviar...\n");
                            debug("Sin IP y nueva muestra por enviar...");
                            dataSecuence = muestras;
                            setServerFrame(dataSecuence,lastSample);
                            SetProcessState( (uint8_t *)&gprsState, activateContext);	
                        }
                        else{
//                            printf("connectionStatus sin IP y sin muestras pendientes\n");
                            debug("connectionStatus sin IP y sin muestras pendientes.");
                            SetProcessState( (uint8_t *)&gprsState, waitForNewRequests);
                        }
                    }
                    else if(strstr(gprsBuffer,"CGPADDR") && strlen(gprsBuffer)>30){
                    //TENGO IP, ESTOY CONECTADO
                        if( isThereSamplesToSend() ) {	
                        //TENGO MUESTRAS PARA ENVIAR
//                            printf("Con IP y nueva muestra por enviar...\n");
                            debug("Con IP y nueva muestra por enviar...");
                            dataSecuence = muestras;
                            setServerFrame(dataSecuence,lastSample);
                            SetProcessState( (uint8_t *)&gprsState, socketDial);
                        }
                        else {
                        //NO TENGO MUESTRAS PARA ENVIAR
//                            printf("connectionStatus con IP sin muestras para enviar \n");
                            debug("connectionStatus con IP sin muestras para enviar.");                            
                            SetProcessState( (uint8_t *)&gprsState,waitForNewRequests);
                        }
                    }   
                    else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) ) {
//                        printf("connectionStatus NOCARRIER \n");
                        debug("connectionStatus NOCARRIER.");
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }
                    else{
                        debug("Estado de conexion indeterminado");
                        debug(gprsBuffer);
                        SetProcessState( (uint8_t *)&gprsState, gprsReset);
                    }
                }
                else{
                    //printf("TIMEOUT MDM RESPONSE. State:%s\n ",getStateName(gprsState));
                    debug("TIMEOUT MDM RESPONSE.");
                }
			}
			break;
    }
    
    return	true;
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
 * @param whichSample	�ltima muestra disponible (lastSample) o las siguientes (nextSample) 
 * @return				true en caso de exito o false si hubo alg�n problema o no hay muestras para enviar
 */
char	setServerFrame( uint8_t frameType, uint8_t whichSample )
{
	//Clean buffer
    cleanDataGPRSBuffer();
	
	muestra_t muestraAlmacenada;
    trama_muestra_t tramaMuestra;
	trama_config_t configFrame;
	trama_inicio_t tramaInicio;

	extern rtcc_t tiempo;

	switch( frameType )
	{
		case( muestras ):
			//	levanta la proxima muestra para formatear a la trama de envio al servidor
			if( getSample( &muestraAlmacenada, whichSample ) == false ){
                return	false;	//	no hay mas muestras para enviar
            }
            else{
                prepareSample(&tramaMuestra, &muestraAlmacenada);
                buildHexFrame((char*)&tramaMuestra,tramaGPRS, sizeof(trama_muestra_t)+1);
                /*Tenemos 1 byte de padding que eliminar*/
                removePaddingBytes(tramaGPRS,42,sizeof(trama_muestra_t)*2);
            }
            break;

		case( configuracion ): //Revisar el struct estacion y cuando se inicia
			configFrame.cmd = frameType;
			configFrame.tipo = estacion.tipo;
			configFrame.num_serie = swapBytes( estacion.num_serie );
			configFrame.reservado1 = 0x0101;                            //	Numero de grupo
			configFrame.reservado2 = 0xFFFF;                            //	Libre
			configFrame.capMemoria = swapBytes( (uint16)(MAX_SAMPLES) );//	Capacidad de memoria
			configFrame.tiempoProm = 0x00;                              //	Tiempo Promedio
			configFrame.periodo = 0x0A;                                 //	Periodo
            
			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_1, &configFrame.sensorHab1 );
			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_2, &configFrame.sensorHab2 );
			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_3, &configFrame.sensorHab3 );
            
//            printf("Building config frame\n");
            debug("Building config frame");
//            printDeviceSensorEnables();
            
//            configFrame.cmd = 0x11;
//			configFrame.tipo = 0x11;
//			configFrame.num_serie = 0x1111;
//			configFrame.reservado1 = 0x1111;                            //	Numero de grupo
//			configFrame.reservado2 = 0x1111;
//			configFrame.capMemoria = 0x1111;                            //	Capacidad de memoria
//			configFrame.tiempoProm = 0x11;                              //	Tiempo Promedio
//			configFrame.periodo = 0x11;                                 //	Periodo    
//			configFrame.sensorHab1 = 0x11;
//			configFrame.sensorHab2 = 0x11;
//			configFrame.sensorHab3 = 0x22;

			buildHexFrame((char*)&configFrame,tramaGPRS,sizeof(trama_config_t));
            /*Hay que eliminar un byte de padding*/
//            removePaddingBytes(tramaGPRS, 30, sizeof(trama_config_t)*2);
            
            //Env�o hardcodeado una trama de registro
//            strcpy(tramaGPRS,atcmd_FRAME_1);
			break;

		case( registro ):
			tramaInicio.cmd = frameType;
			tramaInicio.tipo = estacion.tipo;
			tramaInicio.num_serie = swapBytes( estacion.num_serie );
			tramaInicio.hora = bcd2dec( tiempo.hora );
			tramaInicio.min = bcd2dec( tiempo.minutos );
			tramaInicio.dia = bcd2dec( tiempo.dia );
			tramaInicio.mes = bcd2dec( tiempo.mes );
			tramaInicio.anio = bcd2dec( tiempo.anio );
			tramaInicio.alarmaMuestras = 0;
			tramaInicio.tiempoMedicion = 0;
			tramaInicio.inicioMuestras = bcd2dec( estacion.InicioMuestras );
			tramaInicio.pEscritura = swapBytes( getSamplesWrite() );
			tramaInicio.pLectura = swapBytes( getSamplesRead() );
			tramaInicio.dAlmacenados = swapBytes( getSamplesTotal() );
			tramaInicio.pMaxMin = 0;
			tramaInicio.periodo = 0x0A;
            
            MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_1, &tramaInicio.sensorHab1 );
			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_2, &tramaInicio.sensorHab2 );
			MCHP_24LCxxx_Read_byte( _24LC512_0, INT_ENABLE_SENSOR_3, &tramaInicio.sensorHab3 );
            
            buildHexFrame((char*)&tramaInicio, tramaGPRS, sizeof(trama_inicio_t));
            
            /*En el caso de esta trama, tenemos 2 bytes de padding que eliminar*/
            removePaddingBytes(tramaGPRS,26,sizeof(trama_inicio_t)*2);
//            removePaddingBytes(tramaGPRS,48,sizeof(trama_inicio_t)*2);
            
            //Env�o hardcodeado una trama de configuracion
//            strcpy(tramaGPRS,atcmd_FRAME_3);
			break;

		default:
		{
			return	false;
		}
	}
//    printf("tramaGPRS:\n%s(%s)\n",tramaGPRS,getFrameType(frameType));
	return	true;
}

void prepareSample(trama_muestra_t *tramaMuestra, muestra_t *muestraAlmacenada)
{
    //	copia los datos a la trama - estacion
    tramaMuestra->cmd = muestras;
    tramaMuestra->tipo = estacion.tipo; //configDevice.type = 3;
    tramaMuestra->num_serie = swapBytes( estacion.num_serie ); //configDevice.serial = 9998;
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
}

uint8_t buildHexFrame(char *trama, char *tramaHex, uint8 frameSize)
{
    uint8_t n,k; 
    char *p = NULL; char *t = NULL;
//    const char string_cierre[] = "\x001A";
    const char string_cierre[] = "";

    n = 0;
	t = tramaHex;
    p = trama;
    
    while( n < frameSize-1 ){ 
        sprintf( (char*)t + (2*n), (const char*)"%02X", *(p+n));
        n++;
//        printf("n:%d | (char*)t+(2*n):%s\n",n,(char*)t + (2*n));
    }
    strcat( (char*)tramaHex, (char*)&string_cierre );
//    printf("n*2:%d | (char*)t+(2*(n-1)):%s\n",n*2,(char*)t+(2*(n-1)));

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

uint8    getNextDataSecuence()
{
    
    /*Si la trama anterior fue una muestra o configuracion,
    y si tengo muestras pendientes de enviar*/
    if(isThereSamplesToSend() && !registerInProcess()) {
        return muestras;
//        setServerFrame(dataSecuence,nextSample);
//        SetProcessState( (uint8_t *)&gprsState, socketSend);
//        debug("Setting next data frame to send.");
    }
    else{ 
        /*Si no tengo muestras pendientes de enviar y no estoy registrado*/
        if(registerInProcess()){
            //No se registr�. Procede a registrarse.
            return configuracion;
//            setServerFrame(dataSecuence,nextSample);
//            debug("Sending config frame.");
//            SetProcessState( (uint8_t *)&gprsState, socketSend);
        }
        /*Si estoy registrado y la muestra anterior fue 'registro'*/
        else if(!isRegistered()){
            return registro;
            
            //La trama anterior fue de registro, ahora tiene que mandar de configuracion
            //Notifico a Sample que ya tengo la hora del servidor
//            xTaskNotify(xGprsHandle, SYNC_SERVER_TIME_NOTIFICATION,eSetValueWithOverwrite);
            
//            registered = true; /* Esto tendria que hacerlo cuando reciba 
//                                confirmacion del server de haber recibido
//                                la trama de configuracion*/
//            registering = false;
//            dataSecuence = configuracion;
//            setServerFrame(dataSecuence,nextSample);
//            SetProcessState( (uint8_t *)&gprsState, socketSend);
//            debug("Frame registro sended. Sending configuration frame.");
        }
        else{
            //No hay muestras pendientes por enviar
//            SetProcessState( (uint8_t *)&gprsState, closeSocket);
//            debug("No more frames. Closing socket...");
            return 0;
        }
    }
    
}

char isRegistered()
{
    return registered;
}

char registerInProcess()
{
    return registering;
}

void removePaddingBytes(char *tramaGPRS,uint8 paddingPosition, uint8 frameSize)
{
    int k;
    //Para eliminar el byte de padding generado al comienzo del struct viento
    for(k=paddingPosition; k<frameSize; k++){
        tramaGPRS[k]=tramaGPRS[k+2];
    }    
}

void cleanBufferTail(uint8 frameSize)
{
    int k;
    //Para eliminar el residuo al final de la trama
    for(k=frameSize; k<GPRS_BUFFER_SIZE; k++){
        tramaGPRS[k]=NULL;
    }
}

const char* getFrameType(uint8 frameType)
{
    switch(frameType){
        case muestras: return "MUESTRAS";
        case registro: return "REGISTRO";
        case configuracion: return "CONFIGURACION";
        default: 
//            printf("Unexpected frameType:%d",frameType);
            return "WRONG_FRAME_TYPE";
    }
}

headerOptions_t getHeaderIndex(const char* header)
{    
    if (strcmp(header, "024F") == 0) 
    {
        return h024F;
    } 
    else if (strcmp(header, "004F") == 0)
    {
      return h004F;
    }
    else if (strcmp(header, "004E") == 0)
    {
      return h004E;
    }
    else if (strcmp(header, "024E") == 0)
    {
      return h024E;
    }
    /* more else if clauses */
    else /* default: */
    {
        return 0;
    }   
}