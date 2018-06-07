/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */

#ifndef GPRSTASK_H
#define	GPRSTASK_H

#include    "typedef.h"
#include    "drivers/at_cmds.h"

#define     GPRS_BUFFER_SIZE    120 //Duplicado ojo!

/*	enumeracion de los posibles estados en el manejo del gprs	*/
	static enum
	{
		noCommand = 0,				//!	no se ejecuta ningun comando
		gprsReset,					//!	reinicio del proceso GPRS
		initModem,					//!	inicializacion del modem
		getModemID,					//!	consulta identificacion de la revision del modem
		getMSN,						//!	consulta en numero de serie del fabricante
		getIMSI,					//!	consulta el numero de identificacion del abonado
		getIMEI,					//!	consulta el numero de identificacion serie del modem
		getSignal,					//!	consulta el nivel de se�al
		ipAddress,					//!	consulta la direccion de IP al modem
		setContext,					//!	configura el contexto
        activateContext,			//!	Activa el contexto previamente configurado
        configSocket,               //!	Configurar el socket
        configExtendSocket,         //!	Configuraci�n extendida el socket
        socketDial,                 //!	Apertura del socket
        socketDial_2,                
		socketStatus,               //!Nuevo estado para saber el estado del socket
		closeSocket,				//!	cierra el puerto de conexion con la red
		sendData,					//!	envia los datos almacenados usando el protocolo configurado
		putData,					//!	coloca los datos dentro del protocolo
        receiveData,				//Recibe datos desde el servido
		gprsWaitReset,				//!	estado para esperar el reinicio del equipo
        finalStateToggleLed,
	}gprsState;

#endif	/* GPRSTASK_H */

