
/**
  MCCP1 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    mccp1.c

  @Summary
    This is the generated driver implementation file for the MCCP1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for MCCP1. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - pic24-dspic-pic32mm : v1.35
        Device            :  PIC24FJ1024GB610
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.31
        MPLAB             :  MPLAB X 3.60
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include <xc.h> 
#include "mccp1_capture.h"


void MCCP1_CAPTURE_Initialize(void)
{
    // CCPON enabled; MOD Every rising edge; CCSEL enabled; CCPSIDL disabled; TMR32 16 Bit; CCPSLP disabled; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; 
    CCP1CON1L = (0x8011 & 0x7FFF); //Disabling CCPON bit
    //RTRGEN disabled; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each IC Event; SYNC None; OPSRC Timer Interrupt Event; 
    CCP1CON1H = 0x0000;
    //ASDGM disabled; SSDG disabled; ASDG None; PWMRSEN disabled; 
    CCP1CON2L = 0x0000;
    //OCCEN disabled; OCDEN disabled; OCEEN disabled; ICGSM Level-Sensitive mode; OCFEN disabled; ICSEL ICM1; AUXOUT Disabled; OCAEN disabled; OCBEN disabled; OENSYNC disabled; 
    CCP1CON2H = 0x0000;
    //DT 0; 
    CCP1CON3L = 0x0000;
    //OETRIG disabled; OSCNT None; POLACE disabled; POLBDF disabled; PSSBDF Tri-state; OUTM Steerable single output; PSSACE Tri-state; 
    CCP1CON3H = 0x0000;
    //ICDIS disabled; SCEVT disabled; TRSET disabled; ICOV disabled; ASEVT disabled; TRIG disabled; TRCLR disabled; 
    CCP1STATL = 0x0000;
    //TMR 0; 
    CCP1TMRL = 0x0000;
    //TMR 0; 
    CCP1TMRH = 0x0000;
    //PR 0; 
    CCP1PRL = 0x0000;
    //PR 0; 
    CCP1PRH = 0x0000;
    //CMP 0; 
    CCP1RAL = 0x0000;
    //CMP 0; 
    CCP1RBL = 0x0000;
    //BUF 0; 
    CCP1BUFL = 0x0000;
    //BUF 0; 
    CCP1BUFH = 0x0000;
    
    CCP1CON1Lbits.CCPON = 0x1; //Enabling CCP

    // Clearing IF flag before enabling the interrupt.
    IFS3bits.CCP1IF = 0;
    // Enabling MCCP1 interrupt.
    IEC3bits.CCP1IE = 1;

    // Clearing IF flag before enabling the interrupt.
    IFS6bits.CCT1IF = 0;
    // Enabling MCCP1 interrupt.
    IEC6bits.CCT1IE = 1;
}

void MCCP1_CAPTURE_Start( void )
{
    /* Start the Timer */
    CCP1CON1Lbits.CCPON = true;
}
void MCCP1_CAPTURE_Stop( void )
{
    /* Stop the Timer */
    CCP1CON1Lbits.CCPON = false;
}
void __attribute__ ( ( interrupt, no_auto_psv ) ) _CCP1Interrupt ( void )
{
    /* Check if the Timer Interrupt/Status is set */
    if(IFS3bits.CCP1IF)
    {
        IFS3bits.CCP1IF = 0;
    }
    vLedToggleLED(0);
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _CCT1Interrupt( void )
{
    if(IFS6bits.CCT1IF)
    {
        IFS6bits.CCT1IF = 0;
    }
    vLedToggleLED(1);
}

uint16_t MCCP1_CAPTURE_Data16Read( void )
{
    /* Return the captured timer data */
    return(CCP1BUFL);

}


bool MCCP1_CAPTURE_HasBufferOverflowed( void )
{
    return( CCP1STATLbits.ICOV );
}
bool MCCP1_CAPTURE_IsBufferEmpty( void )
{
    return( ! CCP1STATLbits.ICBNE );
}
void MCCP1_CAPTURE_OverflowFlagReset( void )
{
    CCP1STATLbits.ICOV = 0;
}
/**
 End of File
*/