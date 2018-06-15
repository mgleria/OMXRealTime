
/*
 * Archivo con los comandos AT para el modulo G30 de TELIT.
 */

#include	"drivers/at_cmds.h"

/**********************************************************************************************/
/*	Respuestas generales a cualquier comando	*/
string	_OK_[]		=	"\r\nOK";
string	_ERROR_[]	=	"\r\nERROR\r\n";
string	_TIMEOUT_[] =	"\r\nTIMEOUT\r\n";
string	_NOCARRIER_[] =	"\r\nNO CARRIER\r\n";
string	_SRING_[] =	"SRING";
string  _OVERFLOW_[] = "BUFFER_OVERFLOW";

/*	Formato de mensaje	*/
string	atcmd_msgCheckFormat[]			=	"AT+CMGF?\r\n";
string	atresp_msgCheckFormat[]			=	"+CMGF:";
string	atcmd_msgFormat[]				=	"AT+CMGF=1\r\n";
//string  MsgBuffer[]			            =	"AT+CMGL=2\r\n";



/*	Request Revision	*/
string	atcmd_getModemID[]				=	"AT+GMR\r\n";
//string	atresp_getModemID[]				=	"\r\n";
//string	atresp_getModemID[]				=	".";

/*	Request IMSI		*/
string	atcmd_getIMSI[]					=	"AT+CIMI\r";
//string	atresp_getIMSI[]				=	"\r\n";

/*	Request Product Serial Number Identification - IMEI	*/
string	atcmd_getIMEI[]					=	"AT+CGSN\r\n";
//string	atresp_getIMEI[]				=	"\r\n";

/*	Read Factory Serial Number	*/
string	atcmd_getMSN[]					=	"AT+GMI\r\n";
string	atresp_getMSN[]					=	"\r\n";

/*	Read Message	*/
string	atcmd_readMsg[]					=	"AT+CMGR=%u\r\n";
string	atresp_readMsg[]				=	"+CMGR:";

/*	SMS response	*/
string	atcmd_sendMsg[]					=	"AT+CMGS=\"%s\"\r";
string	atresp_sendMsg[]				=	"+CMGS:";

/*	Delete readded Message	*/
string	atcmd_delMsg[]					=	"AT+CMGD=%u\r\n";

/*	Lee PDP Context	*/
//string	atcmd_readContext[]				=	"AT+CGDCONT?\r\n";

/*	Define PDP Context*/
string atcmd_setContext[]                 = "AT+CGDCONT=1,\"IP\",%s,\"0.0.0.0\",0,0\r\n";

/*	Activate Context	*/
string	atcmd_activateContext[]				= "AT#SGACT=1,1\r\n";
//string	atresp_getactivateContext[]		    = "#SGACT:";


/*	Config Socket	*/
string	atcmd_configSocket[]				= "AT#SCFG=1,1,300,0,600,0\r";

/*	Config Extended Socket	*/
string	atcmd_configExtendSocket[]		    = "AT#SCFGEXT=1,1,1,0,0,0\r";

/*	Config Dial Socket	*/
string	atcmd_socketDial[]		    = "AT#SD=1,0,%u,\"%s\",0,0,1\r";

/*	Socket receive= SRECV=1,%s (Socket 1, %s bytes)	*/
 string	atcmd_socketReceive[] = "AT#SRECV=1,50\r\n";
 

/*	Send a Socket Data to Network	*/
string	atcmd_sendData[]				="AT#SSEND=1\r\n";


/*	Socket status	*/
string	atcmd_socketStatus[]					=	"AT#SS=1\r";

/*	Close a Socket	*/
string	atcmd_closeSocket[]				=	"AT#SH=1\r\n";

/*	Signal Strength	*/
string	atcmd_getSignal[]				=	"AT+CSQ\r\n";
string	atresp_getSignal[]				=	"+CSQ:";


/********************PRUEBA COMANDOS HARDCODEADOS******************************/

string	atcmd_disableEcho[]                 = "ATE0\r\n";

string  atcmd_setContextClaro[]         = "AT+CGDCONT=1,\"IP\",igprs.claro.com.ar,\"0.0.0.0\",0,0\r\n";

string  atcmd_setContextPersonal[]         = "AT+CGDCONT=1,\"IP\",internet.personal.com,\"0.0.0.0\",0,0\r\n";

string	atcmd_activateContextHARDCODED[]    = "AT#SGACT=1,1\r\n";

string	atcmd_configSocketHARDCODED[]       = "AT#SCFG=1,1,300,0,600,0\r\n";

string	atcmd_configExtendSocketHARDCODED[]	= "AT#SCFGEXT=1,1,1,0,0,0\r";

string	atcmd_socketDialHARDCODED_1[]		= "AT#SD=1,0,4545,www.omixom.com,0,0,1\r";

string	atcmd_socketDialHARDCODED_2[]		= "AT#SSEND=1\r";

string	atcmd_FRAME[]                       = "040327090E0310220701000009016501650000000ABF2000";

string	atcmd_FRAME2[]                      = "01032707160A110C110B008000800020734414000400000000000000001D002180000000000000000000000000000000000000EA010AFF6300";

string	atcmd_EOF[]                         = "\x1A"; //<SUB> (substitute) ascii character ->Fin de texto plano

string	atcmd_sListenHARDCODED[]            = "AT#SRECV=1,50\r\n";

string	atcmd_closeSocketHARDCODED[]		= "AT#SH=1\r\n";

string  atcmd_STN_OFF[]                     = "AT#STIA=0\r\n";
