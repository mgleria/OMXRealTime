
#ifndef	_24LCxxx_H
#define	_24LCxxx_H

//#include	"HardwareProfile.h"
//#include	"drivers/i2c_driver.h"
#include	"funciones/memory.h"

//************************* DEFINICIONES EEPROM EXTERNA*************************
#define  EEPROM_QTY                 2           // cantidad de eeprom disponibles en el pcb
#define  MEMORY_SIZE				(65536)		//	64 KB
#define  SAMPLES_BLOCK_SIZE         ((MEMORY_SIZE-HEADER_SIZE) / sizeof(muestra_t))
#define  MAX_SAMPLES				(SAMPLES_BLOCK_SIZE*EEPROM_QTY)

#define  _24LC512_0					0xA0		//	1010000x
#define  _24LC512_1					0xA2		//	1010001x
#define  _24LC512_2					0xA4		//	1010010x
#define  _24LC512_3					0xA6		//	1010011x
#define  _24LC512_4					0xA8		//	1010100x
#define  _24LC512_5					0xAA		//	1010101x
#define  _24LC512_6					0xAC		//	1010110x
#define  _24LC512_7					0xAE		//	1010111x

//void	init_i2c();
void	write_ext_eeprom_byte( uint16 address, uint8 data, uint8 memory );
char	read_ext_eeprom_byte( uint16 address, uint8 memory );
void	write_ext_eeprom_array( uint16 address, char* array, uint16 nbytes, uint8 device );
char	*read_ext_eeprom_array( uint16 address, char* array, uint16 nbytes, uint8 device );

#endif
