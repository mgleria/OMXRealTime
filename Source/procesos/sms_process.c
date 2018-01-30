

/*	Application includes	*/
#include	"procesos/sms_process.h"
#include	"funciones/puts.usb.h"
#include	"funciones/CmdLineInterpreter.h"
#include    "funciones/memory.h"

/*	Standard includes	*/
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<ctype.h>

#ifdef	USE_SMS

/*	at commands includes	*/
#include	"drivers/at_cmds.h"

/**********************************************************************************************/
/*	variables globales para este archivo	*/
/*	buffers de transmision y recepcion	*/
char smsBuffer[Buffer_SMS_Size];


/**********************************************************************************************/
/*	variables externas a este archivo	*/


/*	indica si se envia o recibe una trama	*/
static char sendCmd = TRUE;
static uint8 reset_cmd;
uint8 smsreset_done;

/*	arreglo para almacenar el numero de telefono en caso de un SMS	*/
static char	smsPhoneNumber[22] = {0};

/*	texto de respuesta al comando SMS	*/
static char smsResponse[200];

/*	variable de estado y estado previo del proceso */
//static uint8 smsState = 1, prev_smsState = 0xFF;

/*	tiempo maximo para que el proceso permanezca en el mismo estado	*/
//static uint16 stateTimeout = 0;

/**********************************************************************************************/
/*	Funciones prototipo locales	*/
char	saveSMSnumber( char* text );
char	*getSMSnumber( void );
char	*getSMSresponse( void );
uint8	SaveSMSresponse( char* text );
void	strrep( char* s1, const char* s2, const char* s3 );

/**********************************************************************************************/
/*	***	Macros ***	*/
/**	macro para cambiar el estado del proceso	*/
#define	SetProcessState(state)		{	smsState = state;	sendCmd = TRUE;		}

/**	macro para calular tiempo en milisegundos	*/
#define	msec(m)		(uint16)((uint16)m * SMS_PROCESS_TICK / 10)
/**	macro para calcular tiempo en segundos	*/
#define	sec(s)		(uint16)((uint16)s * (1/SMS_PROCESS_TICK))

/**	macro para configurar un tiempo de demora de la tarea	*/
#define	SetProcessDelay(d)		(processDelay = d)
/**	macro para obtener el tiempo restante de demora de la tarea	*/
#define	GetProcessDelay()		(processDelay)

/*	DEFINICION DE TEXTOS CONSTANTES	*/
#define	_MAX_PHONE_DIGITS_	sizeof(smsPhoneNumber)

/*	Definicion de maximos intentos de envio de SMS */
#define	_MAX_SMS_SEND_ATTEMPTS_		8


/**********************************************************************************************/
/**
 *
 * @return	FALSE si el proceso esta corriendo, TRUE caso de solicitar un reinicio de equipo.
 */
char	smsProcess( void )
{
	static enum
	{
		smsNoCommand,				//!	no command
		smsReset,					//!	reset
		msgCheckFormat,				//!	check text format
		msgFormat,					//!	Formato de mensaje
		delMsg,						//!	borra mensajes de la memoria del modem
		readMsg,					//! lee mensajes de texto recibidos
		sendMsg,					//!	envia mensajes de texto
		smsWait
	}smsState = smsReset, prev_smsState = 0xFF;

	static uint8 smsSendAttempts = 0;		///< Intentos de envio de SMS.
	static uint8 smsIndex = 0;				///< Indice de SMS. Se permite hasta un maximo de 4
	
	//	retardo inicial para comenzar el proceso
	static uint16 processDelay = sec(60);

	/*	demora del proceso	*/
	if( GetProcessDelay() )
	{
		SetProcessDelay( GetProcessDelay() - 1 );
		return	FALSE;
	}

	/* Controla que el proceso evolucione, es decir, que no se encuentre
	 * en un estado por mas de cierto tiempo.
	 */
	#define	PROCESS_TIMEOUT	(uint16)(2.0*60.0/SMS_PROCESS_TICK)
	static uint16 processTimeout = 0;	/*	tiempo maximo para que el proceso permanezca en el mismo estado	*/
	if( prev_smsState != smsState )
	{
		processTimeout = 0;
		prev_smsState = smsState;
	}
	if( (processTimeout++) >= PROCESS_TIMEOUT )
	{
		processTimeout = 0;
		SetProcessState(smsReset);
		putsProcessState( SMS_ID, "Reset request" );
		return	SMS_APPLICATION_RESET;
	}
	#undef	PROCESS_TIMEOUT

	//	para la comunicacion con el software
	usr_stdout = _UART2_;

	/*	estados del proceso	*/
	switch( smsState )
	{
		default:
		case( smsNoCommand ):
			putsProcessState( SMS_ID, "smsNoCommand" );
			SetProcessDelay( sec(45) );
			SetProcessState(msgCheckFormat);
			break;

		/*	check Message Format	*/
		case( msgCheckFormat ):
			if( sendCmd )
			{
				putsProcessState( SMS_ID, "msgCheckFormat" );
				SendATCommand((string*)atcmd_msgCheckFormat,smsBuffer,smsBuffer,10,0,2);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
	
				char *s;
				
			
				if( s = strstr( (char*)smsBuffer, (char*)atresp_msgCheckFormat ) )
				{
					
					SetProcessState(smsReset);
					s += 7;
					if( *s == '0' )		//	PDU mode (default)
					{	
						SetProcessState(msgFormat);	
					}
					if( *s == '1' )		//	Text mode
					{	SetProcessState(readMsg);	}
					SetProcessDelay(sec(10));
				}
				if( strstr( (char*)smsBuffer, (string*)_ERROR_ ) )
				{
					SetProcessState(smsReset);
				}
				
                if( strstr( (char*)smsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( smsReset);
				}
				
				if( strstr( (char*)smsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( smsReset);
				}
			}
			break;

		/*	Message Format	*/
		case( msgFormat ):
			if( sendCmd )
			{
				
				putsProcessState( SMS_ID, "msgFormat" );
				SendATCommand((string*)atcmd_msgFormat,smsBuffer,smsBuffer,10,0,1);
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
			
				if( strstr( (char*)smsBuffer, (char*)_OK_ ) )
				{
					SetProcessState(readMsg);
					SetProcessDelay(sec(30));
				}
				if( strstr( (char*)smsBuffer, (string*)_ERROR_ ) )
				{
					SetProcessState(smsReset);
				}
				
				if( strstr( (char*)smsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( smsReset);
				}
				
				if( strstr( (char*)smsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState( smsReset);
				}
			}
			break;

		/*	Read Message	*/
			/*
			 * Ejemplos:
			 * 
			 * <CR><LF>+CMGR: "REC UNREAD","+543515326777",,"12/03/29,09:06:36-12"<CR><LF>DATO<CR><LF><CR><LF>OK<CR><LF>
			 * <CR><LF>+CMGR: "REC UNREAD","+543515186285",,"13/07/29,18:51:25+00"<CR><LF>29100398 06*200.110.129.118:4545<CR><LF><CR><LF>OK<CR><LF>
			 *
			 * <CR><LF>+CMGR: "REC READ","15","","13/08/08,11:21:22-12"<CR><LF> texto del SMS <CR><LF><CR><LF>OK<CR><LF>
			 *<CR><LF>+CMGR: "REC READ","15","","13/08/08,11:21:22-12"<CR><LF> texto del SMS <CR><LF><CR><LF>OK<CR><LF>*/
		
		case( readMsg ):
			if( sendCmd )
			{				
				
				auto char ATcmd[16] = {0};
				smsIndex &= 0x3;			//	hasta 4 mensajes simultaneos
				putsProcessState( SMS_ID, "readMsg" );
				
                sprintf( ATcmd, atcmd_readMsg, (smsIndex+1) );
				smsIndex ++;
				SendATCommand((string*)ATcmd,smsBuffer,smsBuffer,4,0,2); //con 1 funciona
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
                				
			    if( strstr( (char*)smsBuffer, (string*)atresp_readMsg ) )
				{
				
					SetProcessState(delMsg);
					SetProcessDelay(sec(30));
					/* Si el numero de destino del SMS es valido, comprueba que el comando del
					 * mensaje sea valido y carga el buffer con la respuesta correspondiente.
					 */
					if( saveSMSnumber( (char*)smsBuffer ) )			//	guarda el numero
					{
						
						if( SaveSMSresponse((char*)smsBuffer) )		//	guarda respuesta al sms
						{
							SetProcessState(sendMsg);
						}
                        if(smsreset_done)
                        {
                            strcpy( smsResponse, (string*) "Reset done");
                            smsreset_done = FALSE;
                            i2cbus_write( _24LC512_0, SMS_RESET_FLAG, &smsreset_done, sizeof (smsreset_done));
                            SetProcessState(sendMsg);
                        }
					}
				}
               
                else if( strstr( (char*)smsBuffer, (string*)_OVERFLOW_))
                {
                   SetProcessState(delMsg);
                   SetProcessDelay(sec(10));
                }
				
                else if( strstr( (char*)smsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState(smsWait);
				}
			
                else if( strstr( (char*)smsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( smsReset);
				}
				
				
			}
			break;

			/*	SMS response	*/
			/*
			 * Ejemplos:
			 * 
			 * AT+CMGS="+543515326777"<CR><CR><LF>>Fecha: 28/3/12 18:57 Temp: 22.3*C Hum: 44.0%HR Lluv: 0.0mm<SUB>
			 * <CR><LF>+CMGS: 8<CR><LF><CR><LF>OK<CR><LF>
			 *
			 * AT+CMGS="+543515326777"<CR><CR><LF>>Fecha: 29/3/12 9:10 Temp: 21.9*C Hum: 51.5%HR Lluv: 0.0mm<SUB>
			 * <CR><LF>+CMGS: 12<CR><LF><CR><LF>OK<CR><LF>
			 */
		case(sendMsg):
			if( sendCmd )
			{
				auto char ATcmd[32] = {0};
				//static char resp[200] = {0};
                static char resp[200] = {0};
				putsProcessState( SMS_ID, "sendMsg" );
				sprintf( ATcmd, atcmd_sendMsg, (char*)getSMSnumber() );
				/*	agrega caracter de fin de numero */
				strcat( ATcmd, (string*)"\r" );
				/*	envia el numero destinatario del SMS	*/
				SendATCommand( (string*)ATcmd, smsBuffer,smsBuffer,1, 0, 0 );
				/*	agrega la respuesta	*/
				strcpy( resp, getSMSresponse() );
				/*	agrega caracter <ctrl-z>	*/
				strcat( resp, (string*)"\x001A" );
				/*	envia el comando AT	*/
				SendATCommand( (string*)resp, resp,smsBuffer,10, 20, 2 );
                /*  controla si el equipo debe resetearse luego */
                if (strstr((char*)resp, (const char*) "Reset in progress"))
                    reset_cmd = TRUE;
				/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				if( strstr( (char*)smsBuffer, (string*)atresp_sendMsg ) )
				{
					smsSendAttempts = 0;		//	se pudo enviar el SMS de respuesta
					SetProcessState(delMsg);
                    if (reset_cmd)
                    {   
                        auto uint8 auxres;
                        auxres = 1;
                        i2cbus_write( _24LC512_0, SMS_RESET_FLAG, &auxres, sizeof (auxres) );
                        reset_cmd = FALSE;
                        return SMS_APPLICATION_RESET | ALLOW_RESET_DEVICE;
                    }
				}
//				if( strstr( (char*)smsBuffer, (string*)_ERROR_ ) )
//				{
//					SetProcessState(smsReset);
//				}
				if( strstr( (char*)smsBuffer, (string*)_TIMEOUT_ ) )
				{
					/*	si no puede enviar sms en ciertos intentos, borra el sms */
					if( (++smsSendAttempts) > _MAX_SMS_SEND_ATTEMPTS_ )
					{
						SetProcessState(smsReset);
					}
					else
					{
						SetProcessDelay(sec(15));
						SetProcessState(sendMsg);
					}
				}
				
				if( strstr( (char*)smsBuffer, (string*)_NOCARRIER_ ) )
				{
					SetProcessState( smsReset);
				}
			}
			break;

			/*	Delete Message	*/
		case( delMsg ):
			if( sendCmd )
			{
				putsProcessState( SMS_ID, "delMsg" );
				static char ATcmd[20] = {0};
				sprintf( ATcmd, atcmd_delMsg, smsIndex);
				SendATCommand((string*)ATcmd,smsBuffer,smsBuffer,10,0,1);
              	/*	modo recepcion para espera de la respuesta	*/
				sendCmd = FALSE;
			}
			else
			{
				if( strstr( (char*)smsBuffer, (char*)_OK_ ) )
				{
					SetProcessState(msgCheckFormat);
					SetProcessDelay(sec(30));
				}
				if( strstr( (char*)smsBuffer, (string*)_ERROR_ ) )
				{
					SetProcessState(smsReset);
				}
				if( strstr( (char*)smsBuffer, (string*)_TIMEOUT_ ) )
				{
					SetProcessState(smsReset);
				}
			}
			break;
            
		/*	Solo para cambiar de estado y que el procesos no crea que esta clavado	*/
		case(smsWait):
			putsProcessState( SMS_ID, "smsWait" );
			SetProcessDelay(sec(25));
			SetProcessState(readMsg);
			break;
	}
	return	FALSE;
}


/**********************************************************************************************/
/**
 * \brief
 * Guarda el numero de telefono del ultimo SMS leido. Maximo 20 caracteres.\n
 * Se guarda como
 * @param text	puntero a la respuesta del modem al comando AT+CMGR=1?.
 * @return		true si el numero es valido, false si no.
 */
char	saveSMSnumber( char* text )
{
	/*	algunas prestadoras mandan el numero sin +54 !!! OJO !!!	*/
	const char _PHONE_NUMBER_START_[] = "\",\"";
	auto char *dest = (char*)&smsPhoneNumber;
	auto uint8 n = 0;

	if( text = strstr( text, _PHONE_NUMBER_START_ ) )
	{
		text += 3;				//	apunta a la primer numero/simbolo
	//	*dest++ = *text++;		//	copia la primer comilla

		//	si el primer numero es '0', no se tiene en cuenta
		if( *text == '0' )
			text++;				//	no copia el '0'

		//	Si empieza con + lo copia
		if( *text == '+')
			*dest++ = *text++;	//	copia el '+'

		//	mientras sea un numero o hasta _MAX_PHONE_DIGITS_ se copia
		while( isdigit(*text) && ((n++)< (_MAX_PHONE_DIGITS_ - 2)) )
		{
			*dest++ = *text++;
		}
		if( n > (_MAX_PHONE_DIGITS_ - 2))
			return	FALSE;

	//	*dest++ = '"';			//	agrega la comilla de cierre
		*dest = NULL;			//	fin numero

		#ifndef	__DEBBUG
		usr_stdout = _UART2_;
		printf( "|LOG|Numero %s|end|",smsPhoneNumber );
		#endif
		return	TRUE;
	}
	else
	{
		putsLog("Numero no reconocido");
		return FALSE;
	}
}

/**********************************************************************************************/
/**
 * \brief
 * Devuelve el numero de telefono del ultimo SMS recibido.
 * @return	puntero al primer digito del numero de telefono del ultimo SMS recibido.
 */
char	*getSMSnumber( void )
{
	return	(char*)&smsPhoneNumber;
}

/**********************************************************************************************/
/**
 * Guarda en memoria la proxima respuesta que debe dar al numero almacenado.
 * @param text texto recibido del comando SMS
 * return true si hay respuesta, false si no hay.
 */
uint8	SaveSMSresponse( char* text )
{
	putsLog("SMS recibido");
	smsResponse[0] = NULL;

	//	<CR><LF>+CMGR: "REC UNREAD","+543515186285",,"13/07/29,18:51:25+00"<CR><LF>Clima?<CR><LF><CR><LF>OK<CR><LF>
	/*	si se encuentra un mensaje en el texto ... */
	if( text = strstr( text, (string*)"UNREAD") )
	{
		if( text = strstr(text, "\r\n") )	//	busca el inicio del texto del comando SMS
		{
			/*	convierte todos los caracteres a minuscula	*/
			auto uint8 usrQuery = FALSE;
			char *s = text + 2;				//	salta los caracteres <CR><LF>
			while( *s )
				*s = tolower( *s++ );
			smsResponse[0] = NULL;
			#ifdef	USE_CMD_LINE
			/*	si es una consulta de usuario	*/
			if( strstr( (string*)text, (string*)"clima?" ) )
			{
				sprintf( text, "--temp? --hum? --press? --rain? --wdir? --wspd? --srad? --tsoil?\0");
				usrQuery = TRUE;
			}
			//	copia respuesta al comando
			strcpy( smsResponse, processCmdLine( text ) );
			if( smsResponse[0] != NULL )
			{
				//	@todo reemplazar por la funcion para quitar comandos
				if( usrQuery )
				{
					strrep(smsResponse,"--temp","Temper");
					strrep(smsResponse,"--hum","Humed");
					strrep(smsResponse,"--press","Presion");
					strrep(smsResponse,"--rain","Lluvia");
					strrep(smsResponse,"--wdir","Dir.V.");
					strrep(smsResponse,"--wspd","Vel.V.");
					strrep(smsResponse,"--srad","Radiac");
					strrep(smsResponse,"--tsoil","T.Suelo");
				}
				printf( "|LOG|Respuesta: %s|end|", smsResponse );
				return	TRUE;
			}
			#else
			return	smsCmdFind( text, smsResponse );
			#endif
		}
	}
	/*	si no hay datos en el cuerpo del mensaje	*/
		/*	si el SMS ya fue leido	*/
	if( text = strstr( text, (string*)"READ") )
	{
		smsResponse[0] = NULL;
		return FALSE;
	}
    
	return FALSE;
}

/**********************************************************************************************/
/**
 * Devuelve la respuesta a un comando de SMS previamente recibido.
 * @return puntero a la respuesta.
 */
char	*getSMSresponse( void )
{
	return	&smsResponse;
}

/**********************************************************************************************/
/**
 * \brief
 * Reemplaza s2 con s3 en caso de encontrarse dentro de s1.\n
 * La unica condicion es que s2 y s3 deben tener la misma cantidad de caracteres.
 *
 * @param s1	cadena de texto.
 * @param s2	texto a ser reemplazado.
 * @param s3	texto de reemplazo.
 */
void	strrep( char* s1, const char* s2, const char* s3 )
{
	char* st;
	//	controla que exista s2 dentro de s1
	if( st = strstr(s1, s2) )
	{
		uint8 lens2 = strlen( s2 );
		uint8 lens3 = strlen( s3 );
		if( lens2 != lens3 )
			return;
		strncpy( st, s3, lens2 );
	}
}

#endif	//	USE_SMS