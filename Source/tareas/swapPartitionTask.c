#include "tareas/swapPartitionTask.h"
#include "ezbl.h"
#include "FreeRTOS.h"
#include "freeRTOS/task.h"

TaskHandle_t xTaskSwapPartition;
extern volatile int timeForPartitionSwap __attribute__((persistent));    // Global variable signaling when we complete a successful firmware update on the Inactive Partition

void vTaskSwapPartition( void *pvParameters )
{
    unsigned long ledBlinkTimer;
    int i;

    
    if(!_SFTSWP)
    {
        EZBL_ConsoleClear();    // Writes "Shift In", "Clear Screen" and "Reset Attributes" ANSI control codes to EZBL_STDOUT
    }
    
    ledBlinkTimer = NOW_32();
    
    for(;;)
    {
        ClrWdt();
        // Every half second toggle an LED (1 Hz blink rate) to indicate we are alive
//        if(NOW_32() - ledBlinkTimer > NOW_sec/2u)
//        {
//            LEDToggle(0x07);
//            ledBlinkTimer += NOW_sec/2u;
//        }
        LEDToggle(0x08);


        if(timeForPartitionSwap)
        {
            //Introduzco esta demora para que la tarea vTaskBootloader pueda terminar de enviar todas sus tramas al host
            EZBL_printf("\n\nNew firmware detected. Changing partitions in 500ms.");
            vTaskDelay(xMsToTicks(500));
            EZBL_FIFOFlush(EZBL_STDOUT, NOW_sec);       // Flush all TX status messages from printf() statements
            EZBL_PartitionSwap();                       // Perform the partition swap and branch to 0x000000 on the (presently) Inactive Partition
        }

        ButtonRead();
        if(ButtonsReleased & 0x1)       // Check if user pushed then released S4 (right-most Explorer 16/32 button)
        {
            EZBL_printf("\n\nButton push detected: swapping partitions manually");
            EZBL_printf("\nDecrementing FBTSEQ on Inactive Partition so it is reset active...");
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
                        "SWAP",
                        1000,
                        NULL,
                        MAX_PRIORITY-2,
                        &xTaskSwapPartition);
        
        debug("startSwapPartitionTask()");
}