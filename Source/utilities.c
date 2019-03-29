#include "utilities.h"
#include "freeRTOSConfig.h"
#include "ezbl.h"
#include <string.h>
#include "uart1.h"

//#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
TickType_t xMsToTicks( TickType_t xTimeInMs){
    
    uint32_t xTimeInTicks =0;
    
    xTimeInTicks = ((uint32_t)xTimeInMs*(uint32_t)configTICK_RATE_HZ)/(uint32_t)configTICK_RATE_HZ;
    
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

void debug(const char* s){
    
//    UART1_WriteBuffer(s,strlen(s));
//    UART1_Write(0x0D); // \r
//    UART1_Write(0x0A); // \n
//     EZBL_printf("\n %s",s);
    EZBL_printf("\n");
    EZBL_printf(s);
}

void printMemoryPointers()
{
//    EZBL_printf("\n fifoSize: %d", EZBL_STDOUT->fifoSize);
    EZBL_printf("\n%-8s%-8s%-8s", "Total", "Read", "Write"); 
    EZBL_printf("\n%-8d%-8d%-8d", getSamplesTotal(), getSamplesRead(), getSamplesWrite());
    //    UART1_TX_FIFO_WaitUntilFlushed();
}

char *findNthCharacterOcurrence(const char *src,const char ch, uint8_t n)
{
    char *p = src;
    int count;
    for (count = 0; ; ++count) {
        p = strchr(p, ch);
        if (!p || count == n)
            break;
        p++;
    }
    return p;
}

void    flushBuffer(uint8_t *buffer, uint16_t bufferSize)
{
    int i;
    for(i=0;i<bufferSize;i++){
        buffer[i]='\0';
    }
}
