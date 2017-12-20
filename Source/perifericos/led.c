/* Scheduler includes. */
#include "FreeRTOS.h"

/* Demo app includes. */
#include "perifericos/led.h"

#define ptOUTPUT 	0x0
#define ptALL_OFF	0x0

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

void vLedInitialise( void )
{
//	/* The explorer 16 board has LED's on port A.  Only the led's bits are set as output
//	so PORTA is read-modified-written directly. */
// 	TRISA.TRISA0 = ptOUTPUT;
//    TRISA.TRISA1 = ptOUTPUT;
//    TRISA.TRISA2 = ptOUTPUT;
//    TRISA.TRISA3 = ptOUTPUT;
//    TRISA.TRISA4 = ptOUTPUT;
//    TRISA.TRISA5 = ptOUTPUT;
//    TRISA.TRISA6 = ptOUTPUT;
//    TRISA.TRISA7 = ptOUTPUT;
//    
//	PORTA.RA0 = ptALL_OFF;
//    PORTA.RA1 = ptALL_OFF;
//    PORTA.RA2 = ptALL_OFF;
//    PORTA.RA3 = ptALL_OFF;
//    PORTA.RA4 = ptALL_OFF;
//    PORTA.RA5 = ptALL_OFF;
//    PORTA.RA6 = ptALL_OFF;
//    PORTA.RA7 = ptALL_OFF;
    
}
/*-----------------------------------------------------------*/

void vLedSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
unsigned portBASE_TYPE uxLEDBit;

	/* Which port A bit is being modified? */
	uxLEDBit = 1 << uxLED;

	if( xValue )
	{
		/* Turn the LED on. */
		portENTER_CRITICAL();
		{
			PORTA |= uxLEDBit;
            LATA = PORTA;
		}
		portEXIT_CRITICAL();
	}
	else
	{
		/* Turn the LED off. */
		portENTER_CRITICAL();
		{
			PORTA &= ~uxLEDBit;
            LATA = PORTA;
		}
		portEXIT_CRITICAL();
	}
}
/*-----------------------------------------------------------*/

void vLedToggleLED( unsigned portBASE_TYPE uxLED )
{
unsigned portBASE_TYPE uxLEDBit;

	uxLEDBit = 1 << uxLED;
	portENTER_CRITICAL();
	{
		/* If the LED is already on - turn it off.  If the LED is already
		off, turn it on. */
		if( PORTA & uxLEDBit )
		{
			PORTA &= ~uxLEDBit;
		}
		else
		{
			PORTA |= uxLEDBit;
		}
	}
	portEXIT_CRITICAL();
}

