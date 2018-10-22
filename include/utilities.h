/* 
 * File:   utilities.h
 * Author: Tincho
 *
 * Created on 14 de mayo de 2018, 17:17
 */

#include "freeRTOS/FreeRTOS.h"
#include "portmacro.h"

#ifndef UTILITIES_H
#define	UTILITIES_H

//#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
TickType_t xMsToTicks( TickType_t xTimeInMs);

TickType_t xSegToTicks( TickType_t xTimeInSeg);

TickType_t xMinToTicks( TickType_t xTimeInMin);

uint16_t	swapBytes( uint16_t var );
uint8_t	bcd2dec( uint8_t bcd );

void debugUART1(const char* s);

void    printMemoryPointers();

char *findNthCharacterOcurrence(const char *src,const char ch, uint8_t n);

void    flushBuffer(uint8_t *buffer, uint16_t bufferSize);


#endif	/* UTILITIES_H */

