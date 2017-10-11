#ifndef	RTCC_H
#define	RTCC_H

//#include	"HardwareProfile.h"
#include "typedef.h"

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

//extern rtcc_t tiempo;

/**********************************************************************************************/
/*	Funciones prototipo accesibles externamente */

void	rtc_init();

uint8	getTimeDate( uint8 );

void	get_rtcc_time( rtcc_t* rtcc );
void	get_rtcc_date( rtcc_t* rtcc );
void	get_rtcc_datetime(rtcc_t* rtcc);

void	set_rtcc_time( rtcc_t *rtcc );
void	set_rtcc_date( rtcc_t *rtcc );
void	set_rtcc_datetime( rtcc_t* rtcc );

void	read_rtcc_array( uint8 address, char* array, uint8 size );
void	write_rtcc_array( uint8 address, char* array, uint8 size );

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

#endif
