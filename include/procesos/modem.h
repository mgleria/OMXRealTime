/* 
 * File:   conexion.h
 * Author: Juan Cruz Becerra
 *
 * Created on 28 de noviembre de 2012, 22:42
 */

#ifndef MODEM_H
#define	MODEM_H

#include	"sistema/HardwareProfile.h"
//#include	"sistema/systemState.h"
//#include	"drivers/uart_driver.h"

#if	defined	(USE_GPRS)	||	defined	(USE_SMS)

/*	DEFINIR AQUI EL PERIODO EN EL QUE SE LLAMA A LA TAREA	*/
#ifndef	MODEM_TASK_TICK
#define	MODEM_TASK_TICK		0.100
//#warning	Se define por defecto el periodo de la tarea modemTask a MODEM_TASK_TICK
//#else
//#warning	Se define el periodo de la tarea modemTask a MODEM_TASK_TICK
#endif

/*	DEFINICIONES DE TIEMPOS	*/
#define	MODEM_CMD_TIMEOUT	(unsigned int)(10.0/MODEM_TASK_TICK)

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
/*	Funciones prototipo globales	*/
void    vTaskModem( void *pvParameters );

void	SendATCommand( const char* text, char* tx, char* rx, uint16 t, uint16 d, uint8 f );
void	setConexionProcessTick();
void	uart_modem_interrupt( void );

/*	tarea para el manejo del hardware en comun entre gprs y gps */
char	conexion( void );

/*	para poder configurar por donde se desea salir con las funciones printf, ... */
extern char usr_stdout;

#endif	//	USE_xxxx

#endif	/* MODEM_H */

