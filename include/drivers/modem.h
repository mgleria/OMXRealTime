/* 
 * File:   conexion.h
 * Author: Juan Cruz Becerra
 *
 * Created on 28 de noviembre de 2012, 22:42
 */

#ifndef MODEM_H
#define	MODEM_H

#include    <stdio.h>

#include	"sistema/HardwareProfile.h"
#include    "utilities.h"
#include    "tmr5.h"

#include    "ezbl.h"

#include    "freeRTOS/FreeRTOS.h"
#include    "freeRTOS/semphr.h"
#include    "task.h"
//#include "freeRTOSConfig.h"

extern EZBL_FIFO *EZBL_COMBootIF;

/*	DEFINICIONES DE TIEMPOS	*/
#define	T_ESPERA_MUTEX_MODEM_MS     500

/*	DEFiNICIONES DE PINES DE CONTROL	*/
#define	MODEM_POWER(s)		{(TRISXbits.TRISXx = 0); (LATXbits.LATXx = s);}


/*	Definir aqui el maximo de comandos permitidos para arbitrar	*/
#define	MAX_CMD_PERMITTED		4		//	multiplos de 2^n

/*	Definir aqui el tiempo de espera para la recepcion de datos del modem	*/
#define	MODEM_DATA_TIMEOUT		(uint16)(0.150/TICK_SYSTEM_STATE)

/*	Definir aqui el tiempo maximo de espera para transferencia de datos entre modem y micro */
#define     CONEXION_TIMEOUT		(uint16)((10.0*60.0)/MODEM_TASK_TICK)	//	10 minutos

/*	Definir aqui el numero maximo de respuestas "TIMEOUT" permitidas antes de reiniciar el subproceso */
#define	MAX_TIMEOUT_PERMITTED	8

/*	Definir aqui el tama?o del buffer de recepci?n de informaci?n desde el modem */
#define MODEM_BUFFER_SIZE       256

#define REQUEST_QUEUE_SIZE      1

#define RESPONSE_QUEUE_SIZE     1

#define MAX_LENGHT_END_OF_FRAME 10

/**********************************************************************************************/
/*	Estructura de cola de comandos
 */

enum remitente
{
	GPRS = 0,
	SMS,
	SHELL
};

enum endOfFrames
{
    OK = 0,
    CONNECT,
    RING,
    NO_CARRIER,
    ERROR,
    CONNECT_1200
};

typedef struct
{
	char*		cmd;                ///<	puntero a la linea de comando
	char*		resp;               ///<	puntero donde se copia la respuesta al comando enviado
	uint16		timeout;            ///<	tiempo maximo de espera de la respuesta en ms
	uint16		delay;              ///<	demora para enviar el comando
	uint8		nFrames;            ///<	numero de tramas que se esperan
    uint8       sender;             ///<    id de quien solicita el comando
    uint8       expextedEndOfFrame; ///<    Final de trama que se espera recibir del modem
}cmdQueue_t;




#define     DATA_READY      1
#define     DATA_ERROR      2


/**********************************************************************************************/
/**
 * \brief
 * Envia un comando al modem si es que este no está ocupado atendiendo otro proceso.
 * @param text	cadena de texto que contiene el comando.
 * @param tx	puntero al buffer donde queda el comando almacenado hasta ser enviado.
 * @param rx	puntero al buffer donde se almacena/n la/s respuesta/s al comando.
 * @param t		tiempo de espera (s) maximo de espera de la/s respuesta/s. 
 * @param d		tiempo de demora (s) en enviar el comando al modem.
 * @param f		numero de respuestas esperadas.
 */
int16_t	SendATCommand( const char* text, char* tx, char* rx, uint16 t, uint16 d, uint8 f );

#endif	/* MODEM_H */

