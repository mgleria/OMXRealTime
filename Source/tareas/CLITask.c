#include "tareas/CLITask.h"


//Handle referenciado en tmr5.c para uso de xTaskNotify()
TaskHandle_t xCLIHandle;

void vTaskCLI( void *pvParameters ){
    
    UBaseType_t uxHighWaterMarkShell;
    // Seccion de inicializacion
    uint8_t comando[MAX_COMMAND_LENGHT] ={0};
    string respuesta[MAX_RESP_LENGHT]={0};
    portBASE_TYPE bytesEnviados, bytesRecibidos;
    bytesEnviados = bytesRecibidos = -1;
    uint32_t ulNotifiedValue;
    
    TMR2_Start();
    
    // Cuerpo de la tarea
    for( ;; ){   
            uxHighWaterMarkShell = uxTaskGetStackHighWaterMark( NULL );

            LEDToggle(0x4);
            
            /* La siguiente línea va a bloquear la tarea CLI hasta que llegue 
             * una notificacion. En este caso no filtramos cuál porque solo
             * estamos enviando una sola notificacion desde una sola fuente: 
             * TMR2_CallBack()*/
            ulNotifiedValue = ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
            
            bytesRecibidos = EZBL_FIFORead(comando, EZBL_STDIN, EZBL_STDIN->dataCount);     
            
            //Si se recibio algo, procesamos el comando para obtener una respuesta
            if(bytesRecibidos>0) {
                bytesRecibidos=0;
                strcpy( respuesta, processCmdLine( &comando ) );
                //Envio la respuesta por la UART hacia la PC.
                
                EZBL_printf("\nRespuesta comando: ");
                EZBL_printf(respuesta);
            }
            //Si se envia la respuesta, se suspende la tarea hasta que llegue el proximo comando.
//            flushComando(comando); 
            flushBuffer(comando, MAX_COMMAND_LENGHT);
    }
}

void startCLITask(){
    
    xTaskCreate(    vTaskCLI,
                    "CLI",
                    1000,
                    NULL,
                    MAX_PRIORITY-2,
                    &xCLIHandle);
    
    debug("startCLITask()");
}