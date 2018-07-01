
/*
 * Archivo con los comandos AT para el modulo G30 de TELIT.
 */

#ifndef	AT_CMDS_H
	#define	AT_CMDS_H

#include	"sistema/HardwareProfile.h"

/**********************************************************************************************/
/*	Respuestas generales a cualquier comando	*/
extern string	_OK_[];
extern string	_ERROR_[];
extern string	_TIMEOUT_[];
extern string	_SRING_[];
extern string	_NOCARRIER_[];
extern string   _OVERFLOW_[];
/*	Formato de mensaje	*/
//	comandos
extern string	atcmd_msgFormat[];
//	respuestas generales
extern string	atcmd_msgCheckFormat[];
extern string	atresp_msgCheckFormat[];

/*	Request Revision	*/
extern string	atcmd_getModemID[];
//extern string	atresp_getModemID[];

/*	Request IMSI		*/
extern string	atcmd_getIMSI[];
//extern string	atresp_getIMSI[];

/*	Request Product Serial Number Identification - IMEI	*/
extern string	atcmd_getIMEI[];
//extern string	atresp_getIMEI[];

/*	Read Factory Serial Number	*/
extern string	atcmd_getMSN[];
extern string	atresp_getMSN[];

/*	Read Message	*/
extern string	atcmd_readMsg[];
extern string	atresp_readMsg[];

/*	SMS response	*/
extern string	atcmd_sendMsg[];
extern string	atresp_sendMsg[];

/*	Delete Message	*/
extern string	atcmd_delMsg[];

/*	Lee PDP Context	*/
extern string	atcmd_readContext[];

/*	Define PDP Context	*/
extern string	atcmd_setContext[];
//extern string	atcmd_setContext1[];
//extern string	atcmd_setContext2[];

/*extern char atcmd_setContextI[50];
extern char atcmd_setContextF[50];*/

/*	Create Network Call	*/
//extern string	atcmd_createWirelessLink[];
//extern string	atresp_NO_CARRIER[];
//extern string	atresp_NO_IP[];
//extern string	atresp_IP_OK[];

/*	Open a Socket	*/
extern string	atcmd_openSocket[];
//extern string	atresp_openSocket_no_open[];
//extern string	atresp_openSocket_inactive[];
//extern string	atresp_openSocket_active[];

/*	Add Data to Socket	*/
extern string	atcmd_putData[];
//extern string	atresp_putData[];

/*	Send a Socket Data to Network	*/
extern string	atcmd_sendData[];
//extern string	atresp_sendData_flowed_off[];
//extern string	atresp_sendData_serverResp[];

/*	Close a Socket	*/
extern string	atcmd_closeSocket[];
//extern string	atresp_closeSocket[];

/*	Read IP address	*/
//extern string	atcmd_ipAddress[];
//extern string	atresp_ipAddress[];
//extern string	atresp_ipAddress_noIP[];

/*	Signal Strength	*/
extern string	atcmd_getSignal[];
extern string	atresp_getSignal[];


/*NUEVOS COMANDOS*/

extern string	atcmd_activateContext[];
//extern string	atresp_getactivateContext[];
extern string	atcmd_configSocket[];
extern string	atcmd_configExtendSocket[];
extern string	atcmd_socketDial[];
extern string	atcmd_socketReceive[];

extern string	atcmd_socketStatus[];

extern string	atcmd_STN_OFF[];

/********************PRUEBA COMANDOS HARDCODEADOS******************************/

extern string	atcmd_disableEcho[]                 ;



//extern string  atcmd_setContextHARDCODED[]          ;

extern string   atcmd_setContextPersonal[]          ;

extern string   atcmd_setContextClaro[]             ;

extern string	atcmd_activateContextHARDCODED[]    ;

extern string	atcmd_configSocketHARDCODED[]       ;

extern string	atcmd_configExtendSocketHARDCODED[]	;

extern string	atcmd_socketDialHARDCODED_1[]		;

extern string	atcmd_socketSend[]                  ;

extern string	atcmd_FRAME_1[]                     ;

extern string	atcmd_FRAME2[]                      ;

extern string	atcmd_EOF[]                         ;

extern string	atcmd_sListenHARDCODED[]            ;

extern string	atcmd_closeSocketHARDCODED[]		;

extern string   atcmd_checkIP[]                     ;

//extern string MsgBuffer[];
//PARA TRANSMITIR TRAMA FIJA
//extern  string	datatosend[];

#endif	//	AT_CMDS_H
