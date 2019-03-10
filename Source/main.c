/*
 * File:   main.c
 * Author: Tincho
 *
 * Created on 20 de septiembre de 2017, 18:56
 */


#include "xc.h"
#include <libpic30.h>

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "freeRtos/FreeRTOS.h"
#include "freeRtos/task.h"
#include "freeRtos/queue.h"

///*FreeRTOS Include*/
//#include "semphr.h" 

/*EZBL Include*/
#include "ezbl.h"


/* Demo application includes. */
#include "perifericos/lcd.h"
#include "perifericos/adc.h"
#include "perifericos/led.h"
#include "tmr2.h"
//#include "uart1.h"
#include "i2c1.h"

#include "mcc.h"
#include "typedef.h"

#include "funciones/shell.h"
#include "funciones/eeprom.h"
#include "funciones/rtcc.h"
#include "funciones/memory.h"
#include "drivers/at_cmds.h"

#include "timers.h"

/**********************************************************************************************/
/*	texto del modelo de equipo	*/
#if		defined	(__OMX_S__)
string model[] = "OMX-S";
#elif	defined	(__OMX_S_C__)
string model[] = "OMX-Li";
#elif	defined	(__OMX_T__)
string model[] = "OMX-Tec";
#elif defined (__OMX_N__)
string model[] = "OMX-N";
#else
#error	No se define el modelo de equipo
#endif

/**	Definicion de la version de firmware */
#define	FW_DEVICE_VERSION	(2.00)	///<	version y revision del firmware (entero.fraccional)

/**	Macro para la version y revision de firmware	*/
#define	deviceVersion(ver)	(ver*100)

string version[] = "2.00";

#define mainMAX_STRING_LENGTH				( 20 )
#define bufLen                              ( 15 )
#define DEFAULT_STACK_SIZE                  (1000)  

//***********************Prototipo de tareas************************************

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

void vTaskShell( void *pvParameters );

void vTaskModem( void *pvParameters ); 

#include "tareas/testTask.h"
#include "tareas/sampleTask.h"
#include "tareas/gprsTask.h"

//***********************Prototipo de funciones externas************************
//The vTaskDelay() API function prototype
void vTaskDelay( TickType_t xTicksToDelay ); 

//***********************Prototipo de funciones propias*************************
void setEstacionConfig();
void setDeviceConfig();
void printConfigRegisters();
void printConfigRealRegistersPartition1();
void printConfigRealRegistersPartition2();

//******************************Globales****************************************

//The queue used to send messages to the LCD task.
static QueueHandle_t xLCDQueue;

QueueHandle_t   xModemRequests;
QueueHandle_t   xModemResponses;

//Handlers tareas
TaskHandle_t xShellHandle;
TaskHandle_t xModemHandle;
TaskHandle_t xTaskSwapPartition;

//uint8 sendCmd=TRUE;
static const char *pcSensor = "Pot";
static char cStringBuffer[ mainMAX_STRING_LENGTH ];

/*	Estructura con informacin del equipo	*/
estacion_t estacion;
configDevice_t configDevice;

/*	fecha y hora del sistema. Definida en ext_rtcc.c	*/
extern rtcc_t tiempo;

/*Macro para resetear los punteros de memoria en cada inicio de la app*/
#define     RESET_MEMORY     1

extern volatile int timeForPartitionSwap __attribute__((persistent));    // Global variable signaling when we complete a successful firmware update on the Inactive Partition

int main( void )
{
//    #if defined (__DEBUG)
//    __builtin_software_breakpoint();
//    #endif
    
    SYSTEM_Initialize();
    
//            ClrWdt();

    rtc_init();
////    vLedInitialise();
    setEstacionConfig();
    
    if(RESET_MEMORY) resetSamplesPtr();
    
    printMemoryPointers();
    
//    EZBL_BootloaderInit();
//    startSampleTask();
    startGprsTask();
//    startCLITask();    
//    startSwapPartitionTask();
    
//    startTestTask();
    //    vTaskTestClone();
    //    vTraceEnable(TRC_START);
     
//    /* Start the task that will control the LCD.  This returns the handle
//	to the queue used to write text out to the task. */
//	xLCDQueue = xStartLCDTask();
    
	/* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}

void vTaskSwapPartition( void *pvParameters )
{
    unsigned long ledBlinkTimer;
    int i;

    
    if(!_SFTSWP)
    {
        EZBL_ConsoleClear();    // Writes "Shift In", "Clear Screen" and "Reset Attributes" ANSI control codes to EZBL_STDOUT
    }
    
    ledBlinkTimer = NOW_32();
    
    printConfigRegisters();
    
    printConfigRealRegistersPartition1();
    printConfigRealRegistersPartition2();
    
    for(;;)
    {
        ClrWdt();
        // Every half second toggle an LED (1 Hz blink rate) to indicate we are alive
        if(NOW_32() - ledBlinkTimer > NOW_sec/2u)
        {
            LEDToggle(0x07);
            ledBlinkTimer += NOW_sec/2u;
//            EZBL_printf("\n\nTesting");
        }


        if(timeForPartitionSwap)
        {
//            EZBL_printf("\n\nNew firmware detected. Changing partitions now.");
            EZBL_printf("\n\nNew firmware detected. Changing partitions in 500ms.");
            vTaskDelay(xMsToTicks(2000));
            EZBL_FIFOFlush(EZBL_STDOUT, NOW_sec);       // Flush all TX status messages from printf() statements
            EZBL_PartitionSwap();                       // Perform the partition swap and branch to 0x000000 on the (presently) Inactive Partition
        }

        ButtonRead();
        if(ButtonsReleased & 0x1)       // Check if user pushed then released S4 (right-most Explorer 16/32 button)
        {
            EZBL_printf("\n\nButton push detected: swapping partitions manually");
            EZBL_printf("\nAlso second button held:"
                            "\n  Decrementing FBTSEQ on Inactive Partition so it is reset active...");
                i = EZBL_WriteFBTSEQ(0, 0, -1);
                EZBL_printf(i == 1 ? "success" : "failed (%d)", i);
//            if(ButtonsLastState & 0x8)  // Also check if user is holding down S3 (left-most Explorer 16/32 button)
//            {
//                EZBL_printf("\nAlso second button held:"
//                            "\n  Decrementing FBTSEQ on Inactive Partition so it is reset active...");
//                i = EZBL_WriteFBTSEQ(0, 0, -1);
//                EZBL_printf(i == 1 ? "success" : "failed (%d)", i);
//                // NOTE: if you want to change the EZBL_WriteFBTSEQ() call to
//                // program the Active Partition's FBTSEQ value, you must remove
//                // FBTSEQ's address on the Active Partition from this line at
//                // the top of ezbl_uart_dual_partition.c:
//                //     EZBL_SetNoProgramRange(0x000000, 0x400000);
//            }
            EZBL_FIFOFlush(EZBL_STDOUT, NOW_sec);       // Flush all TX status messages from printf() statements
            EZBL_PartitionSwap();                       // Perform the partition swap and branch to 0x000000 on the (presently) Inactive Partition
        }
    }
}

void startSwapPartitionTask()
{
        xTaskCreate(    vTaskSwapPartition,
                        "vTaskSwapPartition",
                        1000,
                        NULL,
                        MAX_PRIORITY,
                        &xTaskSwapPartition);
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;
    debug("vApplicationStackOverflowHook");

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationIdleHook( void )
{
	/* Schedule the co-routines from within the idle task hook. */
	vCoRoutineSchedule();
}

void flushComando(uint8_t *comando)
{
    int i;
    for (i = 0; i < MAX_COMMAND_LENGHT ; i++){
        comando[i] = 0;
    }
}

void setDeviceConfig()
{
    configDevice.type = 3;
    configDevice.serial = 9998;
    configDevice.fw_version = 1;
    configDevice.factoryReset = 0x55;
}

void setEstacionConfig()
{
    setDeviceConfig();
    /*	actualiza las variables del RTCC	*/
	get_rtcc_datetime( &tiempo );

	/*	lee la configuracion del equipo */
	estacion.tipo = configDevice.type;
	estacion.num_serie = configDevice.serial;
	estacion.hora = tiempo.hora;		//	getTimeDate(_HOUR_);
	estacion.min = tiempo.minutos;		//	getTimeDate(_MIN_);
	estacion.dia = tiempo.dia;			//	getTimeDate(_DAY_);
	estacion.mes = tiempo.mes;			//	getTimeDate(_MON_);
	estacion.anio = tiempo.anio;		//	getTimeDate(_YEAR_);
	estacion.InicioMuestras = (estacion.min & 0xF0) + 9;	//	enviado en formato BCD
	estacion.intentosConex = getConnAttempts();
	estacion.dAlmacenados = getSamplesTotal();
	estacion.pEscritura = getSamplesWrite();
	estacion.pLectura = getSamplesRead();
	estacion.fw_version = configDevice.fw_version;

	/*	tipo de reset del equipo */
	estacion.tiporeset = RCON;
}

void printConfigRegisters(){
    EZBL_printf("\n  _FSEC initial config = 0x%04X", BWRP_OFF & BSS_OFF & BSEN_OFF & GWRP_OFF & GSS_OFF & CWRP_OFF & CSS_DIS & AIVTDIS_DISABLE);
    EZBL_printf("\n  _FOSCSEL initial config = 0x%04X", FNOSC_FRC & PLLMODE_PLL96DIV2 & IESO_OFF);
    EZBL_printf("\n  _FOSC initial config = 0x%04X", POSCMD_XT & OSCIOFCN_ON & SOSCSEL_ON & PLLSS_PLL_PRI & IOL1WAY_OFF & FCKSM_CSECME);
    EZBL_printf("\n  _FWDT initial config = 0x%04X", WDTPS_PS1024 & FWPSA_PR32 & SWON & WINDIS_OFF & WDTCMX_LPRC & WDTCLK_LPRC);
    EZBL_printf("\n  _FPOR initial config = 0x%04X", BOREN_ON & LPCFG_ON & DNVPEN_ENABLE);
    EZBL_printf("\n  _FICD initial config = 0x%04X", PGX2 & JTAGEN_OFF & BTSWP_ON);
    EZBL_printf("\n  _FDEVOPT1 initial config = 0x%04X", ALTCMPI_DISABLE & TMPRPIN_OFF & SOSCHP_ON & ALTVREF_ALTVREFDIS);
    EZBL_printf("\n  _FBOOT initial config = 0x%04X", BTMODE_DUAL);  
    EZBL_printf("\n  _RCON = 0x%04X", RCON);  
}

void printConfigRealRegistersPartition1(){
    EZBL_printf("\n  ------------------------------------- ");
    EZBL_printf("\n  _FSEC real config = 0x%04X", EZBL_ReadFlash(0x055f00));
    EZBL_printf("\n  _FOSCSEL real config = 0x%04X", EZBL_ReadFlash(_FOSCSEL));
    EZBL_printf("\n  _FOSC real config = 0x%04X", EZBL_ReadFlash(_FOSC));
    EZBL_printf("\n  _FWDT real config = 0x%04X", EZBL_ReadFlash(_FWDT));
    EZBL_printf("\n  _FPOR real config = 0x%04X", EZBL_ReadFlash(_FPOR));
    EZBL_printf("\n  _FICD real config = 0x%04X", EZBL_ReadFlash(_FICD));
    EZBL_printf("\n  _FDEVOPT1 real config = 0x%04X", EZBL_ReadFlash(_FDEVOPT1));
    EZBL_printf("\n  _FBOOT real config = 0x%04X", EZBL_ReadFlash(_FBOOT));  
}

void printConfigRealRegistersPartition2(){
    EZBL_printf("\n  ------------------------------------- ");
    EZBL_printf("\n  _FSEC real config = 0x%04X", EZBL_ReadFlash(_FSEC+0x400000));
    EZBL_printf("\n  _FOSCSEL real config = 0x%04X", EZBL_ReadFlash(_FOSCSEL+0x400000));
    EZBL_printf("\n  _FOSC real config = 0x%04X", EZBL_ReadFlash(_FOSC+0x400000));
    EZBL_printf("\n  _FWDT real config = 0x%04X", EZBL_ReadFlash(_FWDT+0x400000));
    EZBL_printf("\n  _FPOR real config = 0x%04X", EZBL_ReadFlash(_FPOR+0x400000));
    EZBL_printf("\n  _FICD real config = 0x%04X", EZBL_ReadFlash(_FICD+0x400000));
    EZBL_printf("\n  _FDEVOPT1 real config = 0x%04X", EZBL_ReadFlash(_FDEVOPT1+0x400000));
    EZBL_printf("\n  _FBOOT real config = 0x%04X", EZBL_ReadFlash(_FBOOT+0x400000));
}

