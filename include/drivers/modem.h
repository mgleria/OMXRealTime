/* 
 * File:   conexion.h
 * Author: Juan Cruz Becerra
 *
 * Created on 28 de noviembre de 2012, 22:42
 */

#ifndef MODEM_H
#define	MODEM_H

#include    <stdio.h>
#include    <string.h>

#include	"sistema/HardwareProfile.h"
#include    "utilities.h"
#include    "tmr5.h"

#include    "ezbl.h"

#include    "freeRTOS/FreeRTOS.h"
#include    "freeRTOS/semphr.h"
#include    "task.h"

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

void    setupModem();
/**********************************************************************************************/
/**
 * \brief
 * Envia un comando al modem si es que este no está ocupado atendiendo otro proceso.
 * @param cmd	cadena de texto que contiene el comando.
 * @param d		tiempo de demora (s) en enviar el comando al modem.
 */
int16_t	SendATCommand( const char* cmd, uint16 d);
/**
 * \brief
 * Recibe la respuesta del modem a un comando AT
 * @param buffer	buffer donde se almacenará la respuesta del modem
 * @param attempts	puntero a la cantidad de intentos restantes
 * @param responseDelay	espera tolerable para el comando
 */
uint8_t	receiveATCommand( char* buffer, uint8_t *attempts, TickType_t responseDelay );

#endif	/* MODEM_H */

