#include "tareas/CLITask.h"
#include "ezbl.h"

//Handle referenciado en tmr5.c para uso de xTaskNotify()
TaskHandle_t xCLIHandle;

void vTaskCLI( void *pvParameters ){
    
    UBaseType_t uxHighWaterMarkShell;
    // Seccion de inicializacion
    uint8_t comando[MAX_COMMAND_LENGHT] ={0};
    string respuesta[MAX_RESP_LENGHT]={0};
    portBASE_TYPE bytesEnviados, bytesRecibidos;
    bytesEnviados = bytesRecibidos = -1;
    
    TMR2_Start();
    
    // Cuerpo de la tarea
    for( ;; ){   
            uxHighWaterMarkShell = uxTaskGetStackHighWaterMark( NULL );
            //Detengo el TMR2 que inicio al final de _U1RXInterrupt().
//            TMR2_Stop();
            //Leemos el comando recibido por la UART
            
            bytesRecibidos = EZBL_FIFORead(comando, EZBL_STDIN, EZBL_STDIN->dataCount);
            
            LEDToggle(0x01);            
            
            //Si se recibio algo, procesamos el comando para obtener una respuesta
            if(bytesRecibidos>0) {
                bytesRecibidos=0;
                strcpy( respuesta, processCmdLine( &comando ) );
                //Envio la respuesta por la UART hacia la PC.
                
                EZBL_printf("\nRespuesta comando: ");
                EZBL_printf(respuesta);
            }
            //Si se envia la respuesta, se suspende la tarea hasta que llegue el proximo comando.
            flushComando(comando); 
            vTaskSuspend( NULL );
            
    }
}

void startCLITask(){
    
    xTaskCreate(    vTaskCLI,
                    "vTaskCLI",
                    1000,
                    NULL,
                    MAX_PRIORITY-2,
                    &xCLIHandle);
}