#include "funciones/rtcc.h"
#include "sistema/ext_rtcc.h"
#include "utilities.h"
#include "uart1.h"

rtcc_t tiempo;					///<	Estructura con la fecha y hora del sistema.

void rtc_init()
{
	uint8_t	data;
    uint8_t aux = 0x11;
    
	read_rtcc_byte(0,&data);
	data = data & 0x7F;
	write_rtcc_byte(0, &data);
	__delay_ms(2);
	write_rtcc_byte(7, &aux);
	__delay_ms(2);

	get_rtcc_datetime( &tiempo );
}

uint8_t	write_rtcc_array( uint8_t address, uint8_t* array, uint8_t size )
{
    //BUFFER_RTCC_SIZE debe ser mayor que size ya que writeBuffer[0] es la word Address
    
    uint8_t         writeBuffer[BUFFER_RTCC_SIZE];
    uint16_t        timeOut, slaveTimeOut;
    int i;
    
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    if((size+1)>BUFFER_RTCC_SIZE) return 0;
    
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
            if (slaveTimeOut == SLAVE_I2C_RTCC_TIMEOUT){
                break;
            }
                
            else
                slaveTimeOut++;
        } 
        if ((slaveTimeOut == SLAVE_I2C_RTCC_TIMEOUT) || 
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

uint8_t	read_rtcc_array( uint8_t address, uint8_t* array, uint8_t size ){
    
    I2C1_MESSAGE_STATUS status;
    uint16_t    retryTimeOut, slaveTimeOut;
    uint16_t    counter;

    status = I2C1_MESSAGE_PENDING;

    //Se mantiene el ciclo for para la utilización de las sentencias 'break'
    for (counter = 0; counter < 1; counter++)
    {

        // build the write buffer first
        // starting address of the EEPROM memory

        // Now it is possible that the slave device will be slow.
        // As a work around on these slaves, the application can
        // retry sending the transaction
        retryTimeOut = 0;
        slaveTimeOut = 0;

        while(status != I2C1_MESSAGE_FAIL)
        {
            I2C1_MasterWrite(       &address,
                                    1,
                                    RTCC_ADDRESS,
                                    &status);

            // wait for the message to be sent or status has changed.
            while(status == I2C1_MESSAGE_PENDING)
            {
                // add some delay here

                // timeout checking
                // check for max retry and skip this byte
                if (slaveTimeOut == SLAVE_I2C_RTCC_TIMEOUT){
//                    vLedSetLED(0,1); //Enciende el LED1 (D3)
                    return (1);
                }
                else
                    slaveTimeOut++;
            }

            if (status == I2C1_MESSAGE_COMPLETE)
                break;

            // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
            //               or I2C1_DATA_NO_ACK,
            // The device may be busy and needs more time for the last
            // write so we can retry writing the data, this is why we
            // use a while loop here

            // check for max retry and skip this byte
            if (retryTimeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                break;
            else
                retryTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
        {

            // this portion will read the byte from the memory location.
            retryTimeOut = 0;
            slaveTimeOut = 0;

            while(status != I2C1_MESSAGE_FAIL)
            {
                // read one byte from EEPROM (2 is the count of bytes to write)
                I2C1_MasterRead(        array,
                                        size,
                                        RTCC_ADDRESS,
                                        &status);

                // wait for the message to be sent or status has changed.
                while(status == I2C1_MESSAGE_PENDING)
                {
                    // add some delay here

                    // timeout checking
                    // check for max retry and skip this byte
                    if (slaveTimeOut == SLAVE_I2C_RTCC_TIMEOUT){
//                        vLedSetLED(1,1); //Enciende el LED1 (D4)
                        return (2);
                    }
                    else
                        slaveTimeOut++;
                }

                if (status == I2C1_MESSAGE_COMPLETE)
//                    vLedSetLED(1,1); //Enciende el LED1 (D4)
                    break;

                // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                //               or I2C1_DATA_NO_ACK,
                // The device may be busy and needs more time for the last
                // write so we can retry writing the data, this is why we
                // use a while loop here

                // check for max retry and skip this byte
                if (retryTimeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                    break;
                else
                    retryTimeOut++;
            }
        }

        // exit if the last transaction failed
        if (status == I2C1_MESSAGE_FAIL)
        {
//            vLedSetLED(2,1); //Enciende el LED1 (D5)
            return(3);
            break;
        }
    }
//    vLedSetLED(3,1); //Enciende el LED1 (D6)
    return(4);
}

/**********************************************************************************************/
/**
 * \brief
 * Lee un byte en el dispositivo RTCC
 * @param address
 * @param array
 */
uint8_t	read_rtcc_byte( uint8_t address, uint8_t* data)
{
	return read_rtcc_array(address, data, 1);
}

/**********************************************************************************************/
/**
 * \brief
 * Escribe un byte en el dispositivo RTCC
 * @param address
 * @param array
 */
uint8_t	write_rtcc_byte( uint8_t address, uint8_t* data)
{
	return write_rtcc_array( address, data, 1);
}

/**********************************************************************************************/
/**
 * \brief
 * Lee la hora desde el dispositivo RTCC
 * @param rtcc
 */
void	get_rtcc_time( rtcc_t* rtcc )
{   
    //Uso aritmética de punteros para pasar por referencia los campos del struct 'tiempo'
    read_rtcc_byte(_HOUR_, (uint8_t*)rtcc); //el comienzo de la estructura 'tiempo' en memoria apunta al campo 'hora'
    read_rtcc_byte(_MIN_, (uint8_t*)rtcc+1);//'rtcc+1' = campo 'minutos'
    read_rtcc_byte(_SEC_, (uint8_t*)rtcc+2);//'rtcc+2' = campo 'segundos'
    rtcc->hora		&=	0x3F;
	rtcc->minutos	&=	0x7F;
	rtcc->segundos	&=	0x7F;
//	rtcc->hora		=	(read_rtcc_byte(2) & 0x3F);
//	rtcc->minutos	=	(read_rtcc_byte(1) & 0x7F);
//	rtcc->segundos	=	(read_rtcc_byte(0) & 0x7F);
}

/**********************************************************************************************/
/**
 * \brief
 * Lee la fecha desde el RTCC
 * @param rtcc
 */
void	get_rtcc_date( rtcc_t* rtcc )
{
    //Uso aritmética de punteros para pasar por referencia los campos del struct 'tiempo'
    read_rtcc_byte(_DOW_, (uint8_t*)rtcc+3);//'rtcc+2' = campo 'segundos'
    read_rtcc_byte(_DAY_, (uint8_t*)rtcc+4);//'rtcc+2' = campo 'segundos'
    read_rtcc_byte(_MON_, (uint8_t*)rtcc+5);//'rtcc+1' = campo 'minutos'
    read_rtcc_byte(_YEAR_, (uint8_t*)rtcc+6); //'rtcc+1' = campo 'minutos'
    
//    rtcc->anio	&=	(read_rtcc_byte(6));
	rtcc->mes	&=	0x1F;
	rtcc->dia	&=	0x3F;
	rtcc->dow	&=	0x07;
}

/**********************************************************************************************/

void    get_rtcc_datetime( rtcc_t *rtcc )
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
    uint8_t seg,min,hour;
    seg     = rtcc->segundos & 0x7F;
    min     = rtcc->minutos & 0x7F;
    hour    = rtcc->hora & 0x3F;
    
    write_rtcc_byte(_SEC_,&seg);
    write_rtcc_byte(_MIN_,&min);
    write_rtcc_byte(_HOUR_,&hour);
//  write_rtcc_byte( 0, rtcc->segundos & 0x7F);
//	write_rtcc_byte( 1, rtcc->minutos & 0x7F);
//	write_rtcc_byte( 2, rtcc->hora & 0x3F);
}

/**********************************************************************************************/
/**
 * \brief
 * 
 * @param rtcc
 */
void	set_rtcc_date( rtcc_t *rtcc )
{
    uint8_t day,month,year,dow;
    dow     = rtcc->dow;
    day     = rtcc->dia & 0x3F;
    month   = rtcc->mes & 0x1F;
    year    = rtcc->anio;
    
    write_rtcc_byte(_DOW_,&dow);
    write_rtcc_byte(_DAY_,&day);
    write_rtcc_byte(_MON_,&month);
    write_rtcc_byte(_YEAR_,&year);
//	write_rtcc_byte( 3, rtcc->dow );
//	write_rtcc_byte( 4, rtcc->dia & 0x3F);
//	write_rtcc_byte( 5, rtcc->mes & 0x1F);
//	write_rtcc_byte( 6, rtcc->anio );
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

void printRTCCTime( rtcc_t* rtcc )
{
    #define AUX_BUFFER_SIZE 20
    char auxBuffer[AUX_BUFFER_SIZE];
    flushBuffer(auxBuffer,AUX_BUFFER_SIZE);
    
    sprintf(auxBuffer,"%d/%d/%d %d:%d:%d\r\n", 
            bcd2dec(rtcc->dia), 
            bcd2dec(rtcc->mes), 
            bcd2dec(rtcc->anio), 
            bcd2dec(rtcc->hora), 
            bcd2dec(rtcc->minutos), 
            bcd2dec(rtcc->segundos));
    
//    UART1_WriteBuffer(auxBuffer,AUX_BUFFER_SIZE);
    debug(auxBuffer);
    
}

void printCurrentRTCCTime()
{
    rtcc_t now;
    get_rtcc_datetime(&now);
    printRTCCTime(&now);
}

/**********************************************************************************************/
/**
 * 
 * @param add
 * @return
 */
//uint8	getTimeDate( const uint8 add )
//{
//	return	(uint8)read_rtcc_byte(add&7) & 0x7F;
//}

/**********************************************************************************************/