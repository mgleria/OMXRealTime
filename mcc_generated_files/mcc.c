/**
  @Generated PIC24 / dsPIC33 / PIC32MM MCUs Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    mcc.c

  @Summary:
    This is the mcc.c file generated using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
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

#define FCY         16000000ul      // Changing this automatically changes the PLL settings to run at this target frequency

#include <xc.h>
#include "../ezbl_integration/ezbl.h"

// Set flash page erase size = 0x800 program addresses = 3072 Flash bytes/1024 instruction words/512 Flash double words.
// PIC24FJ1024GB610/GA610 family devices implement 0x800; dsPIC33EP64GS506, dsPIC33EP128GS808, PIC24FJ256GB412/GA412 family devices implement 0x400.
// Single Partition bootloaders don't have to define this as it is automatically obtained by ezbl_tools.jar and inserted in the .gld linker script.
EZBL_SetSYM(EZBL_ADDRESSES_PER_SECTOR, 0x800);



//const unsigned int EZBL_i2cSlaveAddr = 0x60;          // Define I2C Slave Address that this Bootloader will listen/respond to, applicable only if I2C_Reset() is called
EZBL_FIFO *EZBL_COMBootIF __attribute__((persistent));  // Pointer to RX FIFO to check activity on for bootloading
const long EZBL_COMBaud = 115200;

//// Configuration bits: selected in the GUI
//
//// FSEC
//#pragma config BWRP = OFF    // Boot Segment Write-Protect bit->Boot Segment may be written
//#pragma config BSS = DISABLED    // Boot Segment Code-Protect Level bits->No Protection (other than BWRP)
//#pragma config BSEN = OFF    // Boot Segment Control bit->No Boot Segment
//#pragma config GWRP = OFF    // General Segment Write-Protect bit->General Segment may be written
//#pragma config GSS = DISABLED    // General Segment Code-Protect Level bits->No Protection (other than GWRP)
//#pragma config CWRP = OFF    // Configuration Segment Write-Protect bit->Configuration Segment may be written
//#pragma config CSS = DISABLED    // Configuration Segment Code-Protect Level bits->No Protection (other than CWRP)
//#pragma config AIVTDIS = OFF    // Alternate Interrupt Vector Table bit->Disabled AIVT
//
//// FOSCSEL
//#pragma config FNOSC = FRC    // Oscillator Source Selection->Internal Fast RC (FRC)
//#pragma config PLLMODE = PLL96DIV2    // PLL Mode Selection->96 MHz PLL. (8 MHz input)
//#pragma config IESO = OFF    // Two-speed Oscillator Start-up Enable bit->Start up with user-selected oscillator source
//
//// FOSC
//#pragma config POSCMD = NONE    // Primary Oscillator Mode Select bits->Primary Oscillator disabled
//#pragma config OSCIOFCN = OFF    // OSC2 Pin Function bit->OSC2 is clock output
//#pragma config SOSCSEL = ON    // SOSC Power Selection Configuration bits->SOSC is used in crystal (SOSCI/SOSCO) mode
//#pragma config PLLSS = PLL_PRI    // PLL Secondary Selection Configuration bit->PLL is fed by the Primary oscillator
//#pragma config IOL1WAY = OFF    // Peripheral pin select configuration bit->Allow multiple reconfigurations
//#pragma config FCKSM = CSECME    // Clock Switching Mode bits->Both Clock switching and Fail-safe Clock Monitor are enabled
//
//// FWDT
//#pragma config WDTPS = PS1024    // Watchdog Timer Postscaler bits->1:1024
//#pragma config FWPSA = PR32    // Watchdog Timer Prescaler bit->1:32
//#pragma config FWDTEN = ON    // Watchdog Timer Enable bits->WDT Enabled
//#pragma config WINDIS = ON    // Watchdog Timer Window Enable bit->Watchdog Timer in Window mode
//#pragma config WDTWIN = WIN25    // Watchdog Timer Window Select bits->WDT Window is 25% of WDT period
//#pragma config WDTCMX = LPRC    // WDT MUX Source Select bits->WDT always uses LPRC as its clock source
//#pragma config WDTCLK = LPRC    // WDT Clock Source Select bits->WDT uses LPRC
//
//// FPOR
//#pragma config BOREN = ON    // Brown Out Enable bit->Brown Out Enable Bit
//#pragma config LPCFG = ON    // Low power regulator control->Retention Sleep controlled by RETEN
//#pragma config DNVPEN = ENABLE    // Downside Voltage Protection Enable bit->Downside protection enabled using ZPBOR when BOR is inactive
//
//// FICD
//#pragma config ICS = PGD2    // ICD Communication Channel Select bits->Communicate on PGEC2 and PGED2
//#pragma config JTAGEN = OFF    // JTAG Enable bit->JTAG is disabled
//#pragma config BTSWP = ON    // BOOTSWP Disable->BOOTSWP instruction is enabled
//
//// FDEVOPT1
//#pragma config ALTCMPI = DISABLE    // Alternate Comparator Input Enable bit->C1INC, C2INC, and C3INC are on their standard pin locations
//#pragma config TMPRPIN = OFF    // Tamper Pin Enable bit->TMPRN pin function is disabled
//#pragma config SOSCHP = ON    // SOSC High Power Enable bit (valid only when SOSCSEL = 1->Enable SOSC high power mode (default)
//#pragma config ALTVREF = ALTREFEN    // Alternate Voltage Reference Location Enable bit->VREF+ and CVREF+ on RA10, VREF- and CVREF- on RA9
//
//// FBOOT
//#pragma config BTMODE = DUAL    // Boot Mode Configuration bits->Device is in Dual Boot mode

// Device Configuration Words
// Config words can be defined in a Bootloader project, Application project,
// or mixed between each other so long as any given flash/config write-word-
// size location has exactly one definition. On devices with flash Config
// words and a flash double word minimum programming size (0x4 program
// addresses), this means two adjacent Config words may not be mixed between
// projects and instead both Config words must be defined in the same project.
//
// These defaults place all (or almost all) in the Bootloader project as this
// is the safest from a bootloader-bricking standpoint.
#if defined(EZBL_BOOT_PROJECT)  // Compiling for a Bootloader Project
    EZBL_SET_CONF(_FSEC, BWRP_OFF & BSS_OFF & BSEN_OFF & GWRP_OFF & GSS_OFF & CWRP_OFF & CSS_DIS & AIVTDIS_DISABLE)
            
    EZBL_SET_CONF(_FOSCSEL, FNOSC_FRC & PLLMODE_PLL96DIV2 & IESO_OFF)
    EZBL_SET_CONF(_FOSC, POSCMD_XT & OSCIOFCN_ON & SOSCSEL_ON & PLLSS_PLL_PRI & IOL1WAY_OFF & FCKSM_CSECME)
    EZBL_SET_CONF(_FWDT, WDTPS_PS1024 & FWPSA_PR32 & SWON & WINDIS_OFF & WDTCMX_LPRC & WDTCLK_LPRC)
    EZBL_SET_CONF(_FPOR, BOREN_ON & LPCFG_ON & DNVPEN_ENABLE)
    EZBL_SET_CONF(_FICD, PGX2 & JTAGEN_OFF & BTSWP_ON)
    EZBL_SET_CONF(_FDEVOPT1, ALTCMPI_DISABLE & TMPRPIN_OFF & SOSCHP_ON & ALTVREF_ALTVREFDIS)
    #if defined(__DUAL_PARTITION)
    EZBL_SET_CONF(_FBOOT, BTMODE_DUAL)    // FBOOT = 0xFFFF = Signal Partition mode; 0xFFFE = Ordinary Dual Partition mode; 0xFFFD = Protected Dual Partition mode
    #endif
#else   // Compiling for an Application Project (EZBL_BOOT_PROJECT is not defined)
    //EZBL_SET_CONF_FBTSEQ(4094)          // FBTSEQ is set at run time - should not be defined explicitly unless you need to reset back to a max value. If doing so, define it in the App project, not the Bootloader.
#endif  // Goes to: #if defined(EZBL_BOOT_PROJECT)

#include "mcc.h"

void OSCILLATOR_Initialize(void)
{
    // Switch to FRC clock (no PLL), in case if the PLL is currently in use.
    // We should not be changing the PLL prescalar, postscalar or feedback
    // divider (if present) while the PLL is clocking anything.
    __builtin_write_OSCCONH(0x00);
    __builtin_write_OSCCONL(OSCCON | _OSCCON_OSWEN_MASK);
    while(OSCCONbits.OSWEN);            // Wait for clock switch to complete

    // Configure PLL for Fosc = 32MHz/Fcy = 16MIPS using 8 MHz internal FRC oscillator + SOSC self-tuning
    CLKDIV = 0xB120;                    // ROI = 1, DOZE = 8:1, RCDIV<2:0> = Fast RC Oscillator (FRC) with PLL module (FRCPLL), CPDIV<1:0> = 32MHz (96MHz PLL post divider div by 1 for 32MHz CPU clock); PLLEN = 1 (PLL stays active for USB when CPU not being clocked from PLL)
    OSCDIV = 0x0000;                    // 1:1 divide on FRC, XT or EC oscillator (2:1 is the default)
    OSCTUN = 0x8000;                    // FRC self-tuning enabled, STSRC = 0 (FRC is tuned to approximately match the 32.768 kHz SOSC tolerance)
    __builtin_write_OSCCONH(0x01);      // Initiate Clock Switch to use the FRC Oscillator + PLL (NOSC = 0b001)
    __builtin_write_OSCCONL(OSCCON | _OSCCON_OSWEN_MASK);

    // Wait for clock switch to complete and PLL to be locked (if enabled)
    while(OSCCONbits.OSWEN);            // Wait for clock switch to complete
    if((OSCCONbits.COSC & 0x5) == 0x1)  // 0x0 = Fast RC Oscillator (FRC); 0x1 = Fast RC Oscillator (FRC) with Divide-by-N and PLL; 0x2 = Primary Oscillator (XT, HS, EC); 0x3 = Primary Oscillator (XT, HS, EC) with PLL; 0x4 = Secondary Oscillator (SOSC); 0x5 = Low-Power RC Oscillator (LPRC); 0x6 = Fast RC Oscillator (FRC) with Divide-by-16; 0x7 = Fast RC Oscillator (FRC) with Divide-by-N
    {
        while(!_LOCK);
    }
    
//    // CF no clock failure; NOSC FRC; SOSCEN disabled; POSCEN disabled; CLKLOCK unlocked; OSWEN Request Switch; IOLOCK not-active; 
//    __builtin_write_OSCCONL((uint8_t) (0x0001 & 0x00FF));
//    // CPDIV 1:1; PLLEN disabled; DOZE 1:8; RCDIV PRI; DOZEN disabled; ROI disabled; 
//    CLKDIV = 0x3200;
//    // STOR disabled; STORPOL Interrupt when STOR is 1; STSIDL disabled; STLPOL Interrupt when STLOCK is 1; STLOCK disabled; STSRC SOSC; STEN disabled; TUN Center frequency; 
//    OSCTUN = 0x0000;
//    // ROEN disabled; ROSEL FOSC; ROSIDL disabled; ROSWEN disabled; ROOUT disabled; ROSLP disabled; 
//    REFOCONL = 0x0000;
//    // RODIV 0; 
//    REFOCONH = 0x0000;
//    // ROTRIM 0; 
//    REFOTRIML = 0x0000;
//    // DCOTUN 0; 
//    DCOTUN = 0x0000;
//    // DCOFSEL 8; DCOEN disabled; 
//    DCOCON = 0x0700;
//    // DIV 0; 
//    OSCDIV = 0x0000;
//    // TRIM 0; 
//    OSCFDIV = 0x0000;
}

void buttons_Initialize()
{
    // Set push buttons as GPIO inputs
    //
    // Function     Explorer 16 PIM Header      PIC24FJ1024GB610 Device Pins
    // Button       PIM#, PICtail#, name        PIC#, name
    // S4 (LSb)       80, 106, RD13               80, OCM3F/PMD13/RD13
    // S5             92,  74, RA7                92, AN22/OCM1F/PMA17/RA7              <- Pin function is muxed with LED D10; S5 button can't be used because LED clamps weak 10k pull up voltage too low
    // S6             84, 100, RD7                84, C3INA/U5RTS/U5BCLK/OC5/PMD15/RD7
    // S3 (MSb)       83,  99, RD6                83, C3INB/U5RX/OC4/PMD14/RD6
    EZBL_DefineButtonMap(RD6, RD7, RD13);
    _TRISD13 = 1;
    //_TRISA7  = 1;
    _TRISD7  = 1;
    _TRISD6  = 1;
    //_ANSA7   = 0;
    _ANSD7   = 0;
    _ANSD6   = 0;
}

void leds_Initialize()
{
    // Set LED pins as GPIO outputs
    //
    // Function     Explorer 16 PIM Header      PIC24FJ1024GB610 Device Pins
    // LED          PIM#, PICtail#, name        PIC#, name
    // D3 (LSb)       17, 69, RA0/TMS             17, TMS/OCM3D/RA0
    // D4             38, 70, RA1/TCK             38, TCK/RA1
    // D5             58, 38, RA2/SCL2            58, PMPCS1/SCL2/RA2
    // D6             59, 40, RA3/SDA2            59, SDA2/PMA20/RA3
    // D7             60, 71, RA4/TDI             60, TDI/PMA21/RA4
    // D8             61, 72, RA5/TDO             61, TDO/RA5
    // D9             91, 73, RA6                 91, AN23/OCM1E/RA6
    // D10 (MSb)      92, 74, RA7                 92, AN22/OCM1F/PMA17/RA7        <- Pin function is muxed with button S5; we will use it as an LED output only
    EZBL_DefineLEDMap(RA7, RA6, RA5, RA4, RA3, RA2, RA1, RA0);
    *((volatile unsigned char *)&LATA)  = 0x00; // Write bits LATA<7:0> simultaneously, don't change LATA<15:8>
    *((volatile unsigned char *)&TRISA) = 0x00;
    *((volatile unsigned char *)&ANSA)  = 0x00;
}

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    
    InitializeBoard(); //Esto no es parte de MCC
    
    I2C1_Initialize();
    TMR2_Initialize();
}

/**
 * Initializes system level hardware, such as the system clock source (PLL),
 * I/O pins for LED status indication, a timer for the NOW_*() timekeeping and
 * scheduling APIs and one or more communications peripherals for EZBL
 * bootloading.
 *
 * Although not required or used for bootloading, this function also initializes
 * I/O pins for push buttons, LCD interfacing and some extra items commonly on
 * Microchip development boards. When porting to other hardware, the extra
 * initialization code can be deleted.
 *
 * @return unsigned long system execution clock, in instructions/second (FCY).
 *
 *         One 16-bit timer peripheral will be enabled, along with it's ISR at a
 *         a period of 65536 system clocks. The timer/CCP used for this is
 *         selected within this code using the NOW_Reset() macro.
 *
 *         At least one communications is also selected and initialized, along
 *         with 1 or 2 ISRs for it (UART RX + TX ISRs or Slave I2C (no Master
 *         I2C).
 */
// @return: FCY clock speed we just configured the processor for
unsigned long InitializeBoard(void)
{
//    EZBL_KeepSYM(EZBL_TrapHandler);
    
    OSCILLATOR_Initialize();
    
    #if defined(XPRJ_uart) || defined(XPRJ_default) || defined(EZBL_INIT_UART)    // XPRJ_* definitions defined by MPLAB X on command line based on Build Configuration. If you need this interface always, you can alternatively define a project level EZBL_INIT_UART macro.    // Configure UART2 pins as UART.
    // - Pin names are with respect to the PIC.
    // - Outputs bits in TRIS registers are all set as inputs because the PPS or
    //   UART2 hardware overrides it.
    //
    // Function     Explorer 16 PIM Header          PIC24FJ1024GB610 Device Pins
    // UART2        PIM#, PICtail#, name            PIC#, name
    // U2RX  (in)     40, 51, RPI32/CTED7/PMA18/RF12      40, RPI32/CTED7/PMA18/RF12
    // U2TX  (out)    39, 52, RP31/RF13                   39, RP31/RF13
//    IOCPUFbits.IOCPF12 = 1;      // Turn on weak pull up on U2RX so no spurious data arrives if nobody connected
////    IOCPUF = 0x100C;            // Turn on weak pull up on U2RX (MCC)    
//    _U2RXR  = 0x0020;           // RF12->UART2:U2RX;
//    _RP31R  = _RPOUT_U2TX;      // RF13->UART2:U2TX;
//    if(EZBL_COMBaud <= 0)       // If auto-baud enabled, delay our UART initialization so MCP2221A POR timer and init
//    {                           // is complete before we start listening. POR timer max spec is 140ms, so MCP2221A TX
//        NOW_Wait(140u*NOW_ms);  // pin glitching could occur long after we have enabled our UART without this forced delay.
//    }
//    EZBL_COMBootIF = UART_Reset(2, FCY, EZBL_COMBaud, 1);
    IOCPUFbits.IOCPF4 = 1;      // Turn on weak pull up on U2RX so no spurious data arrives if nobody connected
    _U2RXR  = 10;               // U2RX on RP10
    _RP17R  = _RPOUT_U2TX;      // U2TX on RP17
    if(EZBL_COMBaud <= 0)       // If auto-baud enabled, delay our UART initialization so MCP2221A POR timer and init
    {                           // is complete before we start listening. POR timer max spec is 140ms, so MCP2221A TX
        NOW_Wait(140u*NOW_ms);  // pin glitching could occur long after we have enabled our UART without this forced delay.
    }
    EZBL_COMBootIF = UART_Reset(2, FCY, EZBL_COMBaud, 1);
    #endif


    // Initialize/select 16-bit hardware timer for NOW time keeping/scheduling
    // APIs. This call selects the hardware timer resource (can be TMR1-TMR6 or
    // CCP1-CCP8, if available) and the _Tx/_CCTxInterrupt gets automatically
    // implemented by code in ezbl_lib.a.
    NOW_Reset(TMR4, FCY);
    
    buttons_Initialize();
    
    leds_Initialize();
    
    // Report 16 MIPS on PIC24F
    return FCY;
}



/**
 End of File
*/