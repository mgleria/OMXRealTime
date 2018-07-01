/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */

#ifndef GPRSTASK_H
#define	GPRSTASK_H

#include    "utilities.h"
#include    "typedef.h"
#include    "drivers/at_cmds.h"
#include    "drivers/modem.h"
#include    "tmr4.h"
#include    "funciones/memory.h"
#include    "funciones/rtcc.h"

#include    "freeRTOS/FreeRTOS.h"
#include    "freeRTOS/task.h"
#include    "freeRTOSConfig.h"

#define     GPRS_BUFFER_SIZE    120 //Duplicado ojo!

/*	enumeracion de los posibles estados en el manejo del gprs	*/
	static enum GPRS_STATE
	{
		noCommand = 0,				//!	no se ejecuta ningun comando
		gprsReset,					//!	reinicio del proceso GPRS
		initModem,					//!	inicializacion del modem
		getModemID,					//!	consulta identificacion de la revision del modem
		getMSN,						//!	consulta en numero de serie del fabricante
		getIMSI,					//!	consulta el numero de identificacion del abonado
		getIMEI,					//!	consulta el numero de identificacion serie del modem
		getSignal,					//!	consulta el nivel de seï¿½al
		connectionStatus,			//!	consulta la direccion de IP al modem
		setContext,					//!	configura el contexto
        activateContext,			//!	Activa el contexto previamente configurado
        configSocket,               //!	Configurar el socket
        configExtendSocket,         //!	Configuraciï¿½n extendida el socket
        socketDial,                 //!	Apertura del socket
        socketSend,                 //! Prepara el socket para en el envío de datos               
		socketStatus,               //! Nuevo estado para saber el estado del socket
		closeSocket,				//!	cierra el puerto de conexion con la red
		sendData,					//!	envia los datos almacenados usando el protocolo configurado
		putData,					//!	coloca los datos dentro del protocolo
        receiveData,				//! Recibe e interpreta datos desde el servidor
		gprsWaitReset,				//!	estado para esperar el reinicio del equipo
        finalStateToggleLed,
        STN_OFF
	}gprsState = gprsReset;
    
    typedef enum
    {
        muestras = 1,
        reservado,
        configuracion,
        registro
    }putDataSecuence_t;
    
    void startGprsTask();

#endif	/* GPRSTASK_H */

