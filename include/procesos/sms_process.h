/* 
 * File:   sms.h
 * Author: Juan Cruz
 *
 * Created on 17 de enero de 2013, 23:19
 */

#ifndef SMS_H
#define	SMS_H

#include	"HardwareProfile.h"
#include	"procesos/sub_procesos/conexion.h"
#include	"procesos/mask.h"

#ifdef	USE_SMS
/*
#define	Buffer_SMS_HEAD_Size	32
#define Buffer_SMS_RESP_Size	160
#define	Buffer_SMS_FOOT_Size	8
*/
#define Buffer_SMS_Size			256


/*	definir aqui el periodo en el que se ejecuta el proceso	*/
#define	SMS_PROCESS_TICK			(0.500)

/**********************************************************************************************/
/*	funciones prototipo accesibles desde otros archivos	*/
char	smsProcess( void );

#endif	//	USE_SMS

#endif	/* SMS_H */
