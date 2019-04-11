/**
  UART5 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart5.c

  @Summary
    This is the generated source file for the UART5 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for UART5. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - pic24-dspic-pic32mm : v1.35
        Device            :  PIC24FJ1024GB610
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.31
        MPLAB 	          :  MPLAB X 3.60
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

#include "uart5.h"

/**
  Section: Data Type Definitions
*/

/** UART Driver Queue Status

  @Summary
    Defines the object required for the status of the queue.
*/

typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
}

UART_BYTEQ_STATUS;

/** UART Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

*/
typedef struct
{
    /* RX Byte Q */
    uint8_t                                      *rxTail ;

    uint8_t                                      *rxHead ;

    /* TX Byte Q */
    uint8_t                                      *txTail ;

    uint8_t                                      *txHead ;

    UART_BYTEQ_STATUS                        rxStatus ;

    UART_BYTEQ_STATUS                        txStatus ;

} UART_OBJECT ;

static UART_OBJECT uart5_obj ;

/** UART Driver Queue Length

  @Summary
    Defines the length of the Transmit and Receive Buffers

*/

#define UART5_CONFIG_TX_BYTEQ_LENGTH 256
#define UART5_CONFIG_RX_BYTEQ_LENGTH 8


/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart5_txByteQ[UART5_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart5_rxByteQ[UART5_CONFIG_RX_BYTEQ_LENGTH] ;


/**
  Section: Driver Interface
*/


void UART5_Initialize (void)
{
   // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
   U5MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
   // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
   U5STA = 0x0000;
   // BaudRate = 115200; Frequency = 16000000 Hz; U5BRG 34; 
   U5BRG = 0x0022;
   // ADMADDR 0; ADMMASK 0; 
   U5ADMD = 0x0000;

   IEC6bits.U5RXIE = 1;

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
   U5MODEbits.UARTEN = 1;  // enabling UART ON bit
   U5STAbits.UTXEN = 1;
   
   

   uart5_obj.txHead = uart5_txByteQ;
   uart5_obj.txTail = uart5_txByteQ;
   uart5_obj.rxHead = uart5_rxByteQ;
   uart5_obj.rxTail = uart5_rxByteQ;
   uart5_obj.rxStatus.s.empty = true;
   uart5_obj.txStatus.s.empty = true;
   uart5_obj.txStatus.s.full = false;
   uart5_obj.rxStatus.s.full = false;
}




/**
    Maintains the driver's transmitter state machine and implements its ISR
*/
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U5TXInterrupt ( void )
{ 
    if(uart5_obj.txStatus.s.empty)
    {
        IEC7bits.U5TXIE = false;
        return;
    }

    IFS7bits.U5TXIF = false;

    while(!(U5STAbits.UTXBF == 1))
    {

        U5TXREG = *uart5_obj.txHead;

        uart5_obj.txHead++;

        if(uart5_obj.txHead == (uart5_txByteQ + UART5_CONFIG_TX_BYTEQ_LENGTH))
        {
            uart5_obj.txHead = uart5_txByteQ;
        }

        uart5_obj.txStatus.s.full = false;

        if(uart5_obj.txHead == uart5_obj.txTail)
        {
            uart5_obj.txStatus.s.empty = true;
            break;
        }
    }
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U5RXInterrupt( void )
{


    while((U5STAbits.URXDA == 1))
    {

        *uart5_obj.rxTail = U5RXREG;

        uart5_obj.rxTail++;

        if(uart5_obj.rxTail == (uart5_rxByteQ + UART5_CONFIG_RX_BYTEQ_LENGTH))
        {
            uart5_obj.rxTail = uart5_rxByteQ;
        }

        uart5_obj.rxStatus.s.empty = false;
        
        if(uart5_obj.rxTail == uart5_obj.rxHead)
        {
            //Sets the flag RX full
            uart5_obj.rxStatus.s.full = true;
            break;
        }
        
    }

    IFS6bits.U5RXIF = false;
   
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U5ErrInterrupt ( void )
{
    if ((U5STAbits.OERR == 1))
    {
        U5STAbits.OERR = 0;
    }

    IFS7bits.U5ERIF = false;
}

/**
  Section: UART Driver Client Routines
*/

uint8_t UART5_Read( void)
{
    uint8_t data = 0;

    data = *uart5_obj.rxHead;

    uart5_obj.rxHead++;

    if (uart5_obj.rxHead == (uart5_rxByteQ + UART5_CONFIG_RX_BYTEQ_LENGTH))
    {
        uart5_obj.rxHead = uart5_rxByteQ;
    }

    if (uart5_obj.rxHead == uart5_obj.rxTail)
    {
        uart5_obj.rxStatus.s.empty = true;
    }

    uart5_obj.rxStatus.s.full = false;

    return data;
}


unsigned int UART5_ReadBuffer( uint8_t *buffer, const unsigned int bufLen)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( bufLen ))
    {
        if( uart5_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = UART5_Read () ;
        }
    }

    return numBytesRead ;
}



void UART5_Write( const uint8_t byte)
{
    IEC7bits.U5TXIE = false;
    
    *uart5_obj.txTail = byte;

    uart5_obj.txTail++;
    
    if (uart5_obj.txTail == (uart5_txByteQ + UART5_CONFIG_TX_BYTEQ_LENGTH))
    {
        uart5_obj.txTail = uart5_txByteQ;
    }

    uart5_obj.txStatus.s.empty = false;

    if (uart5_obj.txHead == uart5_obj.txTail)
    {
        uart5_obj.txStatus.s.full = true;
    }

    IEC7bits.U5TXIE = true ;
	
}


unsigned int UART5_WriteBuffer( const uint8_t *buffer , const unsigned int bufLen )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( bufLen ))
    {
        if((uart5_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            UART5_Write (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

}


UART5_TRANSFER_STATUS UART5_TransferStatusGet (void )
{
    UART5_TRANSFER_STATUS status = 0;

    if(uart5_obj.txStatus.s.full)
    {
        status |= UART5_TRANSFER_STATUS_TX_FULL;
    }

    if(uart5_obj.txStatus.s.empty)
    {
        status |= UART5_TRANSFER_STATUS_TX_EMPTY;
    }

    if(uart5_obj.rxStatus.s.full)
    {
        status |= UART5_TRANSFER_STATUS_RX_FULL;
    }

    if(uart5_obj.rxStatus.s.empty)
    {
        status |= UART5_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= UART5_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}


uint8_t UART5_Peek(uint16_t offset)
{
    if( (uart5_obj.rxHead + offset) > (uart5_rxByteQ + UART5_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart5_rxByteQ[offset - (uart5_rxByteQ + UART5_CONFIG_RX_BYTEQ_LENGTH - uart5_obj.rxHead)];
    }
    else
    {
      return *(uart5_obj.rxHead + offset);
    }
}


unsigned int UART5_ReceiveBufferSizeGet(void)
{
    if(!uart5_obj.rxStatus.s.full)
    {
        if(uart5_obj.rxHead > uart5_obj.rxTail)
        {
            return(uart5_obj.rxHead - uart5_obj.rxTail);
        }
        else
        {
            return(UART5_CONFIG_RX_BYTEQ_LENGTH - (uart5_obj.rxTail - uart5_obj.rxHead));
        } 
    }
    return 0;
}


unsigned int UART5_TransmitBufferSizeGet(void)
{
    if(!uart5_obj.txStatus.s.full)
    { 
        if(uart5_obj.txHead > uart5_obj.txTail)
        {
            return(uart5_obj.txHead - uart5_obj.txTail);
        }
        else
        {
            return(UART5_CONFIG_TX_BYTEQ_LENGTH - (uart5_obj.txTail - uart5_obj.txHead));
        }
    }
    return 0;
}


bool UART5_ReceiveBufferIsEmpty (void)
{
    return(uart5_obj.rxStatus.s.empty);
}


bool UART5_TransmitBufferIsFull(void)
{
    return(uart5_obj.txStatus.s.full);
}


UART5_STATUS UART5_StatusGet (void)
{
    return U5STA;
}



/**
  End of File
*/
