#include "perifericos/adc.h"

#define PIN_ANALOG      1
#define PIN_DIGITAL     0

#define PIN_INPUT       1
#define PIN_OUTPUT      0
/*********************************************************************
* Function: ADC_ReadPercentage(ADC_CHANNEL channel);
*
* Overview: Reads the requested ADC channel and returns the percentage 
*            of that conversions result (0-100%).
*
* PreCondition: channel is configured via the ADCConfigure() function
*
* Input: ADC_CHANNEL channel - enumeration of the ADC channels
*        available in this demo.  They should be meaningful names and 
*        not the names of the ADC pins on the device (as the demo code 
*        may be ported to other boards).
*         i.e. ADC_ReadPercentage(ADC_CHANNEL_POTENTIOMETER);
*
* Output: uint8_t indicating the percentage of the result 0-100% or
*         0xFF for an error
*
********************************************************************/
uint8_t ADC_ReadPercentage( ADC_CHANNEL channel )
{
    uint8_t percent;

    switch(channel)
    {
        case ADC_CHANNEL_5:
            break;
      
        case ADC_CHANNEL_4:
            break ;

        default:
            return 0xFF;
    }
    
    //A very crude percentage calculation
    percent = (ADC_Read10bit(channel) / 10);

    if(percent > 100)
    {
        percent = 100;
    }
    return percent;
}

/*********************************************************************
* Function: ADC_Read10bit(ADC_CHANNEL channel);
*
* Overview: Reads the requested ADC channel and returns the 10-bit
*           representation of this data.
*
* PreCondition: channel is configured via the ADCConfigure() function
*
* Input: ADC_CHANNEL channel - enumeration of the ADC channels
*        available in this demo.  They should be meaningful names and
*        not the names of the ADC pins on the device (as the demo code
*        may be ported to other boards).
*         i.e. - ADCReadPercentage(ADC_CHANNEL_POTENTIOMETER);
*
* Output: uint16_t the right adjusted 10-bit representation of the ADC
*         channel conversion or 0xFFFF for an error.
*
********************************************************************/
uint16_t ADC_Read10bit(ADC_CHANNEL channel)
{
    uint16_t i;
    
    switch(channel)
    {
        case ADC_CHANNEL_5:
            break;

        case ADC_CHANNEL_4:
            break ;

        default:
            return 0xFFFF;
    }

    AD1CHS = channel ;

    // Get an ADC sample
    _SAMP = 1;           //Start sampling
    for(i=0;i<1000;i++)
    {
        Nop(); //Sample delay, conversion start automatically
    } 

    _SAMP = 0;           //Start sampling
    for(i=0;i<1000;i++)
    {
        Nop(); //Sample delay, conversion start automatically
    }

    while(!_DONE);       //Wait for conversion to complete

    return ADC1BUF0;
}

/*********************************************************************
* Function: bool ADC_ChannelEnable(ADC_CHANNEL channel, ADC_CONFIGURATION configuration);
*
* Overview: Configures the ADC module to specified setting
*
* PreCondition: none
*
* Input: ADC_CHANNEL channel - the channel to enable
*        ADC_CONFIGURATION configuration - the mode in which to run the ADC
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool ADC_ChannelEnable(ADC_CHANNEL channel)
{
    switch(channel)
    {
        case ADC_CHANNEL_5:
            _ANSB5 = PIN_ANALOG ;

        case ADC_CHANNEL_4:
            _ANSB4 = PIN_ANALOG ;
            return true ;

        default:
            return false;
    }
    
    return false;
}

/*********************************************************************
* Function: bool ADC_SetConfiguration(ADC_CONFIGURATION configuration)
*
* Overview: Configures the ADC module to specified setting
*
* PreCondition: none
*
* Input: ADC_CONFIGURATION configuration - the mode in which to run the ADC
*
* Output: bool - true if successfully configured.  false otherwise.
*
********************************************************************/
bool ADC_SetConfiguration(ADC_CONFIGURATION configuration)
{
    if(configuration == ADC_CONFIGURATION_DEFAULT)
    {
        _PVCFG = 0x0 ;
        _ADCS = 0xFF ;
        _SSRC = 0x0;
        _SAMC = 0b10000;
        _FORM = 0b00;
        _SMPI = 0x0;
        _ADON = 1;
        return true;
    }
		
    return false;
}
