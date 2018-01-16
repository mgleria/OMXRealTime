/* 
 * File:   HardwareProfile.h
 * Author: Tincho
 *
 * Created on 16 de enero de 2018, 18:04
 */
/* 
 * File:   HardwareProfile.h
 * Author: Juan Cruz Becerra
 *
 * Created on 8 de enero de 2013, 11:28
 */

#ifndef HARDWAREPROFILE_H
#define	HARDWAREPROFILE_H


/**********************************************************************************************/
/**	
 * Definir el modelo de equipo a complilar en la linea.	\n
 * preprocesador del compilador.						\n
 * Lista de modelos implementados:						\n
 * -	__OMX_S__										\n
 * -	__OMXS_C_ (OMX-Li)								\n
 * -    __OMX_N__ (Sensor de nivel)                     \n
 * -    __OMX_T__ (Estación con sensores de Tecmes)     \n
 *  	*/


/**********************************************************************************************/
#if	!defined	(__OMX_S__)	&&	!defined	(__OMX_S_C__) && !defined (__OMX_N__) &&	!defined	(__OMX_T__)
	#error		Dispositivo no definido
	#endif


/**********************************************************************************************/
/*   Fuses define   */ //Fuses definidos en PIC18F26K80
//#pragma	config	XINST = OFF, RETEN = OFF, INTOSCSEL = LOW, SOSCSEL = DIG
//#pragma	config	IESO = ON, FOSC = INTIO2, FCMEN = OFF, PLLCFG = ON
//#pragma	config	BOREN = ON, BORPWR = LOW, BORV = 1, PWRTEN = ON
//#pragma	config	WDTPS = 1024, WDTEN = 2
//#pragma	config	MCLRE = ON
//#pragma	config	STVREN = ON, BBSIZ = BB1K
//#pragma	config	CP0 = ON, CP1 = ON, CP2 = OFF, CP3 = OFF
//#pragma	config	CPD = OFF, CPB = ON
//#pragma	config	WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
//#pragma	config	WRTB = OFF, WRTC = OFF, WRTD = OFF
//#pragma	config	EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
//#pragma	config	EBTRB = ON

/*	Definicion de los fusible:
 * 
 * No PLL
 * WDT habilitado por la aplicacion
 * Watch Dog Timer uses 1:1024 Postscale = 4.096 segundos
 * High speed Osc
 * Code protected from reads
 * Reset when brownout detected, at 2.0V
 * Power-up Timer Enable
 * Data EEPROM Code Protected
 * Stack full/underflow will cause reset
 * No Debug mode for ICD, salvo se compile para depurar
 * Program memory not write protected
 * Data EEPROM not write protected
 * Internal External Switch Over mode enabled
 * Fail-safe clock monitor enabled
 * Configuration not registers write protected
 * Boot block not write protected
 * No Boot Block code protection
 * Timer1 configured for higher power operation
 * Master Clear pin enabled
 * Extended set extension and Indexed Addressing mode disabled
 */

//// CONFIG1L
//#pragma config RETEN = OFF      // VREG Sleep Enable bit (Ultra low-power regulator is Disabled (Controlled by REGSLP bit))
//#pragma config INTOSCSEL = LOW  // LF-INTOSC Low-power Enable bit (LF-INTOSC in Low-power mode during Sleep)
//#pragma config SOSCSEL = DIG    // SOSC Power Selection and mode Configuration bits (Digital (SCLKI) mode)
//#pragma config XINST = OFF      // Extended Instruction Set (Disabled)
//
//// CONFIG1H
//#pragma config FOSC = INTIO2    // Oscillator (Internal RC oscillator)
//#pragma config PLLCFG = ON      // PLL x4 Enable bit (Enabled)
//#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
//#pragma config IESO = ON        // Internal External Oscillator Switch Over Mode (Enabled)
//
//// CONFIG2L
//#pragma config PWRTEN = ON      // Power Up Timer (Enabled)
//#pragma config BOREN = ON       // Brown Out Detect (Controlled with SBOREN bit)
//#pragma config BORV = 1         // Brown-out Reset Voltage bits (2.7V)
//#pragma config BORPWR = LOW     // BORMV Power level (BORMV set to low power level)
//
//// CONFIG2H
//#pragma config WDTEN = ON       // Watchdog Timer (WDT controlled by SWDTEN bit setting)
//#pragma config WDTPS = 1024     // Watchdog Postscaler (1:1024)
//
//// CONFIG3H
//#pragma config CANMX = PORTC    // ECAN Mux bit (ECAN TX and RX pins are located on RC6 and RC7, respectively)
//#pragma config MSSPMSK = MSK5   // MSSP address masking (5 bit address masking mode)
//#pragma config MCLRE = ON       // Master Clear Enable (MCLR Enabled, RE3 Disabled)
//
//// CONFIG4L
//#pragma config STVREN = ON      // Stack Overflow Reset (Enabled)
//#pragma config BBSIZ = BB1K     // Boot Block Size (1K word Boot Block size)
//
//// CONFIG5L
//#pragma config CP0 = OFF        // Code Protect 00800-03FFF (Disabled)
//#pragma config CP1 = OFF        // Code Protect 04000-07FFF (Disabled)
//#pragma config CP2 = OFF        // Code Protect 08000-0BFFF (Disabled)
//#pragma config CP3 = OFF        // Code Protect 0C000-0FFFF (Disabled)
//
//// CONFIG5H
//#pragma config CPB = OFF        // Code Protect Boot (Disabled)
//#pragma config CPD = OFF        // Data EE Read Protect (Disabled)
//
//// CONFIG6L
//#pragma config WRT0 = OFF       // Table Write Protect 00800-03FFF (Disabled)
//#pragma config WRT1 = OFF       // Table Write Protect 04000-07FFF (Disabled)
//#pragma config WRT2 = OFF       // Table Write Protect 08000-0BFFF (Disabled)
//#pragma config WRT3 = OFF       // Table Write Protect 0C000-0FFFF (Disabled)
//
//// CONFIG6H
//#pragma config WRTC = OFF       // Config. Write Protect (Disabled)
//#pragma config WRTB = OFF       // Table Write Protect Boot (Disabled)
//#pragma config WRTD = OFF       // Data EE Write Protect (Disabled)
//
//// CONFIG7L
//#pragma config EBTR0 = OFF      // Table Read Protect 00800-03FFF (Disabled)
//#pragma config EBTR1 = OFF      // Table Read Protect 04000-07FFF (Disabled)
//#pragma config EBTR2 = OFF      // Table Read Protect 08000-0BFFF (Disabled)
//#pragma config EBTR3 = OFF      // Table Read Protect 0C000-0FFFF (Disabled)
//
//// CONFIG7H
//#pragma config EBTRB = OFF      // Table Read Protect Boot (Disabled)
//
//// #pragma config statements should precede project file includes.
//// Use project enums instead of #define for ON and OFF.
//
///**********************************************************************************************/
///*	Ports micro define	*/
//
///*	port a	*/
//#define     AD_BATERIA			PORTAbits.RA0
//#if defined (__OMX_S__) || defined (__OMX_S_C__) || defined (__OMX_T__)
//    #define     VELETA				PORTAbits.RA1
//#endif
//#if defined  (__OMX_N__)
//    #define    trisHAB_NIVEL   TRISAbits.TRISA1
//    #define    HAB_NIVEL       PORTAbits.RA1
//#endif
//#define		RADIACION			PORTAbits.RA2
//#define     SOILTEMP			PORTAbits.RA3
//#define     OUT_PLUVIOMETRO		PORTAbits.RA5
//#define		trisCOOLER			TRISAbits.TRISA7
//#define     portCOOLER			PORTAbits.RA7
//
///*	port b	*/
//#define     DATA_SHT			LATBbits.LATB0
//#define		DATA_SHT_PORT		PORTBbits.RB0
//#define     SCK_SHT				LATBbits.LATB1
#define     FTDI_EN				PORTBbits.RB3
//#if defined (__OMX_S__) || defined (__OMX_S_C__) || defined (__OMX_T__)
//    #define     ANEMOMETRO			PORTBbits.RB5
//#endif
//#if defined  (__OMX_N__)
//    #define    NIVEL           PORTBbits.RB5
//    #define    trisLevel       TRISBbits.TRISB5
//#endif
//
//
///*	port c	*/
//
//#define     trisPOWER_MDM       TRISCbits.TRISC0
//#define     portPOWER_MDM       PORTCbits.RC0
//#define     trisRST_MDM         TRISCbits.TRISC1
//#define     portRST_MDM         PORTCbits.RC1
//#define     trisPWR_PER         TRISCbits.TRISC2
//#define     portPWR_PER         PORTCbits.RC2
//#define     SCL					PORTCbits.RC3
//#define     SDA					PORTCbits.RC4
//#define     TX_HARD				PORTCbits.RC6
//#define     RX_HARD				PORTCbits.RC7
//
///*	port d	*/
//#define		trisLED_GREEN		TRISDbits.TRISD0
//#define     portLED_GREEN		LATDbits.LATD0 
//#define		trisLED_RED			TRISDbits.TRISD1
//#define     portLED_RED			LATDbits.LATD1
//#define		trisLED_GREEN_2		TRISDbits.TRISD2
//#define     portLED_GREEN_2		LATDbits.LATD2
//
///*	port e	*/
//#define     FREATIMETRO         PORTEbits.RE0
//#define     AD_PANEL            PORTEbits.RE2
//
///*	Macros defines	*/
//#define     POWER_GPRS_HARD_ON	{	trisPOWER_MDM = 1;	portPOWER_MDM = 0;	}
//#define     POWER_GPRS_HARD_OFF	{	trisPOWER_MDM = 0;	portPOWER_MDM = 0;	}
//
//#define	LED_OFF(tris,port)	{	tris = 1;	port = 0;	}
//#define	LED_ON(tris,port)	{	tris = 0;	port = 1;	}
/**********************************************************************************************/
/*	SET SENSOR INCLUDED HERE	*/


    #ifndef	USE_SHT1x
    #define	USE_HIH6131
    #endif
    
	#define	USE_SENSOR_PRESURE
	#define	USE_SENSOR_RAIN
	#define	USE_SENSOR_SOLAR_RAD
	#define	USE_SENSOR_SOIL_TEMP
    #define USE_SENSOR_NAPA_FREAT

#if defined  (__OMX_N__)
	#define USE_SENSOR_LEVEL
#else
    #define	USE_SENSOR_WIND
#endif

/*	Only one type of sensor is allowed	*/
#if	defined	USE_SHT1x	&&	defined	USE_HIH6131
	#error	The use of both sensors is not allowed
	#endif

#if	defined	USE_SHT1x	||	defined	USE_HIH6131
	#define	USE_SENSOR_TEMP_HUM
	#endif

/**********************************************************************************************/
/*	Tasks included depending on model	*/

#define	USE_GPRS
#define	USE_SMS
#define	USE_SAMPLE
#define	USE_RESET
#define	USE_CMD_LINE

/**********************************************************************************************/
/*	Automatic enum of implemented processes	*/
enum
{
	DEFAULT_ID = 0,
#if	defined	USE_SAMPLE
	SAMPLE_ID,
	#endif
#if	defined	USE_GPRS
	GPRS_ID,
	#endif
#if	defined	USE_SMS
	SMS_ID,
	#endif
#if	defined	USE_CMD_LINE
	CMD_ID,
	#endif
#if	defined	USE_RESET
	RESET_ID,
	#endif
	NO_ID
};

#include "typedef.h"

#endif	/* HARDWAREPROFILE_H */



