#include "tareas/CLITask.h"

//Handle referenciado en tmr5.c para uso de xTaskNotify()
TaskHandle_t xCLIHandle;

void vTaskCLI( void *pvParameters ){
    
    UBaseType_t uxHighWaterMarkShell;
    // Seccion de inicializacion
    uint8_t comando[MAX_COMMAND_LENGHT] ={0};
    string respuesta[MAX_RESP_LENGHT]={0};
    portBASE_TYPE bytesEnviados, bytesRecibidos;
    uint16_t sizeRespuesta;
    bytesEnviados = bytesRecibidos = -1;
    
    // Cuerpo de la tarea
    for( ;; ){   
            uxHighWaterMarkShell = uxTaskGetStackHighWaterMark( NULL );
            //Detengo el TMR2 que inicio al final de _U1RXInterrupt().
            TMR2_Stop();
            //Leemos el comando recibido por la UART
            
            //  bytesRecibidos = UART1_ReadBuffer(comando,MAX_COMMAND_LENGHT);
            
            
            //Si se recibio algo, procesamos el comando para obtener una respuesta
            if(bytesRecibidos>0) {
                bytesRecibidos=0;
                strcpy( respuesta, processCmdLine( &comando ) );
                //Env�o la respuesta por la UART hacia la PC.
//                stringToIntArray(respuestaUART,respuesta);
                sizeRespuesta = strlen(respuesta);
                
                //bytesEnviados = UART1_WriteBuffer(respuesta,sizeRespuesta);
            }
            //Si se envi� la respuesta, se suspende la tarea hasta que llegue el pr�ximo comando.
            flushComando(comando); 
            vTaskSuspend( NULL );
            
    }
}

void startCLITask(){
    
    xTaskCreate(    vTaskCLI,
                    "vTaskCLI",
                    1000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xCLIHandle);
}