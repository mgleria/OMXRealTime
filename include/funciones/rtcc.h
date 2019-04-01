/* 
 * File:   rtcc.h
 * Author: Tincho
 *
 * Created on 3 de noviembre de 2017, 17:42
 */

#ifndef RTCC_H
#define	RTCC_H

#include "i2c1.h"
#include "mcc.h"
#include "sistema/ext_rtcc.h"
#include "utilities.h"
#include "uart1.h"

#include <libpic30.h> //Para función delay
#include <stdio.h>

/**********************************************************************************************/
/*	Variables y estructuras para el RTCC	*/
typedef	unsigned char	RTCC_HOUR;
typedef	unsigned char	RTCC_MINUTE;
typedef	unsigned char	RTCC_SECOND;
typedef	unsigned char	RTCC_DOW;
typedef	unsigned char	RTCC_DIA;
typedef	unsigned char	RTCC_MONTH;
typedef	unsigned char	RTCC_YEAR;

typedef struct {
	RTCC_HOUR	hora;
	RTCC_MINUTE minutos;
	RTCC_SECOND segundos;
	RTCC_DOW	dow;
	RTCC_DIA	dia;
	RTCC_MONTH	mes;
	RTCC_YEAR	anio;
} rtcc_t;

/**********************************************************************************************/
#define RTCC_ADDRESS            0x0068  //01101000 Justificada a derecha porque el driver i2c1 se encarga 
                                        //de desplazarla luego y añadirle el bit de dirección R/W

#define SLAVE_I2C_RTCC_TIMEOUT  500
#define BUFFER_RTCC_SIZE        16
/**********************************************************************************************/
#define	SAMPLES_READ_ADDRESS	0x10
#define	SAMPLES_WRITE_ADDRESS	0x12
#define	SAMPLES_TOTAL_ADDRESS	0x14
#define	CONNECTION_ATTEMPTS		0x16
#define	RAIN_SAVED_ADDRESS		0x20

/*	mascaras para pedir los valores de la variable tiempo */
#define	_SEC_					0x00
#define	_MIN_					0x01
#define	_HOUR_					0x02
#define	_DOW_					0x03
#define	_DAY_					0x04
#define	_MON_					0x05
#define	_YEAR_					0x06

/**********************************************************************************************/

uint8_t	read_rtcc_byte( uint8_t address, uint8_t* data);
uint8_t	write_rtcc_byte( uint8_t address, uint8_t* data);

uint8_t	read_rtcc_array( uint8_t address, uint8_t* array, uint8_t size );
uint8_t	write_rtcc_array( uint8_t address, uint8_t* array, uint8_t size );

void    rtc_init();
void    get_rtcc_datetime( rtcc_t *rtcc );
void	set_rtcc_datetime( rtcc_t* rtcc );

void    printRTCCTime( rtcc_t* rtcc );
void    printCurrentRTCCTime();

#endif	/* RTCC_H */

