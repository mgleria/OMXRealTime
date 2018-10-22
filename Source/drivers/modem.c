#include "drivers/modem.h"

SemaphoreHandle_t xMutexModem = NULL;

TickType_t xTimeModemMutex;

void    setupModem(){
    xMutexModem  = xSemaphoreCreateMutex();
    if(!xMutexModem){
//        printf("ERROR en la creaci�n del mutex de Memoria");
        debugUART1("ERROR en la creaci�n del mutex de Memoria");
        //El programa no puede seguir, hay que detenerlo.
    }
    xTimeModemMutex = xMsToTicks(T_ESPERA_MUTEX_MODEM_MS);
}

int16_t	SendATCommand( const char* text, char* tx, char* rx, uint16 t, uint16 d, uint8 f ){
    
    int16_t writedBytes = 0;
    
    /*Si hay que esperar antes de enviar el comando al modem, aqu� es donde */
    if(d>0) vTaskDelay(xMsToTicks(d));
    
    /* Ver si puedo obtener el semaforo. Si este no est� disponible
     * esperar xTimeModemMutex y volver a probar, si falla a�n pasado 
     * xTimeModemMutex, devuelve -1 */
    if( xSemaphoreTake( xMutexModem, xTimeModemMutex ) == pdTRUE ){
        writedBytes = EZBL_printf(text);
        debugUART1(text);
//        writedBytes = UART2_WriteBuffer(text,strlen(text));
        xSemaphoreGive(xMutexModem);
        return writedBytes;
    }
    else{
//        printf("ERROR no se pudo tomar el mutex del modem.\r\n");
        debugUART1("ERROR no se pudo tomar el mutex del modem.\r\n");
        return -1;
    }
}

/*
 * En esta funcion se espera por la respuesta del modem y se guarda la misma en 
 * el buffer indicado.
 * Si se rebibi� alguna respuesta, esta funci�n va a retornar un 1, de lo 
 * contrario retornara 0.
 */

#define MDM_RESP_READY_NOTIFICATION     2020

uint8_t	receiveATCommand( char* buffer, uint8_t *attempts, TickType_t responseDelay )
{
    uint32_t modemResponseNotification = 0;
    uint16_t readedBytes = 0;
    
    *attempts++;
    modemResponseNotification = ulTaskNotifyTake(   pdTRUE, responseDelay ); 
    
    
    if(modemResponseNotification == MDM_RESP_READY_NOTIFICATION){
        TMR5_Stop();       
        readedBytes = EZBL_FIFORead(buffer,EZBL_COMBootIF,MODEM_BUFFER_SIZE);
//        UART2_ReadBuffer(buffer, GPRS_BUFFER_SIZE);
        return 1;
    }
    else
        return 0;
}
    
