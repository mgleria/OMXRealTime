
/*	application includes	*/
#include	"procesos/gprs_process.h"
#include	"funciones/memory.h"
#include	"funciones/simINFO.h"
#include	"funciones/ConfCommand.h"
#include	"funciones/puts.usb.h"





/*	standard includes	*/
#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>

#ifdef	USE_GPRS

/*	at commands includes	*/
#include	"drivers/at_cmds.h"
#include	"procesos/cmd_process.h"

//******************************************************************************
/*	Definicion de variables	locales */
char gprsBuffer[Buffer_GPRS_Size];
//char gprsBufferRx[Buffer_GPRS_Size];
static char ATcmd[96] = {0};
char temporal[Buffer_GPRS_Size] = {0};

/*	variable con informacion del equipo, declarada en omx_s.c	*/
extern estacion_t	estacion;

/*	Flag para inhibir la ejecución del sms process */
bit	smsInhibit = FALSE;

/*	variable con informacion de la SIM, declarada en simCard.c	*/
extern simInfo_t	simInfo;

/*	variable con informacion del modem, declarada en gprs_functions.h	*/
extern modem_t		modemInfo;

/*	intentos de conexion al servidor	*/
static uint8 connexionAttempts = 0;

/*	secuencia de conexion con el servidor	*/
static uint8 dataSecuence = registro;

/*	muestra que se pide al enviar los datos al stack del modem	*/
static uint8 sample = lastSample;

/*	indica si se envia o recibe una trama	*/
static char sendCmd = TRUE;

/*	bit para indicar mientras el equipo se registra	*/
static bit Register = FALSE;

/*	variable de estado del proceso	*/
//static uint8 gprsState = 2, prev_gprsState = 0xFF;	//	comienza por configurar el gprs

/*	variable para controlar la cantidad de timeouts recibidos*/
uint8		TimeOuts=0;

/**********************************************************************************************/
/*	Funciones prototipo locales	*/


/**********************************************************************************************/
/*	***	Macros ***	*/
/**	Cambia el estado de la maquina del proceso	*/
//#define	SetProcessState( &gprsState, state)		{	putsProcessState( GPRS_ID, "gprsWait");	gprsState = state;	sendCmd = TRUE;	}

/**	macro para calular tiempo en milisegundos	*/
#define	msec(m)		(uint16)((uint16)m * GPRS_PROCESS_TICK / 10)
/**	macro para calcular tiempo en segundos	*/
#define	sec(s)		(uint16)((uint16)s * (1/GPRS_PROCESS_TICK))

/**	macro para configurar un tiempo de demora de la tarea/proceso	*/
#define	SetProcessDelay(d)		(processDelay = d)
/**	macro para obtener el tiempo restante de demora de la tarea/proceso	*/
#define	GetProcessDelay()		(processDelay)

/**********************************************************************************************/
/*	DEFINICION DE TEXTOS CONSTANTES	*/
#define	_MAX_CONNEXION_ATTEMPTS_	32

#define	_MAX_TIMEOUTS_	8



/**********************************************************************************************/
/**
 * Funcion para cambiar el estado del proceso.
 * 
 */
void SetProcessState( uint8 * reg, uint8 state )
{
	putsProcessState( GPRS_ID, "gprsWait");
	*reg = state;
	sendCmd = TRUE;
}


/**********************************************************************************************/
/**
 * Función para controlar la cantidad de timeouts o errores consecutivos
 * 
 */
void TimeOutCounter(void)
{
	TimeOuts++;
//	if(TimeOuts>=_MAX_TIMEOUTS_)
//		  RESET(); //RESET POR SOFTWARE
//        SetProcessState( &gprsState,  gprsReset);
}
/**********************************************************************************************/
/**
 * \brief
 * Proceso GPRS.\n
 * Se encarga del manejo del gprs.
 * @return	FALSE si el proceso esta corriendo, TRUE caso de solicitar un reinicio de equipo.
 */

uint8	gprsProcess( void )
{
	
	/*	enumeracion de los posibles estados en el manejo del gprs	*/
	static enum
	{
		noCommand = 0,				//!	no se ejecuta ningun comando
		hardReset,					//!	reinicio por hardware
		gprsReset,					//!	reinicio del proceso GPRS
		initModem,					//!	inicializacion del modem
		getModemID,					//!	consulta identificacion de la revision del modem
		getMSN,						//!	consulta en numero de serie del fabricante
		getIMSI,					//!	consulta el numero de identificacion del abonado
		getIMEI,					//!	consulta el numero de identificacion serie del modem
		getSignal,					//!	consulta el nivel de señal
		ipAddress,					//!	consulta la direccion de IP al modem
		setContext,					//!	configura el contexto
        activateContext,			//!	Activa el contexto previamente configurado
        configSocket,               //!	Configurar el socket
        configExtendSocket,         //!	Configuración extendida el socket
        socketDial,                 //!	Apertura del socket
		socketStatus,               //!Nuevo estado para saber el estado del socket
		closeSocket,				//!	cierra el puerto de conexion con la red
		sendData,					//!	envia los datos almacenados usando el protocolo configurado
		putData,					//!	coloca los datos dentro del protocolo
        receiveData,				//Recibe datos desde el servido
		gprsWaitReset,				//!	estado para esperar el reinicio del equipo
	}gprsState = gprsReset, prev_gprsState = 0xFF;

	#define	TIMEOUT	((uint16)(5.0/GPRS_PROCESS_TICK))
	static uint16 delay = TIMEOUT;
	auto uint8 n = 0;
	static uint16 processDelay = 0;

	/*	demora del proceso	*/
	if( GetProcessDelay() )
	{
		SetProcessDelay( GetProcessDelay() - 1 );
		return	FALSE;
	}

	/* Controla que el proceso evolucione,con es decir, que no se encuentre
	 * en un estado por mas de cierto tiempo.
	 */
	#define	PROCESS_TIMEOUT	(uint16)(1.0*60.0/GPRS_PROCESS_TICK)
	static uint16 processTimeout = 0;	/*	tiempo maximo para que el proceso permanezca en el mismo estado	*/
	if( prev_gprsState != gprsState )
	{
		processTimeout = 0;
		prev_gprsState = gprsState;
	}
	if( (processTimeout++) >= PROCESS_TIMEOUT )
	{
		processTimeout = 0;
		SetProcessState( &gprsState, gprsWaitReset);
		putsProcessState( GPRS_ID, "Reset request" );
		return	GPRS_APPLICATION_RESET;
	}
	#undef	PROCESS_TIMEOUT

	/*	estados del proceso	*/
	switch ( gprsState )
	{
		case( noCommand ):
			putsProcessState( GPRS_ID, "noCommand" );
			break;

		case( gprsReset ):
			putsProcessState( GPRS_ID, "gprsReset" );
			//	borra toda la informacion del modem para reinicializarlo
			clearInfo( modemInfo.fsn, sizeof(modemInfo.fsn) );
			clearInfo( modemInfo.fw_version, sizeof(modemInfo.fw_version) );
			clearInfo( modemInfo.imei, sizeof(modemInfo.imei) );
			clearInfo( modemInfo.imsi, sizeof(modemInfo.imsi) );
			SetProcessState( &gprsState, getModemID);
            smsInhibit = FALSE;
            Register = FALSE;
			dataSecuence = registro;
			if( getServerFrame( muestras, lastSample ) )
				dataSecuence = muestras;
            SetMdmState(no_mdm);
			//ClearSysState( connected | connecting );
			//SetSysState( noConnected );
			break;

		/*	Request Revision	*/
		case( getModemID ):
			
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "getModemID" );
				SendATCommand((string*)atcmd_getModemID,gprsBuffer,gprsBuffer,10,0,2);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				//YA QUE LA RESPUESTA ES POR EJEMPLO<CR><LF>10.00.146<CR><LF>OK<CR><LF>
				//VERIFICAMOS SI EL OK ESTA EN EL STRING RECIBIDO
				if( strstr( (char*)gprsBuffer, _OK_ ))
				{
					saveModemID( (char*)gprsBuffer);
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					#ifndef	__DEBBUG
					usr_stdout = _UART2_;
					printf( (string*)"--m/fv:%s\n", (char*)fgetModemID() );
					#endif
					SetProcessState( &gprsState, getIMSI);	
                    SetMdmState(no_IP);
				}			
				
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					//TimeOutCounter();
					SetProcessState( &gprsState,  gprsReset);
                    
				}
                
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, getModemID);
                    TimeOutCounter();
				}
				
			}
			break;

			/*	Request IMSI	*/
		case( getIMSI ):
			
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "getIMSI" );
				SendATCommand((string*)atcmd_getIMSI,gprsBuffer,gprsBuffer,10,0,2);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				
				if( strstr( (char*)gprsBuffer, _OK_ ) )
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					/*	copia el numero de IMSI */
					if( saveIMSI((char*)gprsBuffer) )
					{
						SetProcessState( &gprsState, getIMEI);
					}
					else
					{
						SetProcessState( &gprsState, gprsReset);
						SetProcessDelay(sec(50));
					}
					#ifndef	__DEBBUG
					usr_stdout = _UART2_;
					printf((string*)"--m/imsi:%s\n", (char*)fgetIMSI());
					#endif
				}
				
                else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					//TimeOutCounter();
					SetProcessState( &gprsState,  gprsReset);
                    SetProcessDelay(sec(0));

				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, getIMSI);
                    TimeOutCounter();
				}				
			}
			break;

			/*	Request Product Serial Number Identification - IMEI	*/
		case( getIMEI ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "getIMEI" );
				SendATCommand((string*)atcmd_getIMEI,gprsBuffer,gprsBuffer,10,0,2);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				if( strstr( (char*)gprsBuffer, _OK_))
				{
					/*	copia el numero de Product Serial Number Identification */
					saveIMEI( (char*)gprsBuffer );
					#ifndef	__DEBBUG
					usr_stdout = _UART2_;
					printf( (string*)"--m/imei:%s\n", (char*)fgetIMEI() );
					#endif
					
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, getSignal);
				}
								
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					//TimeOutCounter();
					SetProcessState( &gprsState,  gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState,  gprsReset);
				}
                
                else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, getIMEI);
                    TimeOutCounter();
				}

			}
			break;

			/*	Read Factory Serial Number	*/
            /*CAMBIO: El comando AT ha sido cambiado*/
		case( getMSN ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "getMSN" );
				SendATCommand((string*)atcmd_getMSN,gprsBuffer,gprsBuffer,10,0,2);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				if( strstr( (char*)gprsBuffer, (string*)atresp_getMSN ) )
				{
					/*	copia el numero de Factory Serial Number */
					saveFSN( (char*)gprsBuffer );
					#ifndef	__DEBBUG
					usr_stdout = _UART2_;
					printf( "--m/fsn:%s\n", fgetFSN() );
					#endif
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, getSignal);
				}

				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					//TimeOutCounter();
					SetProcessState( &gprsState,  gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState,  gprsReset);
				}
                else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, getMSN);
                    TimeOutCounter();
				}
			}
			break;
			
		

			/*	Define PDP Context	*/
		case( setContext ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "setContext" );
				sprintf(ATcmd,atcmd_setContext,getAPN());
			
				  SendATCommand((string*)ATcmd, gprsBuffer,gprsBuffer,10, 0, 1);
			
			
				sendCmd = FALSE;
			}
			else
			{					
				if( strstr( (char*)gprsBuffer,_OK_ ) )
				{
					TimeOuts=0;
					SetProcessState( &gprsState, activateContext);	
				}
                
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					//TimeOutCounter();
					SetProcessState( &gprsState,  gprsReset);
				}
                
                else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, gprsReset);	
                    TimeOutCounter();
				}
					
			}
			break;
    
			/*	Active Context	*/
		case( activateContext ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "activateContext" );
				
                 /*CAMBIO: Definimos el AT set_context que no estaba en la lista*/
				SendATCommand((string*)atcmd_activateContext, gprsBuffer,gprsBuffer,10, 1, 2); 
				
			
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{

                if( strstr( (char*)gprsBuffer, (char*) _OK_ ) )
				{
                    /*COMO NOS DEVUELVE LA DIRECCIÓN IP, DEBEMOS TOMARLA Y ALMACERNARLA*/
			
                    saveIPaddress( (char*)gprsBuffer );
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, configSocket);
                    SetMdmState(IP_ok);
				}
				
                else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					SetProcessState( &gprsState, activateContext);
                    TimeOutCounter();
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{	
					SetProcessState( &gprsState, gprsReset);
				}
                
                else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
				    SetProcessState( &gprsState, activateContext);
                    TimeOutCounter();
				}
				

			}
			break;
			
		/*	Configuración de Socket	*/
		case( configSocket ):
			if( sendCmd )
			{
				
				putsProcessState( GPRS_ID, "configSocket" );
				SendATCommand((string*)atcmd_configSocket, gprsBuffer,gprsBuffer,10, 0, 1 );
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			
			}
			else
			{
                      
				if( strstr( (char*)gprsBuffer, (char*)_OK_ ) )
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
                    /*ESTÁ TODO BIEN, VAMOS AL PRÓXIMO ESTADO*/
					SetProcessState( &gprsState, configExtendSocket);
				}
				            
                else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
				    SetProcessState( &gprsState,  gprsReset);
				}	
			}
			break;
            
			/*	Configuración de Socket	*/
		case( configExtendSocket ):
			if( sendCmd )
			{	
				putsProcessState( GPRS_ID, "configExtendSocket" );   

				SendATCommand((string*)atcmd_configExtendSocket, gprsBuffer,gprsBuffer,10, 0, 1 );
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				if( strstr( (char*)gprsBuffer, _OK_ ) )
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, socketDial);
				}
							
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
				//	TimeOutCounter();
					SetProcessState( &gprsState, gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{	
					SetProcessState( &gprsState, gprsReset);
				}
                                
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
                    SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
				}
			}
			break;
			
       
			/*	Dial Socket	*/
		case( socketDial ):
			if( sendCmd )
			{	
				putsProcessState( GPRS_ID, "socketDial" );
				static char ATcmd[60] = {0};
				sprintf( ATcmd, atcmd_socketDial, getServerPort() , getServerDNS());
                 
				SendATCommand((string*)ATcmd, gprsBuffer,gprsBuffer,50, 0, 1);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{           
				if( strstr( (char*)gprsBuffer, (char*)_OK_ ) )
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
                    /*ESTÁ TODO BIEN, VAMOS AL PRÓXIMO ESTADO*/
					SetProcessState( &gprsState,putData );
                    SetMdmState(srv_reach);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
				//	TimeOutCounter();
					SetProcessState( &gprsState, gprsReset);
				}
                
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
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
				putsProcessState( GPRS_ID, "putData" );
				auto char *ATframe = NULL;
				/*	si hay algo para enviar al servidor ... */
			
				if( ATframe = getServerFrame( dataSecuence, lastSample ) )
				{
					//SetSysState(connecting);
					SendATCommand( (string*)atcmd_sendData, gprsBuffer,gprsBuffer,20, 0,  1); //originalmente recive una sola rta
					/*	modo recepcion para espera de la respuesta	*/
					sendCmd = FALSE;
					strcpy(temporal,ATframe);
                    smsInhibit = TRUE;        
				}
				else
				{
					SetProcessState( &gprsState, closeSocket);
				}
			}
			else
			{
					
				if( strstr( (char*)gprsBuffer, ">" )) //entra aca por la validación del >
				{
			
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, sendData);
                    
				}
									
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
				//	TimeOutCounter();
					SetProcessState( &gprsState, gprsReset);
				}
				
                else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
				}				
				
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
				}

			}
			break;
			
			
			
		
			/*	Send a Socket Data to Network	
			 * Comando para que el modem envie los datos cargados en la memoria.
			 * Si la respuesta esta bien, se configura para enviar el proximo dato
			 * Si esta mal, se configura para enviar nuevamente el dato
			 */
		case( sendData ):
			
			if( sendCmd )
			{
                putsProcessState( GPRS_ID, "sendData" );
				 //COMO LLEGAMOS ACÁ HABIENDO VALIDADO EL >, DIRECTAMENTE LARGAMOS LA TRAMA
                SendATCommand( (char*)temporal, gprsBuffer,gprsBuffer,50, 0,2);		//ENVIAMOS LOS DATOS
				sendCmd = FALSE;
                SetSysState(samp_loading);
			}
			else
			{
                smsInhibit = FALSE;
						
			    if( strstr( (char*)gprsBuffer, _SRING_ ) ) //HAY ALGO POR RECIBIR @ PASARLO A UN STRING COMO OK
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, receiveData);
					
				}
				
                else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
				//	TimeOutCounter();	
					SetProcessState( &gprsState, gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
				{
					SetProcessState( &gprsState,  gprsReset);
				}
				
                else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
                    SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
				}				
			}
	
			break;
			
			/*	Receive data  */
		case( receiveData ):
			
			if( sendCmd )
			{
				
				    char *sizeframe;
					sizeframe = strstr(gprsBuffer,",");
					sizeframe++;
					sprintf(atcmd_socketReceive,(char*)sizeframe); 
					SendATCommand( (char*)atcmd_socketReceive, gprsBuffer,gprsBuffer,45, 0,2); //con tres funciona
				  
				sendCmd = FALSE;
			}
			else
			{
				
				/*	si se pudo enviar la muestra sample = nextSample,
				 *	si no sample = lastSample
				 */
					char *p;
				
					if( p = strstr( gprsBuffer, "024F" ) )
					{    
						setDeviceSensorEnables( p + 6 ); //7
						setDeviceDateTime( p + 13 );    //14
						switch( dataSecuence )
						{
							case( registro ):
								Register = TRUE;
								dataSecuence = configuracion;	
							
								break;
								
							case( configuracion ):
								dataSecuence = muestras;	
								
								break;
								
							case( muestras ):
						
								goto	_continuaSubiendoMuestrasAlamcenadas;
								break;
							default:
								dataSecuence = registro;		break;
						}
						SetProcessState( &gprsState, putData);
					}
					if( p = strstr( gprsBuffer, "004F" ) )
					{
							
						setDeviceSensorEnables( p + 6 );
						setConnAttempts(0);			//	borra los intentos de conexion
_continuaSubiendoMuestrasAlamcenadas:
						/*	si esta subiendo muestras y se recibio la respuesta del servidor */
						if ( dataSecuence == configuracion )
							dataSecuence = muestras;

						if ( dataSecuence == muestras )
                        {
						/*	si hay mas muestras ... */
							if( getServerFrame(dataSecuence, nextSample) ) //	proxima muestra
							{	SetProcessState( &gprsState, putData);		} //nuev
							else
							{	
                                if ( Register )
                                {    SetProcessState( &gprsState, closeSocket);	}
                                else
                                {
                                    dataSecuence = registro;
                                    SetProcessState( &gprsState, putData);
                                }
                            }
                        }

                        else
							{	SetProcessState( &gprsState, closeSocket);	}
                        
                        

					}

					if( (dataSecuence != configuracion) && (dataSecuence != muestras) )
					{	
						//VER QUE PASA EN ESTE ESTADO
						//SetProcessState( &gprsState, ipAddress);	//nuevo
					}
					
					if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
                    {
					SetProcessState( &gprsState, gprsReset);
                    }
					
					if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
                    {
					//	TimeOutCounter();
					SetProcessState( &gprsState, gprsReset);
                    }
					
				}

			break;
			
			/*	Close a Socket	
			 * Cierra el puerto de comunicacion con el servidor.
			 */
		case( closeSocket ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "closeSocket" );
				//ClearSysState(connecting);
                SetSysState(led_off);
				SendATCommand( (string*)atcmd_closeSocket, gprsBuffer,gprsBuffer,45, 0, 1 );
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
              {
				if( strstr( (char*)gprsBuffer,  _OK_ ) )
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					SetProcessState( &gprsState, ipAddress);
					SetProcessDelay( sec(40) );
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
                    SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
				}
			}
			break;
			
 
			/*	Read IP address
			 * Consulta la direccion de IP.
			 */
		case( ipAddress ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "ipAddress" );
				SendATCommand( (string*)atcmd_socketStatus, gprsBuffer,gprsBuffer,50, 0, 2 );
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				
				if((!(strstr(gprsBuffer,"AT")))&&(strlen(gprsBuffer)<40)) //NO TIENE IP, ES DECIR NO ESTOY COENECTADO
				{
					
					if( getServerFrame( dataSecuence, lastSample ) ) 
					{
						//NO ESTOY CONECTADO Y TENGO QUE MANDAR MUESTRAS, MANDO A DISCAR.
						SetProcessDelay( sec(15) );
						SetProcessState( &gprsState, configExtendSocket);	
						
					}
					else
				   {	
					SetProcessDelay( sec(15) );
					SetProcessState( &gprsState, ipAddress);
					}
				}
				
				if(strlen(gprsBuffer)>40) //lo hago en dos separados porque el modem puede demorar un ratito en resp
				{
			     
					//SetSysState(noConnected);
					//if( (++connexionAttempts) > _MAX_CONNEXION_ATTEMPTS_ )
					//	return	GPRS_APPLICATION_RESET;
					//ClearSysState(noConnected);
					//SetSysState(connected);
					connexionAttempts = 0;
					
					/*	si hay muestra/s para enviar */
					// @todo cambiar diagrama de flujo funcionamiento gprs para que contemple consulta de puerto abierto.
					if( getServerFrame( dataSecuence, lastSample ) )
					{	
						
						SetProcessDelay( sec(15) );
						SetProcessState( &gprsState, putData);	
					}
					else
					{
					
						SetProcessDelay( sec(15) );
						SetProcessState( &gprsState,ipAddress);
					
					}
				}

				if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
				}
			}
			break;

			/*	Signal Strength	
			 * Consulta el nivel de señal.
			 */
		case( getSignal ):
			if( sendCmd )
			{
				putsProcessState( GPRS_ID, "getSignal" );
				SendATCommand( (string*)atcmd_getSignal, gprsBuffer, gprsBuffer, 10, 0, 2 );
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				if( strstr( (char*)gprsBuffer, (string*)atresp_getSignal ) )
				{
					TimeOuts=0; //Como recibimos un OK, debemos resetear la cuenta de timeouts consecutivos
					/*	actualiza el valor de señal */
					if( saveSignalLevel( (char*)gprsBuffer ) )
					{	
                          /*CAMBIO: Ahora desde acá vamos a Def_Context*/
                        SetProcessState( &gprsState, setContext);	
                        
                    }
					else
					{	SetProcessState( &gprsState, gprsReset);
					}
					#ifndef	__DEBBUG
					usr_stdout = _UART2_;
					printf("--m/sgnl:%u\n", getSignalLevel() );
					#endif
					SetProcessDelay( sec(10) ); //estaba en 15
				}
//				if( strstr( (char*)gprsBuffer, (string*)_ERROR_ ) )
//				{
//					SetProcessState( &gprsState, gprsReset);
//				}
				
				else if( strstr( (char*)gprsBuffer, (string*)_TIMEOUT_ ) )
				{
                    SetProcessState( &gprsState, gprsReset);
                    TimeOutCounter();
				}
				
                else if( strstr( (char*)gprsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( &gprsState, gprsReset);
				}	
			}
			break;

		case(gprsWaitReset):
			
			break;

		//default:
			//SetProcessState( &gprsState, gprsReset );
			break;
	}
    
    	if(TimeOuts>=_MAX_TIMEOUTS_)
           SetProcessState( &gprsState,  gprsReset);

	return	FALSE;
}

/**********************************************************************************************/


#endif	//	USE_GPRS
