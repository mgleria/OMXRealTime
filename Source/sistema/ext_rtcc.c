
#include    "sistema/ext_rtcc.h"
#include    "i2c1.h"
//#include	"sensores/shtxx.h"

/**********************************************************************************************/
/*	variable del sistema con los valores de fecha y hora	*/
rtcc_t tiempo;					///<	Estructura con la fecha y hora del sistema.

static	uint8_t State = 0;
static	uint16_t StartTime = 0;


///**********************************************************************************************/
///*	funciones prototipo locales	- no deben ser accesibles desde otro archivo	*/
//char	read_rtcc_byte( uint8_t address );
//void	write_rtcc_byte( uint8_t address, char data );

/**********************************************************************************************/
////        rtc_init()     Inicializa el ds1307 habilita salida de 4khzchar ////

/******************************************************************************/
/**
 * \brief
 * Inicializa el RTCC externo.
 */
void rtc_init()
{
	uint8_t data;
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
 * Lee un arreglo desde el dispositivo RTCC
 * @param address
 * @param array
 * @param size
 */
void	read_rtcc_array( uint8_t address, char* array, uint8_t size )
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
uint8_t	write_rtcc_array( uint8_t address, uint8_t* array, uint8_t size )
{
	//BUFFER_RTCC_SIZE debe ser mayor que size ya que writeBuffer[0] es la word Address
    
    uint8_t         writeBuffer[BUFFER_RTCC_SIZE];
    uint16_t        timeOut, slaveTimeOut;
    int i;

    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    // building writeBuffer
    // starting address of the RTCC memory
    writeBuffer[0] = address;               
    // data to be written
    for(i=0;i<size;i++) writeBuffer[i+1]=array[i];

    // Now it is possible that the slave device will be slow.
    // As a work around on these slaves, the application can
    // retry sending the transaction
    timeOut = 0;
    slaveTimeOut = 0;
    
    if((BUFFER_RTCC_SIZE-size)<0) return 0;
    
    size++;

    while(status != I2C1_MESSAGE_FAIL)
    {
        // write one byte to EEPROM (3 is the number of bytes to write)
        I2C1_MasterWrite(   writeBuffer,
                            size,
                            RTCC_ADDRESS,
                            &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING)
        {
            // add some delay here

            // timeout checking
            // check for max retry and skip this byte
            if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
                break;
            }
                
            else
                slaveTimeOut++;
        } 
        if ((slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT) || 
            (status == I2C1_MESSAGE_COMPLETE))
            break;

        // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C1_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here

        // check for max retry and skip this byte
        if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX){
            break;
        }
        else
            timeOut++;
    }
    
    if(status == I2C1_MESSAGE_COMPLETE)
        return 1;
    else
        return 0;
}

/**********************************************************************************************/
/**
 * \brief
 * Lee un byte desde el dispositivo RTCC
 * @param address
 * @return
 */
uint8_t read_rtcc_byte( uint8_t address )
{    
   return 0; 
}

/**********************************************************************************************/
/**
 * \brief
 * Escribe un byte en el dispositivo RTCC
 * @param address
 * @param data
 */
uint8_t write_rtcc_byte( uint8_t address, char data )
{
	return 0;
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
uint8_t	getTimeDate( const uint8_t add )
{
	return	(uint8_t)read_rtcc_byte(add&7) & 0x7F;
}
/**********************************************************************************************/
