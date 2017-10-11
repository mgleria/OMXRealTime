
#ifndef	WR_MEMORY_H
#define	WR_MEMORY_H

//#include	"HardwareProfile.h"
#include	"funciones/24LCxxx.h"

/**********************************************************************************************/
/*	Definicion de direcciones de memoria eeprom externa	*/

/**	Header start address	*/
#define	HEADER_ADD_START			0x0000
/**	Header size	*/
#define	HEADER_SIZE					1024		//	1KB

/*	configuracion del equipo	*/

#define     INT_EE_DEVICE_CONFIG	0x0000

#define     SMS_RESET_FLAG          0x0020

#define		INT_ENABLE_SENSORS_		0x0040
#define     INT_ENABLE_SENSOR_1		0x0040
#define     INT_ENABLE_SENSOR_2		0x0041
#define		INT_ENABLE_SENSOR_3		0x0042
#define		ENABLE_SENSORS_SIZE		0x4

/*	servidor	*/
#define	INT_EE_SERVER_PORT			0x0050
#define	INT_EE_SERVER_DNS			(INT_EE_SERVER_PORT + sizeof(serverInfo.port))

/*	direcciones para las APN	*/
/**	APN start address	*/
#define	INT_EE_APN_START			0x0100
/**	APN size	*/
#define	APN_SIZE					64

#define	INT_EE_APN_1				INT_EE_APN_START+(0*APN_SIZE)
#define	INT_EE_APN_2				INT_EE_APN_START+(1*APN_SIZE)
#define	INT_EE_APN_3				INT_EE_APN_START+(2*APN_SIZE)
#define	INT_EE_APN_4				INT_EE_APN_START+(3*APN_SIZE)
#define	INT_EE_APN_5				INT_EE_APN_START+(4*APN_SIZE)
#define	INT_EE_APN_6				INT_EE_APN_START+(5*APN_SIZE)
#define	INT_EE_APN_STOP				INT_EE_APN_6 + APN_SIZE

/*	Definicion de direcciones para las VPN	*/
/**	VPN start address	*/
#define	INT_EE_VPN_START			0x0300
/**	VPN size	*/
#define	VPN_SIZE					APN_SIZE

#define	INT_EE_VPN_1				INT_EE_VPN_START+(0*VPN_SIZE)
#define	INT_EE_VPN_2				INT_EE_VPN_START+(1*VPN_SIZE)

/*	Definicion de direcciones para almacenar las muestras */
/**	Samples start address	*/
#define	SAMPLES_START_ADD			0x0400
/**	Sample size	*/
#define	SAMPLES_SIZE				64

/**********************************************************************************************/
/*	Definicion de direcciones de memoria ram externa	*/
#define	EXT_RAM_

extern uint16	usbSendSamples;
extern uint16	usbSendSamplesPtr;
extern uint8    usbSendSamplesDev;

/**********************************************************************************************/
/*	para utilizar con la funcion getSample( xxxx )	*/
enum
{
	lastSample = 0,
	nextSample = 1
};

/**********************************************************************************************/
/*	Funciones prototipo de acceso externo	*/
char	getSample( muestra_t *muestra, char sample );
uint8	putSample( muestra_t* muestra );
uint16	getSamplesRead( void);
void	setSamplesRead( uint16 value );
uint16	getSamplesWrite( void );
void	setSamplesWrite( uint16 value );
uint16	getSamplesTotal( void );
void	setSamplesTotal( uint16 value );
uint16	getConnAttempts( void );
void	setConnAttempts( uint8 );

char	*read_int_eeprom_array( uint16 address, char* array, uint8 size );
void	write_int_eeprom_array( uint16 address, char* array, uint8 size );

void	resetSamplesPtr( void );
void	setDeviceSensorEnables( uint8*	p );

/*	macros */
#define	read_int_eeprom_byte( address )			Read_b_eep( address )
#define	write_int_eeprom_byte( address, data )	Write_b_eep( address, data )


#endif
