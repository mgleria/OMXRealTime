
/*	application includes	*/
#include	"procesos/modem.h"
#include	"funciones/puts.usb.h"

/*	standard includes	*/
#include	<stdlib.h>
#include	<stdbool.h>
#include	<stdio.h>
#include	<string.h>

#if	defined	(USE_GPRS)	||	defined	(USE_SMS)

/**********************************************************************************************/
enum
{
	rx = 0,
	tx,
	noAT
};

/**********************************************************************************************/
enum
{
	OFF = 0,
	ON
};

/**********************************************************************************************/
enum
{
	OK = 0,
	ERROR
};

/**********************************************************************************************/
/*	Estructura de cola de comandos
 */
typedef struct
{
	char*		cmd;		///<	puntero a la linea de comando
	char*		resp;		///<	puntero donde se copia la respuesta al comando enviado
	uint16		timeout;	///<	tiempo maximo de espera de la respuesta
	uint16		delay;		///<	demora para enviar el comando
	uint8		nFrames;	///<	numero de tramas que se esperan
}cmdQueue_t;

/**********************************************************************************************/
/*	variables globales para este archivo	*/
static uint16	taskDelay = 0;			///<	Valor de demora para ejecutarse la tarea.
static uint16	cmdDelay = 0;			///<	Valor de demora para enviar el proximo comando.
static uint32	cmdTimeOut = 0;			///<	Maximo tiempo de espera de la respuesta al comando.
static uint8	cmdFrames = 0;			///<	Numero de respuestas esperadas por el comando.
cmdQueue_t		cmdQueue[MAX_CMD_PERMITTED];
static uint8	cmdGet = 0;				///<	indice para obtener el proximo comando de la lista.
static uint8	cmdPut = 0;				///<	indice para colocar el prox. comando en la lista.

/*	Arreglo que contiene la tarma de envio con solicitud de algun proceso. */
//char			modemBufferTx[256] = {0};
/**	Arreglo que contiene la respuesta a la solicitud de algun proceso. */
char			modemBufferRx[256] = {0};
uint8			caracterIndex = 0;		///<	Indice del arreglo modemBufferRx.
uint16			caracterTimeOut = 0;	///<	Timeout para la entrada de caracteres.
char			dataReady = false;		///<	true cuando una trama esta lista.

static uint16 conexion_timeout = 0;		///<	variable para el time out en la comunicacion

static uint8 timeoutTimes = 0;			///<	variable para contar los TIMEOUTs y reiniciar.

static char COM_Error = false;           ///<    flag para indicar la ocurrencia de un error.
/**********************************************************************************************/
/*	Valiables extrernas definidas en otros archivos	*/


/**********************************************************************************************/
/**	macro para configurar un tiempo de demora de la tarea	*/
#define	SetTaskDelay(d)		(taskDelay = d)
/**	macro para obtener el tiempo restante de demora de la tarea	*/
#define	GetTaskDelay()		(taskDelay)

/**	macro para setear el estado para el manejo del modem	*/
#define SetModemState(st,m)	{conexionState = st; sendATcmd = m;}
/**	macro para obtener el estado del modem	*/
#define	GetModemState()		(conexionState)

//	macro para configurar el tiempo de espera de la respuesta al comando AT	*/
//#define	SetCmdTimeOut(t)	(cmdTimeOut = t)
//	macro para obtener el tiempo de espera de la respuesta al comando AT	*/
//#define	GetCmdTimeOut()		(cmdTimeOut)

/**	macro para los tiempos en milisegundos	*/
#define	msec(m)		(uint16)(m * MODEM_TASK_TICK / 10)
/**	macro para los tiempos en segundos	*/
#define	sec(s)		(uint16)((uint16)s * (1/MODEM_TASK_TICK))

/**	macro para la salida de la linea comando al modem	*/
#ifdef	__DEBUG
#define	OutCmd(cmd)		{	usr_stdout = _UART1_;	printf( "%s", cmd );	}
#else
//#define	OutCmd(cmd)		{	usr_stdout = _UART1_;	printf( "%s", cmd );	\
//							usr_stdout = _UART2_;	printf( "|DEV|%s|end|\n", cmd );	} //VER
#endif

/**********************************************************************************************/
/*	Funciones prototipo locales	*/
char	initHardware( void );
char	arbConnection( void );
void	clearModemRxBuffer( void );
void	clearRxBuffer( char* b );
void	putch( char c );
char	putCmdQueue( char* cmdLine, char* rx, uint16 timeout, uint16 delay, uint8 frames );
void    Reset_MDM(void);

/**********************************************************************************************/
/**
 * \brief
 * Tarea que se encarga de manejar de arbitrar entre los procesos gprs y gps. \n
 * Los procesos ponen y reciben los datos a traves de esta tarea. \n
 * Debe ser llamada cada 100ms o bien redefinir el tiempo en conexion.h. \n
 *
 * @return	estado de la tarea
 */
char	conexion( void )
{
	/* Enumeracion de los estados de la tarea que administra las
	 * conexiones al mismo hardware (modem).
	 */
	enum
	{
		initQueue = 0,
		iniciarHardware,
		arbitrarConexiones
	};

	auto uint8 n = 0;
	static char conexionState = initQueue;

	//	retardo para la tarea
	if( GetTaskDelay() )
	{
		SetTaskDelay( GetTaskDelay() - 1 );
		return	GetModemState();
	}

	switch( conexionState )
	{
		//	inicializa la lista de comandos AT, borra todos los comandos.
		case( initQueue ):
			while( n < sizeof(cmdQueue) )
			{
				cmdQueue[n].cmd = NULL;
				cmdQueue[n].resp = NULL;
				cmdQueue[n].timeout = 0;
				cmdQueue[n].delay = 0;
				cmdQueue[n].nFrames = 0;
				n++;
			}
			cmdGet = 0;
			cmdPut = 0;
			conexionState = iniciarHardware;
			break;

		//	inicializa solo el hardware
		case( iniciarHardware ):
			if( initHardware() )
				conexionState = arbitrarConexiones;
			break;

		//	redirecciona los datos entre los procesos
		case( arbitrarConexiones ):
			if( arbConnection() )
				conexionState = iniciarHardware;
			break;
	}

	return	GetModemState();
}

/**********************************************************************************************/
/**
 * \brief
 * Maneja los comandos y respuestas entre los diferentes procesos.
 * @return	ERROR or OK
 */
char	arbConnection( void )
{
	enum
	{
		CARGAR_COMANDO = 0,
		ENVIAR_COMANDO,
		ESPERAR_RESPUESTA,
		PROXIMO_COMANDO
	};

	static char	estado = CARGAR_COMANDO;
	static char* comando = NULL;

	/*	time out para el caso que no se reciban caracteres por mas de un det. tiempo */
	if( (conexion_timeout++) >= CONEXION_TIMEOUT )
	{
		conexion_timeout = 0;
		return	ERROR;
	}

	//	si hay algun comando por procesar...

	switch( estado )
	{
		case( CARGAR_COMANDO ):
			//	si hay algun comando en la cola
			if( cmdQueue[cmdGet].cmd )
			{
				//	carga el comando
				comando = cmdQueue[cmdGet].cmd;
				//	carga el tiempo de espera de la respuesta
				cmdTimeOut = cmdQueue[cmdGet].timeout;
				//	carga el tiempo de demora para enviar el comando
				cmdDelay = cmdQueue[cmdGet].delay;
				//	carga el numero de respuestas que se espera al comando enviado
				cmdFrames = cmdQueue[cmdGet].nFrames;

				estado = ENVIAR_COMANDO;
			}
			else
				estado = PROXIMO_COMANDO;

			break;

		case( ENVIAR_COMANDO ):
			//	espera a que termine la demora para enviar el comando...
			if( !(cmdDelay--) )
			{
				/*	limpia el buffer de recepcion				*/
				clearRxBuffer( (char*)&modemBufferRx );
				dataReady = false;
                
//				RCREG1 = NULL;
//				PIR1bits.RC1IF = false;
//				PIE1bits.RC1IE = ON;
				/*	envia el comando al modem	*/
//				OutCmd( comando ); //VER

				/*	borra el puntero al comando que ya se envio	*/
				cmdQueue[cmdGet].cmd = NULL;

				estado = ESPERAR_RESPUESTA;
			}
			break;

		case( ESPERAR_RESPUESTA ):
			if( (cmdTimeOut--) )
			{
				//	si se recibio un paquete de datos...
				if( dataReady )
				{
					dataReady = false;
					static char *trama = NULL;
					char *auxiliar=NULL;
					static char *trama_prev = NULL;


					/* cuenta los <CR><LF> en en buffer de recepcion */
                    //if( trama == NULL )
						trama = (char*)modemBufferRx;

					#ifndef	__DEBBUG
//VER
//					usr_stdout = _UART2_;
//					printf( "|MDM|%s|end|", trama );			//	eco de cada trama
					#endif
					uint8 trama_count = 0;

					/*CUANDO SE RECIBE PROMPT PARA ENVÍO DE DATOS, ES LA ÚNICA VEZ QUE LA RESPUESTA NO TERMINA CON \r\n
					 ENTONCES SE LE CONCATENA ESTO AL FINAL PARA QUE FUNCIONE EL TRAMACOUNT Y SE RESPETE LA MISMA LÓGICA
					 DE LA MÁQUINA DE ESTADOS.*/
					auxiliar=strstr(trama,"\r\n>");//	buscamos el > para saber que sta listo para recibir
				      if(auxiliar!=NULL)
						{
						 // *trama=NULL;
						  strcpy(trama,"\r\n>\r\n" );
						  strcat(trama,"\0");
					   }

					while( *trama )
					{
						if( trama_prev = (strstr( (char*)trama, (const char*)"\r\n" ) + 2) )
						{
							trama_count ++;
							trama = trama_prev;
							//timeoutTimes = 0;
						}
					}

                    /*	calcula la cantidad de tramas recibidas */
					trama_count /= 2;


					/*	si hay al menos una trama ... */
					//if( trama_count )
					//{
						/*	copia la respuesta a quien la solicita */
					//	strcpy(cmdQueue[cmdGet].resp, modemBufferRx );
					//}

					/*	retorna la respuesta una vez que llegan todas las tramas solicitadas */
					//if( (sint8)(cmdFrames -= trama_count) <= 0 )
                    if( cmdFrames <= trama_count )
					{

					//	timeoutTimes = 0;
						/*	desactiva interrupcion en recepcion */
//VER
//						PIE1bits.RC1IE = OFF;
						trama = NULL;
						/*	copia la respuesta a quien la solicita */
						strcpy( cmdQueue[cmdGet].resp, modemBufferRx );
                        if( strstr( (char*)modemBufferRx, (const char*) "ERROR" ) )
                            COM_Error = true;
                        timeoutTimes = 0;
//						#ifndef	__DEBBUG
//						usr_stdout = _UART2_;
//						printf( "%s", cmdQueue[cmdGet].resp );			//	eco de la trama
//						#endif
						clearModemRxBuffer();
						estado = PROXIMO_COMANDO;
						break;
					}
				}
				//	si no se espera respuesta
				if( !(cmdFrames) )
				{
					clearModemRxBuffer();
					estado = PROXIMO_COMANDO;
					break;
				}

			}
			//	Si se supero el tiempo de espera de la respuesta...
			else
			{
				/*	apaga interrupciones en recepcion de la uart del modem */
//				PIE1bits.RC1IE = OFF;
                //VER
                strcat( (char*)modemBufferRx, (const char*)"\r\nTIMEOUT\r\n\0" );	//	responde con "TIMEOUT"
				//cmdFrames = 1;	//	se forza a una sola respuesta
				timeoutTimes ++;
                strcpy( cmdQueue[cmdGet].resp, modemBufferRx );
                if( strstr( (char*)modemBufferRx, (const char*) "ERROR" ) )
                    COM_Error = true;
                clearModemRxBuffer();
				//dataReady = true;
                estado = PROXIMO_COMANDO;
			}

			break;

		case( PROXIMO_COMANDO ):
			/*	prepara el proximo comando	*/

			cmdGet = (++cmdGet & (MAX_CMD_PERMITTED-1));

			/*	solo carga el comando si encuentra uno disponible	*/
			if( cmdQueue[cmdGet].cmd )
				estado = CARGAR_COMANDO;

            /*  si existieron demasiados errores consecutivos    */
            if (COM_Error)
            {
                estado = CARGAR_COMANDO;
                COM_Error = false;
                return (ERROR);
            }
			/*	si la cola esta vacia y hay ciertos TIMEOUTs	*/
			if( (cmdGet == cmdPut) && (timeoutTimes >= MAX_TIMEOUT_PERMITTED) )
			{
				timeoutTimes = 0;
				estado = CARGAR_COMANDO;
				return	(ERROR);
			}
//			if( (cmdGet == cmdPut) && (errorTimes >= MAX_ERROR_PERMITTED) )
//			{
//				errorTimes = 0;
//				estado = CARGAR_COMANDO;
//				return	(ERROR);
//			}


			break;
	}
	return	(OK);
}

/**********************************************************************************************/
/**
 * \brief
 * Funcion para poner en la cola un comando AT, un tiempo de espera de respuesta,
 * y una demora en ejecutarse ese comando.\n
 * En caso de superar el maximo numero de comandos, el ultimo de la cola se pisa.
 * @param cmdLine	puntero al texto que contiene el comando AT.
 * @param rx		puntero donde se copia la respuesta al comando enviado.
 * @param timeout	tiempo maximo de espera de la respuesta en [S].
 * @param delay		tiempo de demora para enviar el comando en [mS].
 * @param frames	numero de tramas o respuestas que se esperan.
 *
 * @return			true si pudo colocar en la cola el comando
 */
char	putCmdQueue( char* cmdLine, char* rx, uint16 timeout, uint16 delay, uint8 frames )
{

	//	controla que no se pisen los comandos de la lista
//	if( cmdPut == cmdGet )
//		return	false;

	//	carga en la cola el proximo comando a ejecutarse
	cmdQueue[(cmdPut)].cmd = cmdLine;

	//	carga en la cola la direccion del buffer para la respuesta
	cmdQueue[(cmdPut)].resp = rx;


	//	carga el tiempo maximo para esperar la respuesta
	cmdQueue[(cmdPut)].timeout = sec( timeout );

	//	carga el tiempo de demora para enviar el comando
	cmdQueue[(cmdPut)].delay = msec( delay );

	//	carga la cantidad de respuestas que debe devolver el modem al proceso
	cmdQueue[(cmdPut)].nFrames = frames;

	//	incrementa el numero de comandos en la cola y trunca el valor
	cmdPut = (++cmdPut & (MAX_CMD_PERMITTED-1));

	return	(true);
}

/**********************************************************************************************/
/**
 * \brief
 * Borra todos los caracteres del buffer de recepcion hasta el primer NULL.
 */
void	clearRxBuffer( char* b )
{
	while( *b )
		*b++ = NULL;
}

/**********************************************************************************************/
/**
 * \brief
 * Envia un comando a la lista del proceso "conexion".
 * @param text	cadena de texto que contiene el comando.
 * @param tx	puntero al buffer donde queda el comando almacenado hasta ser enviado.
 * @param rx	puntero al buffer donde se almacena/n la/s respuesta/s al comando.
 * @param t		tiempo de espera maximo de espera de la/s respuesta/s.
 * @param d		tiempo de demora en enviar el comando al modem.
 * @param f		numero de respuestas esperadas.
 */
void	SendATCommand( const char* text, char* tx, char* rx, uint16 t, uint16 d, uint8 f )
 {



	/*	copia el comando en el buffer de TX			*/
	strcpy( (char*)tx, (const char*)text );
	/*	envia el comando a la lista					*/
	putCmdQueue( tx, rx, t, d, f );

}

/**********************************************************************************************/
/**
 * Interrupcion de recepcion de la UART 1 utilizada por el modem
 */
void uart_modem_interrupt(void)
{
//	modemBufferRx[caracterIndex] = getc_uart1();		// Copio el dato recibido en el buffer
//
//
//	usr_stdout = _UART1_;
//	modemBufferRx[caracterIndex+1] = NULL;				//	caracter fin de trama
//
//
//	/*	Incremento la posición del buffer de recepcion	*/
//	caracterIndex = (++caracterIndex) & (sizeof(modemBufferRx)-1);
//
//	/*	recarga el tiempo de espera de caracteres	*/
//	caracterTimeOut = MODEM_DATA_TIMEOUT;
//
//	/*	bandera de fin de trama	*/
//	dataReady = false;
//
//	/*	reset del timeout de conexion	*/
//	conexion_timeout = 0; //VER
}

/**********************************************************************************************/
/**
 * \brief
 * Inicializa el HARDWARE del modem.
 *
 * @return	OFF cuando se termina de inicializar, ON mientras se inicializa.
 */
char	initHardware( void )
{
	//	posibles estados locales
	enum
	{
		HARD_OFF = 0,
		HARD_ON,
		HARD_CONFIG
	};

	static char	estado = HARD_OFF;
	static char sendATinit = true;

	switch( estado )
	{
		case( HARD_OFF ):
			#if	(__DEBUG!=1)
//			usr_stdout = _UART2_; //VER
			printf("|LOG|Modem apagado|end|");
			#endif
//			PIE1bits.TX1IE = OFF;
//			POWER_GPRS_HARD_OFF;					// quita alimentacion al modulo
//			SetSysState(noConnected); //VER
			SetTaskDelay( sec(20) );
			estado = HARD_ON;
			break;

		case( HARD_ON ):
//			POWER_GPRS_HARD_ON;						// habilita alimentacio al modulo gprs //VER
			SetTaskDelay( sec(20) );				// retardo para ejecutar el proximo comando
//			set_uart_speed( 1, 138 );				//	uart 1 @ 115200, rx interrupt //VER
//			#ifndef	__DEBUG
//			set_uart_speed( 2, 138 );				//	uart 2 @ 115200, rx interrupt
//			#endif
			putsLog("Modem encendido");
			estado = HARD_CONFIG;
			break;

		case( HARD_CONFIG ):
		//	enable_interrupt(UART1);				// habilita las interrupciones de RDA
			if( sendATinit )
			{
				clearModemRxBuffer();
//				usr_stdout = _UART1_; //VER
				puts( "AT&K0;E0;V1\r" );
				sendATinit = false;
				cmdTimeOut = sec(10);
			}
			else
			{
				if( (cmdTimeOut--) )
				{
					if( dataReady )
					{
						if(strstr( (char*)modemBufferRx, (char*)"\r\nOK\r\n" ) )	// Si reponde un Ok
						{
							putsLog("Inicializacion modem correcta");
							//printf("ModemBufferRx vale: %s",modemBufferRx);
							clearModemRxBuffer();
							sendATinit = true;
							cmdTimeOut = 0;
							estado = HARD_CONFIG;
							SetTaskDelay( sec(1) );
							estado = HARD_OFF;
							return	(ON);								//	fin inicializacion
						}
						dataReady = false;
					}
				}
				else
				{
					sendATinit = true;
					estado = HARD_OFF;
				}
			}
			break;
	}
	return	(OFF);
}

/**********************************************************************************************/
/**
 * \brief
 * Borra todos los caracteres del buffer de recepcion del modem.
 */
void	clearModemRxBuffer( void )
{
	auto uint16 n = 0;
	while( n < sizeof(modemBufferRx) )
		modemBufferRx[n++] = NULL;
	caracterIndex = 0;
}

/**********************************************************************************************/
/**
 * \brief
 *
 */
void	setConexionProcessTick()
{
	if( caracterTimeOut )
	{
		if( !(--caracterTimeOut) )
			dataReady = true;
	}
}


#endif	
