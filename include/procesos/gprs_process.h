#ifndef	GPRS_DRIVER_H
#define	GPRS_DRIVER_H

#include	"HardwareProfile.h"

#include	"procesos/sub_procesos/conexion.h"
#include	"funciones/gprs_functions.h"
#include	"procesos/mask.h"

#ifdef	USE_GPRS

/*	definir aqui el periodo en el que se ejecuta el proceso	*/
#define	GPRS_PROCESS_TICK			(0.500)

/* - Comunicación con Modem ----------------------------------------------- */
uint8	gprsProcess( void );

#define Buffer_GPRS_Size   160

#endif	//	USE_GPRS

#endif
