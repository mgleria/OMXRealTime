#include "drivers/modem.h"

SemaphoreHandle_t xMutexModem = NULL;

TickType_t xTimeModemMutex;

void    setupModem(){
    xMutexModem  = xSemaphoreCreateMutex();
    if(!xMutexModem){
        printf("ERROR en la creaci�n del mutex de Memoria");
        //El programa no puede seguir, hay que detenerlo.
    }
    xTimeModemMutex = xMsToTicks(T_ESPERA_MUTEX_MODEM_MS);
}

int16_t	SendATCommand( const char* text, char* tx, char* rx, uint16 t, uint16 d, uint8 f ){
    
//    int16_t writedBytes = 0;
//    
//    /*Si hay que esperar antes de enviar el comando al modem, aqu� es donde */
//    if(d>0) vTaskDelay(xMsToTicks(d));
//    
//    /* Ver si puedo obtener el semaforo. Si este no est� disponible
//     * esperar xTimeModemMutex y volver a probar, si falla a�n pasado 
//     * xTimeModemMutex, devuelve -1 */
//    if( xSemaphoreTake( xMutexModem, xTimeModemMutex ) == pdTRUE ){
//        writedBytes = UART2_WriteBuffer(text,strlen(text));
//        xSemaphoreGive(xMutexModem);
//        return writedBytes;
//    }
//    else{
//        printf("ERROR no se pudo tomar el mutex del modem.\r\n");
//        return -1;
//    }
    return 0;
}
