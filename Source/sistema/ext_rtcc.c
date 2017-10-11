
#include	"sistema/ext_rtcc.h"
#include "i2c1.h"
//#include	"sensores/shtxx.h"

/**********************************************************************************************/
/*	variable del sistema con los valores de fecha y hora	*/
rtcc_t tiempo;					///<	Estructura con la fecha y hora del sistema.

static	uint8_t State = 0;
static	uint16_t StartTime = 0;
static	uint16_t address = (0xD0 / 2);	/* //Dirección del dispositivo 0b1101000x. 
                                        * El bit 'x' indica escritura (0) o lectura (1) */
//static	uint8_t length = 1;
//static	uint8_t command = 0,	data = 0;


/**********************************************************************************************/
/*	funciones prototipo locales	- no deben ser accesibles desde otro archivo	*/
char	read_rtcc_byte( uint8 address );
void	write_rtcc_byte( uint8 address, char data );

/**********************************************************************************************/
////        rtc_init()     Inicializa el ds1307 habilita salida de 4khzchar ////

/******************************************************************************/
/**
 * \brief
 * Inicializa el RTCC externo.
 */
void rtc_init()
{
	auto char data;
	data = read_rtcc_byte(0);
	data = data & 0x7F;
	write_rtcc_byte(0, data);
//	__delay_ms(2);
	write_rtcc_byte(7, 0x11);
//	__delay_ms(2);

	get_rtcc_datetime( &tiempo );
}

/**********************************************************************************************/
/**
 * \brief
 * Lee un byte desde el dispositivo RTCC
 * @param address
 * @return
 */
char read_rtcc_byte( uint8 address )
{    
    
    static	I2C1_MESSAGE_STATUS I2C_Wflag;
    
    
    //Dirección del dispositivo 0b1101000x. El bit 'x' indica escritura (0) o lectura (1)
	auto char device = 0xD0; 
	#define	WRITE(byte)		(byte &= 0xFE)
	#define	READ(byte)		(byte |= 0x01)

    

    I2C1_MESSAGE_STATUS status;
//    uint8_t     *pData;
    uint16_t    device16;
    uint8_t     receivedData;
//    uint8_t     writeBuffer[2];
    
//    writeBuffer[0] = 
    
    
    device16 = WRITE(device);
//    pData = &address;
/*void I2C1_MasterWrite(
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address,
                                I2C1_MESSAGE_STATUS *pstatus);*/    
    
    I2C1_MasterWrite(   &address,
                        1,
                        device16,
                        &status);
    
    if (I2C_Wflag == I2C1_MESSAGE_FAIL){
        while (1)		// Something wrong
            vLedSetLED(1,1); //Enciende el LED1 (D4)
    }
    
    while (status != I2C1_MESSAGE_COMPLETE); //Espera que termine
    
    device16 = READ(device);
    
    /*void I2C1_MasterRead(
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address,
                                I2C1_MESSAGE_STATUS *pstatus);*/
    
    I2C1_MasterRead(    &receivedData,
                        1,
                        device16,
                        &status);
    
    return (char)status;    
    
    
//	IdleI2C();									// ensure module is idle
//	StartI2C();									// initiate START condition
//	i2c_write_byte( WRITE(device) );
//	i2c_write_byte( address );
//	RestartI2C();
//	i2c_write_byte( READ(device) );
//	return	i2c_read_byte( NOACK );

	#undef	WRITE(byte)
	#undef	READ(byte)	
}

/**********************************************************************************************/
/**
 * \brief
 * Escribe un byte en el dispositivo RTCC
 * @param address
 * @param data
 */
void	write_rtcc_byte( uint8 address, char data )
{
	#define	WRITE(byte)		(byte &= 0xFE)
    static	uint8_t writeBuffer[2];
    static	I2C1_MESSAGE_STATUS I2C_Wflag;
    auto char device = 0xD0;
		
    
    uint16_t device16 = WRITE(device);
    
    // build the write buffer first
    // starting address of the EEPROM memory
    writeBuffer[0] = address;                
    writeBuffer[1] = (uint8)data;                       
            
    /* Start a I2C Write */
    I2C1_MasterWrite(   writeBuffer,
                        2,
                        device16, 
                        &I2C_Wflag);
        
    if (I2C_Wflag == I2C1_MESSAGE_FAIL)
        while (1)		// Something wrong
            vLedSetLED(2,1); //Enciende el LED2 (D5)
    while (I2C_Wflag != I2C1_MESSAGE_COMPLETE); //Espera que termine

//	IdleI2C();									// ensure module is idle
//	StartI2C();									// initiate START condition
//	i2c_write_byte( WRITE(device) );
//	i2c_write_byte( address );
//	i2c_write_byte( data );
//	IdleI2C();									// ensure module is idle
//	StopI2C();									// send STOP condition

	#undef	WRITE(byte)
}

/**********************************************************************************************/
/**
 * \brief
 * Lee la hora desde el dispositivo RTCC
 * @param rtcc
 */
void	get_rtcc_time( rtcc_t* rtcc )
{
	rtcc->hora		=	(read_rtcc_byte(2) & 0x3F);
	rtcc->minutos	=	(read_rtcc_byte(1) & 0x7F);
	rtcc->segundos	=	(read_rtcc_byte(0) & 0x7F);
}

/**********************************************************************************************/
/**
 * \brief
 * Lee la fecha desde el RTCC
 * @param rtcc
 */
void	get_rtcc_date( rtcc_t* rtcc )
{
	rtcc->anio	=	(read_rtcc_byte(6));
	rtcc->mes	=	(read_rtcc_byte(5) & 0x1F);
	rtcc->dia	=	(read_rtcc_byte(4) & 0x3F);
	rtcc->dow	=	(read_rtcc_byte(3) & 0x07);
}

/**********************************************************************************************/

void	get_rtcc_datetime( rtcc_t *rtcc )
{
	get_rtcc_time( rtcc );
	get_rtcc_date( rtcc );
}

/**********************************************************************************************/
/**
 *
 * @param rtcc
 */
void	set_rtcc_time( rtcc_t *rtcc )
{
	write_rtcc_byte( 0, rtcc->segundos & 0x7F);
	write_rtcc_byte( 1, rtcc->minutos & 0x7F);
	write_rtcc_byte( 2, rtcc->hora & 0x3F);
}

/**********************************************************************************************/
/**
 * \brief
 * 
 * @param rtcc
 */
void	set_rtcc_date( rtcc_t *rtcc )
{
	write_rtcc_byte( 3, rtcc->dow );
	write_rtcc_byte( 4, rtcc->dia & 0x3F);
	write_rtcc_byte( 5, rtcc->mes & 0x1F);
	write_rtcc_byte( 6, rtcc->anio );
}

/**********************************************************************************************/
/**
 * \brief
 * 
 * @param rtcc
 */
void	set_rtcc_datetime( rtcc_t* rtcc )
{
	set_rtcc_date( rtcc );
	set_rtcc_time( rtcc );
}

/**********************************************************************************************/
/**
 * 
 * @param add
 * @return
 */
uint8	getTimeDate( const uint8 add )
{
	return	(uint8)read_rtcc_byte(add&7) & 0x7F;
}

/**********************************************************************************************/
/**
 * \brief
 * Lee un arreglo desde el dispositivo RTCC
 * @param address
 * @param array
 * @param size
 */
void	read_rtcc_array( uint8 address, char* array, uint8 size )
{
	while( size-- )
		*array++ = read_rtcc_byte( address++ );
}

/**********************************************************************************************/
/**
 * \brief
 * Escribe un arreglo en el dispositivo RTCC
 * @param address
 * @param array
 * @param size
 */
void	write_rtcc_array( uint8 address, char* array, uint8 size )
{
	while( size-- )
		write_rtcc_byte( address++, *array++ );
}

/**********************************************************************************************/
