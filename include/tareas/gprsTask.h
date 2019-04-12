/* 
 * File:   gprsTask.h
 * Author: Tincho
 *
 * Created on 7 de junio de 2018, 10:19
 */

#ifndef GPRSTASK_H
#define	GPRSTASK_H

#include    "typedef.h"
#include    "utilities.h"
#include    "drivers/at_cmds.h"
#include    "drivers/modem.h"
//#include    "tmr4.h"
//#include    "funciones/memory.h"
#include    "funciones/rtcc.h"
#include    "funciones/eeprom.h"
#include    "tareas/sampleTask.h"

#include    "freeRTOS/FreeRTOS.h"
#include    "freeRTOS/task.h"
#include    "freeRTOSConfig.h"

#include    "ezbl.h"

#define     GPRS_BUFFER_SIZE            120 //Duplicado ojo!
#define     NEW_SAMPLE_NOTIFICATION     3030
#define     MAX_ATTEMPTS_NUMBER         10
#define     FRAME_HEADER_SIZE           5

#define     _024F                       1
#define     _004F                       2
#define     _024E                       3
#define     _004E                       4

/*	enumeracion de los posibles estados en el manejo del gprs	*/
	static enum GPRS_STATE
	{
		gprsReset = 0,				//!	reinicio del proceso GPRS
        disableEcho,        
        initModem,					//!	inicializacion del modem
        setContext,					//!	configura el contexto
		configSocket,               //!	Configurar el socket
        configExtendSocket,         //!	Configuracion extendida el socket
        getSignal,
        activateContext,			//!	Activa el contexto previamente configurado   
        socketDial,                 //!	Apertura del socket
        socketSend,                 //! Prepara el socket para en el env�o de datos
        putData,					//!	coloca los datos dentro del protocolo
        receiveData,				//! Recibe e interpreta datos desde el servidor               
		closeSocket,				//!	cierra el puerto de conexion con la red
        waitForNewRequests,
        connectionStatus,			//!	consulta la direccion de IP al modem
        LAST
	}gprsState = gprsReset;
    
    typedef enum
    {
        muestras = 1,
        reservado,
        configuracion,
        registro
    }putDataSecuence_t;
    
    typedef enum
    {
        h024F = 1,
        h004F,
        h024E,
        h004E
    }headerOptions_t;
    
    void startGprsTask();

#endif	/* GPRSTASK_H */

