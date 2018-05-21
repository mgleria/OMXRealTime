#include "utilities.h"
#include "freeRTOSConfig.h"

//#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
TickType_t xMsToTicks( TickType_t xTimeInMs){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = ((uint32_t)xTimeInMs*(uint32_t)configTICK_RATE_HZ)/(uint32_t)1000;
    
    return (TickType_t) xTimeInTicks;
}

TickType_t xSegToTicks( TickType_t xTimeInSeg){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = xMsToTicks(xTimeInSeg*1000);
    
    return (TickType_t) xTimeInTicks;
}

TickType_t xMinToTicks( TickType_t xTimeInMin){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = xSegToTicks(xTimeInMin*60);
    
    return (TickType_t) xTimeInTicks;
}

/**********************************************************************************************/
/**
 * Convierte un numero en formato BCD a formato DECIMAL.
 * @param bcd	numero en bcd
 * @return		numero en decimal
 */
uint8_t	bcd2dec( uint8_t bcd )
{
	uint8_t decimal = bcd & 0xF;
	decimal += (bcd >> 4) * 10;
	return decimal;
}

/**********************************************************************************************/
/**
 * Intercambia los bytes de una variable de 16 bits.
 * @param var	variable
 * @return		bytes intercambiados
 */
uint16_t	swapBytes( uint16_t var )
{
	uint8_t temp = (uint8_t)(var >> 8);
	var <<= 8;
	var |= (uint16_t)temp;
	return	var;
}
