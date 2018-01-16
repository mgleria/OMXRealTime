
#include	"funciones/puts.usb.h"
//#include	"drivers/uart_driver.h"
#include	<stdio.h>
#include	<stdlib.h>

/**********************************************************************************************/
/**
 * \brief
 * Imprime en la salida USB un texto con filtro para ser impreso en el LOG del software.
 *
 * @param str	texto a imprimir.
 */
void	putsLog( string* str)
{
//	if( FTDI_EN )	return;
//	usr_stdout = _UART2_;
//	printf("|LOG|%s|end|",str);
}

/**********************************************************************************************/
/**
 * \brief
 * Imprime en la salida USB un texto con filtro para ser impreso en el la pestaña de procesos
 * del software.
 *
 * @param process	numero de proceso.
 * @param str		texto que indica el estado del proceso.
 */
void	putsProcessState( uint8 process, string* str)
{
//	if( FTDI_EN )	return;
//	usr_stdout = _UART2_;
//	printf( "--process:%u_%s\n",process,str );
}

