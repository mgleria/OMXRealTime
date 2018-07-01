/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */
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

//Buffer de comunicación entrante y saliente con el modem
static char gprsBuffer[GPRS_BUFFER_SIZE]={0};
//Buffer donde se almacena la trama de datos generada para enviar al modem
static char tramaGPRS[GPRS_BUFFER_SIZE] = {0};

static char registered = FALSE;
static uint8_t dataSecuence = registro;
static uint8_t sampleToSend = lastSample;

static uint32_t notification;
//Handle referenciado en tmr4.c para uso de xTaskNotify()
TaskHandle_t xGprsHandle;

TickType_t responseDelay, modemResetTime;

uint8_t sendCmd = TRUE;

void startGprsTask(){
    
    xTaskCreate(    vTaskGPRS,
                    "vTaskGprs",
                    1000,
                    NULL,
                    MAX_PRIORITY-1, //ACOMODAR prioridades
                    &xGprsHandle);
    
    notification = 0;
  
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
    
    uint16_t contador = 0;
    debugUART1("Initial section GPRS Task\r\n");
    
    
//    SetProcessState(&gprsState,gprsReset);
    
    for(;;)
    {   
        FSM_GprsTask();
        vTaskDelay(taskDelay);  
    }
}

void SetProcessState( uint8_t * reg, uint8_t state )
{
	*reg = state;
	sendCmd = TRUE;
}

uint8_t	FSM_GprsTask( )
{
	/*	estados del proceso	*/
	switch ( gprsState )
	{
	/*	GprsReset: deshabilitaciï¿½n ECHO	*/
		case( gprsReset ):		
			if( sendCmd )
			{
                 debugUART1("GprsReset:\r\n");
                 //Reseteo el modem y espero MODEM_RESET_TIME_S para encenderlo
                 _LATB12 = 0;
                 debugUART1("Modem OFF\r\n");
                 vTaskDelay(modemResetTime);
                 _LATB12 = 1;
                 debugUART1("Modem ON. Waiting for complete boot...\r\n");
                 vTaskDelay(modemResetTime*2);
                 debugUART1("Modem boot complete\r\n");

                registered = FALSE;
                dataSecuence = registro;
                
                //Si hay muestras sin enviar, armo las tramas y postergo el registro
                if( setServerFrame( muestras, lastSample ) )
                    dataSecuence = muestras;

                if(SendATCommand((string*)atcmd_disableEcho,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_disableEcho);                    
			}
			else
			{
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,initModem );
                    }
                    else
                    {
                        debugUART1("NOT OK: ");
                        debugUART1(gprsBuffer);
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        case( initModem ):
			
			if( sendCmd )
			{
                debugUART1("initModem:\r\n"); //"AT#STIA=0\r\n"               
                if(SendATCommand((string*)atcmd_STN_OFF,gprsBuffer,gprsBuffer,10,0,1)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_STN_OFF); 
			}
			else
			{
				notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
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
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1("ALGO SALIO MAL: ");
                        debugUART1(gprsBuffer);
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
            
        case( setContext ):
			if( sendCmd )
			{
                debugUART1("setContext:\r\n");
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
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,configSocket );                 
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1(gprsBuffer);
                    }
                    
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        /*	Configuracion de Socket	*/
		case( configSocket ):
			if( sendCmd )
			{
                debugUART1("configSocket:\r\n");			
				if(SendATCommand((string*)atcmd_configSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_configSocketHARDCODED);
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,configExtendSocket );                 
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1(gprsBuffer);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        /*	Configuracion de Socket	*/
		case( configExtendSocket ):
			if( sendCmd )
			{	
                debugUART1("configExtendSocket:\r\n");
                if(SendATCommand((string*)atcmd_configExtendSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_configExtendSocketHARDCODED);
                
			}
			else
			{
               /* Aguarda por respuesta completa del modem */
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
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
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        case( activateContext ):
			if( sendCmd )
			{
				debugUART1("activateContext:\r\n");
                if(SendATCommand((string*)atcmd_activateContextHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                    else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_activateContextHARDCODED); 
                }
			else
			{
                /* Aguarda por respuesta completa del modem */
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    
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
                    }
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
        
        case( socketDial ):
			if( sendCmd )
			{	
                debugUART1("socketDial:\r\n");
                if(SendATCommand((string*)atcmd_socketDialHARDCODED_1,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_socketDialHARDCODED_1);
			}
			else
			{           
				/* Clear the notification value before exiting. & Block indefinitely*/
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,_OK_))
                    {
                        debugUART1("_OK_\n\r");
                        SetProcessState( &gprsState,socketSend );                 
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1(gprsBuffer);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
			break;
            
        case( socketSend ):
			if( sendCmd )
			{	
                debugUART1("socketSend:\r\n");
                if(SendATCommand((string*)atcmd_socketSend,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_socketSend);
			}
			else
			{   
               /* Aguarda por respuesta completa del modem */
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    if(strstr(gprsBuffer,">"))
                    {
                        debugUART1("> RECIBIDO\n\r");
                        SetProcessState( &gprsState,putData );                 
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
                        debugUART1(gprsBuffer);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        case( putData ):
			if( sendCmd )
			{
                debugUART1("putData:\r\n");

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
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
                    if(strstr(gprsBuffer,"SRING"))
                    {		
                        debugUART1("SRING Recibido\r\n");
                        SetProcessState( &gprsState, receiveData);
                    }
                    else
                    {
                        //strcat(gprsBuffer,"_ERROR_\n\r");
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
			if( sendCmd )
			{
                debugUART1("receiveData:\r\n");
                if(SendATCommand((string*)atcmd_sListenHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_sListenHARDCODED);
			}
			else
			{
                
               /* Aguarda por respuesta completa del modem */
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    debugUART1("Rta SERVER: ");
                    debugUART1(gprsBuffer);
                    debugUART1("\r\n");
                    
                    char *p;
                    
                    if(p = strstr(gprsBuffer,"024F"))
                    {   
                        debugUART1("Rta SERVER: 024F\r\n");
                        //Corrijo la config de sensores y la fecha-hora 
                        setDeviceSensorEnables( p + 6 ); //7
						setDeviceDateTime( p + 13 );    //14
                        debugUART1("Configuracion de sensores y RTCC actualizada.\r\n");
                        processServerResponse();                                           
                    }
                    else if(strstr(gprsBuffer,"004F"))
                    {
                        debugUART1("Rta SERVER: 004F\r\n");
                        processServerResponse();  
                    }
                    else{
                        debugUART1("Respuesta del server desconocida\r\n");
                        debugUART1(gprsBuffer);
                    }
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
            if( sendCmd )
			{	
                debugUART1("closeSocket:\r\n");
                if(SendATCommand((string*)atcmd_closeSocketHARDCODED,gprsBuffer,gprsBuffer,10,0,2)>0){
                    /*	modo recepcion para espera de la respuesta	*/
                    sendCmd = FALSE;
                }
                else printf("ERROR no se pudo enviar el comando %s al modem.\r\n",atcmd_closeSocketHARDCODED);
			}
			else
			{           
				/* Clear the notification value before exiting. & Block indefinitely*/
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);                 
                    if(strstr(gprsBuffer,_OK_))
                    {			
                        debugUART1("_OK_\n\r");
    //					SetProcessState( &gprsState, configExtendSocket);
                        SetProcessState( &gprsState, connectionStatus);
                    }
                    else
                    {
                        debugUART1("ERROR\n\r");
                        debugUART1(gprsBuffer);
                    }   
                }
                else{
                    printf("TIMEOUT MDM RESPONSE. State:%s ",getStateName(gprsState));
                }
			}
		break;
        
        /* En este estado verificamos conectividad del modem*/
        case( connectionStatus ):
			/*Colocar mecanismo sleep para esperar notificaciones desde la tarea 
             sample y disponerse a enviar una trama*/
            
            //Hay que verificar si el equipo tiene si tengo ip
            if( sendCmd )
			{
				debugUART1("connectionStatus:\r\n");
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
                notification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
                
                if(notification == MDM_RESP_READY_NOTIFICATION){
                    UART2_ReadBuffer(gprsBuffer, GPRS_BUFFER_SIZE);
                    //Si tengo IP, la respuesta es de la forma ?xxx.yyy.zzz.www?
                    if((!(strstr(gprsBuffer,"AT")))&&(strlen(gprsBuffer)<40)){    
                    //NO TIENE IP, ES DECIR NO ESTOY CONECTADO			
                        if( setServerFrame( dataSecuence, lastSample ) ){
                            //NO ESTOY CONECTADO Y TENGO QUE MANDAR MUESTRAS, MANDO A DISCAR.
                            SetProcessState( &gprsState, configExtendSocket);	
                        }
                        else{
                            SetProcessState( &gprsState, connectionStatus);
                        }
                    }
                    else if(strlen(gprsBuffer)>40){
                    //TENGO IP, ESTOY CONECTADO
                        if( setServerFrame( dataSecuence, lastSample ) ) {	
                        //TENGO MUESTRAS PARA ENVIAR
                            SetProcessState( &gprsState, putData);	
                        }
                        else {
                        //NO TENGO MUESTRAS PARA ENVIAR
                            SetProcessState( &gprsState,connectionStatus);
                        }
                    }   
                    else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) ) {
                        SetProcessState( &gprsState, gprsReset);
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
//	trama_config_t configFrame;
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

		case( configuracion ):
//			configFrame.cmd = frameType;
//			configFrame.tipo = estacion.tipo;
//			configFrame.num_serie = swapBytes( estacion.num_serie );
//			configFrame.reservado1 = 0x0101;						//	Numero de grupo
//			configFrame.reservado2 = 0xFFFF;						//	Libre
//			configFrame.capMemoria = swapBytes( (uint16)(MAX_SAMPLES) );			//	Capacidad de memoria
//			configFrame.tiempoProm = 0x00;							//	Tiempo Promedio
//			configFrame.periodo = 0x0A;								//	Periodo
//			i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_1, &configFrame.sensorHab1, sizeof(configFrame.sensorHab1) );
//			i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_2, &configFrame.sensorHab2, sizeof(configFrame.sensorHab2) );
//			i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_3, &configFrame.sensorHab3, sizeof(configFrame.sensorHab3) );
//			configFrame.nullE = NULL;								//	para indicar fin de trama

//			strncpy( (char*)&tramaGPRS, (char*)&string_cabecera, strlen((char*)&string_cabecera) );
//			n = 0;	m = (char*)&configFrame;
//			while( n < (sizeof(trama_config_t)-1) )
//				sprintf( (char*)t + (2*n++), (const char*)"%02X", (*m++) );
//			strcat( (char*)tramaGPRS, (char*)&string_cierre );
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
        case noCommand: return "noCommand" ;
        case gprsReset: return "gprsReset" ;
        case initModem: return "initModem" ;
        case getModemID: return "getModemID" ;
        case getMSN: return "getMSN" ;
        case getIMSI: return "getIMSI" ;
        case getIMEI: return "getIMEI" ;
        case getSignal: return "getSignal" ;
        case connectionStatus: return "connectionStatus" ;
        case setContext: return "setContext" ;
        case activateContext: return "activateContext" ;
        case configSocket: return "configSocket" ;
        case configExtendSocket: return "configExtendSocket" ;
        case socketDial: return "socketDial" ;
        case socketSend: return "socketSend" ;
        case socketStatus: return "socketStatus" ;
        case closeSocket: return "closeSocket" ;
        case sendData: return "sendData" ;
        case putData: return "putData" ;
        case receiveData: return "receiveData" ;
        case gprsWaitReset: return "gprsWaitReset" ;
        case finalStateToggleLed: return "finalStateToggleLed" ;
        case STN_OFF: return "STN_OFF" ;
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
    if((dataSecuence == muestras 
    || dataSecuence == configuracion)
    && isThereSamplesToSend()) {
        dataSecuence = muestras;
        setServerFrame(dataSecuence,nextSample);
        SetProcessState(&gprsState, socketSend);
        debugUART1("Setting next frame to send.\r\n");
    }
    else if(dataSecuence == registro){
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