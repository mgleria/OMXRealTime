/* Scheduler includes. */
#include "FreeRTOS.h"

/* Demo app includes. */
#include "perifericos/led.h"

#define ptOUTPUT 	0
#define ptALL_OFF	0

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

void vLedInitialise( void )
{
	/* The explorer 16 board has LED's on port A.  All bits are set as output
	so PORTA is read-modified-written directly. */
	TRISA = ptOUTPUT;
	PORTA = ptALL_OFF;
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
		}
		portEXIT_CRITICAL();
	}
	else
	{
		/* Turn the LED off. */
		portENTER_CRITICAL();
		{
			PORTA &= ~uxLEDBit;
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

