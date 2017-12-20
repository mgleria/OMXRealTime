
/*	application includes	*/
#include    "typedef.h"
#include	"funciones/memory.h"
#include	"funciones/rtcc.h"
#include    "funciones/eeprom.h"



/*	standard includes	*/
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<stdio.h>

/**
 * \brief
 * Se agrega contador de muestras perdidas.
 *
 * \bugs
 * -	Se corrige logica de funcionamiento al calcular las muestras almacenadas.\n
 *		No se tenia en cuenta cuando se producia un sobre paso del puntero de escritura al
 *		puntero de lectura.
 */

/**********************************************************************************************/
/*	Definicion de parametros de las memorias	*/

/*	Memoria interna	*/


/*	Memoria externa	*/
//#define	MEMORY_UNO	0xA0

/**********************************************************************************************/
/*	Variables locales a este archivo	*/
static uint16	samplesRead = 0;        		///<	contador de muestras enviadas al servidor
static uint16	samplesWrite = 0;           	///<	contador de muestras guardadas en memoria
static uint16	samplesTotal = 0;           	///<	contador de muestras sin enviar al servidor
static uint16	samplesLoss = 0;            	///<	contador de muestras perdidas
static uint16	connAttempts = 0;           	///<	contador de intentos de conexion con el servidor
static uint16	usbSendSamples = 0;             ///<	contador de muestras a enviar por USB por cmd
static uint16	usbSendSamplesPtr = 0;          ///<	puntero muestra para enviar por USB por cmd
static uint8    usbSendSamplesDev = _24LC512_0; ///<    memoria desde donde leer las muestras a enviar por USB por cmd

/**********************************************************************************************/
/**
 * \brief
 * Escribe una cadena de bytes en la memoria eeprom interma
 * @param address	direccion de almacenamiento del 1er byte
 * @param array		puntero a la cadena
 * @param size		cantidad de bytes a escribir
 */
//void	write_int_eeprom_array( uint16 address, char* array, uint8 size )
//{
//	while( size-- )
//		write_int_eeprom_byte( address++, *array++ );
//}

/**********************************************************************************************/
/**
 * 
 * @param address	direccion de almacenamiento del 1er byte a leer
 * @param array		puntero donde se almacenan los bytes leidos
 * @param size		cantidad de bytes a leer
 * @return
 */
//char	*read_int_eeprom_array( uint16 address, char* array, uint8 size )
//{
//	while( size-- )
//		(*array++) = read_int_eeprom_byte( address++ );
//	return (array);
//}

/**********************************************************************************************/
/**
 * \brief
 * Se obtiene una muestra almacenada en memoria.\n
 * @param muestra	puntero a estructura del tipo muestra_t
 * @param sample	0: ultima muestra almacenada, si no suma el valor al puntero
 *					de lectura para la proxima muestra. Esto no controla si la proxima
 *					muestra ha sido almacenada o no.
 * @return			false si no hay mas muestras
 */
char	getSample( muestra_t *muestra, char sample )
{
    auto uint8 memDevice = _24LC512_0;
    auto uint8 memNumber = 0; 
    uint8_t returnValue = 10; //SACAR
    
	//	actualiza los valores desde la RAM del RTCC
	read_rtcc_array( SAMPLES_READ_ADDRESS, (uint8_t*)&samplesRead, sizeof(samplesRead) );
	read_rtcc_array( SAMPLES_WRITE_ADDRESS, (uint8_t*)&samplesWrite, sizeof(samplesWrite) );
	read_rtcc_array( SAMPLES_TOTAL_ADDRESS, (uint8_t*)&samplesTotal, sizeof(samplesTotal) );

    // actualiza el nùmero de memoria donde debe leer
    memNumber = (uint8)(samplesRead / SAMPLES_BLOCK_SIZE);
    memDevice += memNumber * 2;
    
	if( !samplesTotal )
		return false;
	
	samplesRead += (sint16)sample;			//	proxima muestra
	if( samplesRead >= MAX_SAMPLES )
		samplesRead = 0;

	/*	si pE >= pL y no hay overfloat	*/
	if( (samplesWrite >= samplesRead) && (samplesTotal != MAX_SAMPLES) )
		samplesTotal = samplesWrite - samplesRead;
	/*	si pE < pL o hay overfloat	*/
	else
		samplesTotal = (samplesWrite + MAX_SAMPLES) - samplesRead;
	
	/*	carga la direccion desde donde se va a leer	*/
	auto uint16 read_address = (samplesRead - (SAMPLES_BLOCK_SIZE * memNumber)) * sizeof(muestra_t);
	read_address += SAMPLES_START_ADD;
    
	if( samplesTotal )						//	si hay muestras almacenadas
        returnValue = MCHP_24LCxxx_Read_array(memDevice,read_address,(uint8_t*)muestra,sizeof(muestra_t));
//		returnValue = MCHP_24LCxxx_Read_array(memDevice,read_address,(uint8_t*)muestra,sizeof(muestra_t));
//        i2cbus_read( memDevice, read_address, (char*)muestra, (uint16)sizeof(muestra_t) );

	//	actualiza los valores en la RAM del RTCC
	write_rtcc_array( SAMPLES_READ_ADDRESS, (char*)&samplesRead, sizeof(samplesRead) );
	write_rtcc_array( SAMPLES_WRITE_ADDRESS, (char*)&samplesWrite, sizeof(samplesWrite) );
	write_rtcc_array( SAMPLES_TOTAL_ADDRESS, (char*)&samplesTotal, sizeof(samplesTotal) );

	if( !samplesTotal )
	{
		samplesLoss = 0;
		return false;
	}

//	return	true;
    return returnValue;
}
/******************************************************************************/
/**
 * \brief
 * Guarda una muestra en la memoria EEPROM externa. El almacenamiento es auto incremental. \n
 * Si no puede guardarla, no actualiza los punteros.
 * @param	muestra	puntero a estructura del tipo muestra_t
 * @return	true si la muestra se guardo exitosamente, false si no.
 */
uint8 putSample( muestra_t* muestra )
{
	uint8_t r1,r2;
	uint8_t intentos = 0;
	uint8_t	readed[sizeof(muestra_t)];
    
    uint8_t memDevice = _24LC512_0;
    uint8_t memNumber = 0;
    
    uint8_t sizeMuestra = sizeof(muestra_t);

	//	actualiza los valores de punteros desde la RAM del RTCC
	read_rtcc_array( SAMPLES_READ_ADDRESS, (char*)&samplesRead, sizeof(samplesRead) );
	read_rtcc_array( SAMPLES_WRITE_ADDRESS, (char*)&samplesWrite, sizeof(samplesWrite) );
	read_rtcc_array( SAMPLES_TOTAL_ADDRESS, (char*)&samplesTotal, sizeof(samplesTotal) );
    
    // actualiza el nùmero de memoria donde debe escribir
    memNumber = (uint8_t)(samplesWrite / SAMPLES_BLOCK_SIZE);
    memDevice += memNumber * 2;
    
        
	/*	carga la direccion donde se va a guardar la muestra	*/
	auto uint16 write_address = (samplesWrite - (SAMPLES_BLOCK_SIZE * memNumber)) * sizeof(muestra_t);
	write_address += SAMPLES_START_ADD;
      
	
	/*	guarda la muestra y en caso de no coincidir lo guardado con la muestra,
	 *	intenta 5 veces como maximo
	 */
	/**	Definir aqui el maximo de intentos de conexion con el servidor antes de reiniciar el equipo.	*/
	#define	MAX_ATTEMPS		5
	do
	{
		/*	guarda la muestra	*/
        r1 = MCHP_24LCxxx_Write_array(memDevice,write_address,(uint8_t*)muestra,sizeof(muestra_t));
//		i2cbus_write( memDevice, write_address, (char*)muestra, sizeof(muestra_t) );
		/*	lee lo que acaba de guardar	*/
        __delay_ms(2);
		r2 = MCHP_24LCxxx_Read_array(memDevice, write_address,readed,sizeof(readed));

        
        
//        s = i2cbus_read( memDevice, write_address, readed, sizeof(readed) );
	}while( strncmp( (char*)muestra,(char*)readed, sizeof(muestra_t) ) && ((intentos++) < MAX_ATTEMPS) );

	/*	si se superan los intentos retorna un false	*/
	if( intentos >= MAX_ATTEMPS )
		return false;
    #undef	MAX_ATTEMPS

	/*	si el puntero de escritura llega a la maxima direccion de almacenamiento	*/
	if( (++samplesWrite) >= MAX_SAMPLES )
		samplesWrite = 0;

	/*	si pE > pL y no hay overflow	*/
	if( (samplesWrite > samplesRead) && (samplesTotal != MAX_SAMPLES) )
		samplesTotal = samplesWrite - samplesRead;
	/*	si pE <= pL o si hay overflow	*/
	else
		/*	si pE <= pL y no hay overflow*/
		if( samplesTotal < MAX_SAMPLES )
			samplesTotal = (samplesWrite + MAX_SAMPLES) - samplesRead;
		/*	si pE <= pL y si hay overfloat, pL sigue a pE	*/
		else
		{
			samplesRead = samplesWrite;
			samplesTotal = MAX_SAMPLES;
			samplesLoss ++;
		}

	//	actualiza los valores de los punteros en la RAM del RTCC
	write_rtcc_array( SAMPLES_READ_ADDRESS, (char*)&samplesRead, sizeof(samplesRead) );
	write_rtcc_array( SAMPLES_WRITE_ADDRESS, (char*)&samplesWrite, sizeof(samplesWrite) );
	write_rtcc_array( SAMPLES_TOTAL_ADDRESS, (char*)&samplesTotal, sizeof(samplesTotal) );
	
	return	true;
}

/**********************************************************************************************/
/**
 * \brief
 * Devuelve el valor del puntero de escritura de muestras.
 * @return
 */
uint16	getSamplesWrite( void )
{
	read_rtcc_array( SAMPLES_WRITE_ADDRESS, (char*)&samplesWrite, sizeof(samplesWrite) );
	return	samplesWrite;
}

/**********************************************************************************************/
/**
 * 
 * @return 
 */
void	setSamplesWrite( uint16 value )
{
	samplesWrite = value;
	write_rtcc_array( SAMPLES_WRITE_ADDRESS, (char*)&samplesWrite, sizeof(samplesWrite) );
}

/**********************************************************************************************/
/**
 * \brief
 * Devuelve el valor del puntero de lectura de muestras.
 * @return
 */
uint16	getSamplesRead( void)
{
	read_rtcc_array( SAMPLES_READ_ADDRESS, (char*)&samplesRead, sizeof(samplesRead) );
	return	samplesRead;
}

/**********************************************************************************************/
/**
 *
 * @return
 */
void	setSamplesRead( uint16 value )
{
	samplesRead = value;
	write_rtcc_array( SAMPLES_READ_ADDRESS, (char*)&samplesRead, sizeof(samplesRead) );
}

/**********************************************************************************************/
/**
 * \brief
 * Devuelve la cantidad de muestas almacenadas
 * @return
 */
uint16	getSamplesTotal( void )
{
	read_rtcc_array( SAMPLES_TOTAL_ADDRESS, (char*)&samplesTotal, sizeof(samplesTotal) );
	return	samplesTotal;
}

/**********************************************************************************************/
/**
 *
 * @return
 */
void	setSamplesTotal( uint16 value )
{
	samplesTotal = value;
	write_rtcc_array( SAMPLES_TOTAL_ADDRESS, (char*)&samplesTotal, sizeof(samplesTotal) );
}

/**********************************************************************************************/
/**
 * \brief
 * Devuelve la cantidad de intentos de conexion con el servidor
 * @return
 */
uint16	getConnAttempts( void )
{
	read_rtcc_array( CONNECTION_ATTEMPTS, (char*)&connAttempts, sizeof(connAttempts) );
	return	connAttempts;
}

/**********************************************************************************************/
/**
 * \brief
 * Incremente los intentos de conexion cuando el equipo no logra conectarse con el servidor.
 * @param n	Si (n != 0) se incrementa el valor del contador de intententos. Si (n = 0) el
 *						contador de intentos es borrado.
 */
void	setConnAttempts( uint8 n )
{
	if( n )
		connAttempts ++;
	else
		connAttempts = 0;
	write_rtcc_array( CONNECTION_ATTEMPTS, (char*)&connAttempts, sizeof(connAttempts) );
}

/**********************************************************************************************/
/**
 * \brief
 * configuracion de fabrica del equipo
 */
void	resetSamplesPtr( void )
{
	setSamplesRead( 0 );
	setSamplesWrite( 0 );
	setSamplesTotal( 0 );
	setConnAttempts( 0 );
}

/**********************************************************************************************/
/**
 * \brief
 * Modifica los bits de habilitacion de los sensores.\n
 * Solo son modificados en caso que cambien, por lo tanto puede llamarse a la funcion
 * cuantas veces sea necesario. 
 * @param p
 */
void	setDeviceSensorEnables( uint8* p )
{
	auto uint8 aux;
    char *end;
	auto char sensor[3] = {0,0,0};
	sensor[0] = *(p + 0);	//	digito 1
	sensor[1] = *(p + 1);	//	digito 2
//	auto uint8 data = xtoi( sensor );
    uint8 data = (uint8)strtol(sensor,&end,16); //No existe xtoi() en XC16

	MCHP_24LCxxx_Read_byte(_24LC512_0,INT_ENABLE_SENSOR_1, &aux);
//    i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_1, &aux, sizeof (aux) );
	if( data != aux )
        MCHP_24LCxxx_Write_byte( _24LC512_0, INT_ENABLE_SENSOR_1, &data);
//        i2cbus_write( _24LC512_0, INT_ENABLE_SENSOR_1, &data, sizeof (data) );

	sensor[0] = *(p + 2);	//	digito 3
	sensor[1] = *(p + 3);	//	digito 4
//	data = xtoi( sensor );
    data = (uint8)strtol(sensor,&end,16); //No existe xtoi() en XC16
    
    MCHP_24LCxxx_Read_byte(_24LC512_0, INT_ENABLE_SENSOR_2, &aux);
//	i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_2, &aux, sizeof (aux) );
	
	if( data != aux )
        MCHP_24LCxxx_Write_byte( _24LC512_0, INT_ENABLE_SENSOR_2, &data);
//    	i2cbus_write( _24LC512_0, INT_ENABLE_SENSOR_2, &data, sizeof (data) );
		
	sensor[0] = *(p + 4);	//	digito 5
	sensor[1] = *(p + 5);	//	digito 6
//	data = xtoi( sensor );
    data = (uint8)strtol(sensor,&end,16); //No existe xtoi() en XC16
    MCHP_24LCxxx_Read_byte(_24LC512_0, INT_ENABLE_SENSOR_3, &aux);
//	i2cbus_read( _24LC512_0, INT_ENABLE_SENSOR_3, &aux, sizeof (aux) );

	if( data != aux )
        MCHP_24LCxxx_Write_byte( _24LC512_0, INT_ENABLE_SENSOR_3, &data);
//		i2cbus_write( _24LC512_0, INT_ENABLE_SENSOR_3, &data, sizeof (data) );
		
}
