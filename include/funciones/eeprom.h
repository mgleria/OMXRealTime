/* 
 * File:   eeprom.h
 * Author: Tincho
 *
 * Created on 1 de noviembre de 2017, 16:15
 */

#ifndef EEPROM_H
#define	EEPROM_H

#define  EEPROM_QTY                 2           // cantidad de eeprom disponibles en el pcb
#define  MEMORY_SIZE				(65536)		//	64 KB
#define  SAMPLES_BLOCK_SIZE         ((MEMORY_SIZE-HEADER_SIZE) / sizeof(muestra_t))
#define  MAX_SAMPLES				(SAMPLES_BLOCK_SIZE*EEPROM_QTY) 

#define  _24LC512_0					0x50		//	01010000
#define  _24LC512_1					0x51		//	01010001
#define  _24LC512_2					0x52		//	01010010
#define  _24LC512_3					0x53		//	01010011
#define  _24LC512_4					0x54		//	01010100
#define  _24LC512_5					0x55		//	01010101
#define  _24LC512_6					0x56		//	01010110
#define  _24LC512_7					0x57		//	01010111
 
#define SLAVE_I2C_GENERIC_RETRY_MAX         500
#define SLAVE_I2C_GENERIC_DEVICE_TIMEOUT    500

#define BUFFER_SIZE                         18      //2 más de la cantidad de datos que pretendo transmitir

uint8_t MCHP_24LCxxx_Init_I2C1(uint8_t chip);
uint8_t MCHP_24LCxxx_Write_byte(uint8_t chip,uint16_t address,uint8_t *pData);
uint8_t MCHP_24LCxxx_Write_array(uint8_t chip,uint16_t address,uint8_t *pData,uint16_t nCount);
uint8_t MCHP_24LCxxx_Read_byte(uint8_t chip,uint16_t address,uint8_t *pData);
uint8_t MCHP_24LCxxx_Read_array(uint8_t chip,uint16_t address,uint8_t *pData, uint16_t nCount);


#endif	/* EEPROM_H */

