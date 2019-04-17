#include "drivers/modem.h"

SemaphoreHandle_t xMutexModem = NULL;

TickType_t xTimeModemMutex;

void    setupModem(){
    xMutexModem  = xSemaphoreCreateMutex();
    if(!xMutexModem){
        debug("ERROR en la creaci�n del mutex de Memoria");
        //El programa no puede seguir, hay que detenerlo.
    }
    xTimeModemMutex = xMsToTicks(T_ESPERA_MUTEX_MODEM_MS);
}

int16_t	SendATCommand( const char* cmd, uint16 d ){
    
    int16_t writedBytes = 0;
    
    /*Si hay que esperar antes de enviar el comando al modem, aqu� es donde */
    if(d>0) vTaskDelay(xMsToTicks(d));
    
    /* Ver si puedo obtener el semaforo. Si este no est� disponible
     * esperar xTimeModemMutex y volver a probar, si falla a�n pasado  
     * xTimeModemMutex, devuelve -1 */
    if( xSemaphoreTake( xMutexModem, xTimeModemMutex ) == pdTRUE ){
//        debug(cmd);
        writedBytes = UART3_WriteBuffer(cmd,strlen(cmd));
        xSemaphoreGive(xMutexModem);
        /*WARNING: La siguiente l�nea esta comentada porque rompe la ejecucion.
         Estimo que es porque 'cmd' no termina en '\0'. Requiere evaluaci�n.*/
//        EZBL_printf("\nCMD MDM: %s",cmd);
        return writedBytes;
    }
    else{
        debug("ERROR no se pudo tomar el mutex del modem");
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
        readedBytes = UART3_ReadBuffer(buffer, MODEM_BUFFER_SIZE);
        
        if(readedBytes) {
        /*WARNING: La siguiente l�nea esta comentada porque rompe la ejecucion.
         Estimo que es porque 'cmd' no termina en '\0'. Requiere evaluaci�n.*/
//            EZBL_printf("\nMDM Response: %s",buffer);
        }
        else
            debug("Respuesta de modem vac�a");    
        return 1;
    }
    return 0;
}

uint8_t	getServerResponse( char* buffer, TickType_t responseDelay )
{
    uint16_t readedBytes = 0;
     
    readedBytes = UART3_ReadBuffer(buffer, MODEM_BUFFER_SIZE);
    
    if(readedBytes)
        return 1;
    else {
        vTaskDelay(responseDelay);
        return 0;
    }   
}
    
