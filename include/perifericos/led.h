/* 
 * File:   led.h
 * Author: Tincho
 *
 * Created on 30 de julio de 2017, 20:59
 */

#include "typedef.h"
#include "FreeRTOS.h"
#include "freeRTOS/portmacro.h"

#ifndef LED_H
#define	LED_H

#ifdef	__cplusplus
extern "C" {
#endif

#define partstDEFAULT_PORT_ADDRESS		( ( uint16_t ) 0x378 )

void vLedInitialise( void );
void vLedSetLED( UBaseType_t uxLED, BaseType_t xValue );
void vLedToggleLED( UBaseType_t uxLED );


#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

