#include "tareas/testTask.h"
#include "tareas/sampleTask.h"


TaskHandle_t xTestHandle;

/**********************************************************************************************/
/**
 * \brief
 * Tarea freeRTOS que se usa en desarrollo para realizar pruebas e interactuar con otras tareas
 * @return	void
 */
void vTaskTest( void *pvParameters )
{
    #define TASK_PERIOD_MS 5000
    TickType_t taskDelay;
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
    
    //Buffer de comunicación entrante y saliente con el modem
    static char gprsBuffer[GPRS_BUFFER_SIZE]={0};
    static char header[FRAME_HEADER_SIZE]={0};
    static headerOptions_t headerIndex; 
    
    const char ch = '\n';
    char *ret;

    
    
    //Loop principal
    for(;;)
    {
        strcpy(gprsBuffer,"\r\n#SRECV: 1,17\r\n024F004900003230303831383133303300\r\n\r\nOK\r\n\r\n");
        
        ret = findNthCharacterOcurrence(gprsBuffer,ch,1);

        //                    strncpy(header,++ret,sizeof(header));
        strncpy(header,++ret,FRAME_HEADER_SIZE-1);
        headerIndex = getHeaderIndex(header);

        printf("Rta SERVER: %s\r\n",gprsBuffer);                   
        printf("Header Rta SERVER: %s\r\n",header);
        printf("###########\r\n");
        printf("headerIndex: %d\r\n",headerIndex);
        
//        Espera arbitraria para dormir la tarea        
        vTaskDelay(taskDelay);
        
        strcpy(gprsBuffer,"\r\n#SRECV: 1,17\r\n024E004900003230303831383133303300\r\n\r\nOK\r\n\r\n");
        ret = findNthCharacterOcurrence(gprsBuffer,ch,1);

        //                    strncpy(header,++ret,sizeof(header));
        strncpy(header,++ret,FRAME_HEADER_SIZE-1);
        headerIndex = getHeaderIndex(header);

        printf("Rta SERVER: %s\r\n",gprsBuffer);                   
        printf("Header Rta SERVER: %s\r\n",header);
        printf("###########\r\n");
        printf("headerIndex: %d\r\n",headerIndex);
        
        vTaskDelay(taskDelay);
        
        strcpy(gprsBuffer,"\r\n#SRECV: 1,17\r\n004F004900003230303831383133303300\r\n\r\nOK\r\n\r\n");
        ret = findNthCharacterOcurrence(gprsBuffer,ch,1);

        //                    strncpy(header,++ret,sizeof(header));
        strncpy(header,++ret,FRAME_HEADER_SIZE-1);
        headerIndex = getHeaderIndex(header);

        printf("Rta SERVER: %s\r\n",gprsBuffer);                   
        printf("Header Rta SERVER: %s\r\n",header);
        printf("###########\r\n");
        printf("headerIndex: %d\r\n",headerIndex);
        
        vTaskDelay(taskDelay);
        
        strcpy(gprsBuffer,"\r\n#SRECV: 1,17\r\n004E004900003230303831383133303300\r\n\r\nOK\r\n\r\n");
        ret = findNthCharacterOcurrence(gprsBuffer,ch,1);

        //                    strncpy(header,++ret,sizeof(header));
        strncpy(header,++ret,FRAME_HEADER_SIZE-1);
        headerIndex = getHeaderIndex(header);

        printf("Rta SERVER: %s\r\n",gprsBuffer);                   
        printf("Header Rta SERVER: %s\r\n",header);
        printf("###########\r\n");
        printf("headerIndex: %d\r\n",headerIndex);
        
        vTaskDelay(taskDelay);
    }
}

void startTestTask(){
    //Funcion de inicialización previa
    
        xTaskCreate(    vTaskTest,
                    "vTaskTest",
                    2000,
                    NULL,
                    MAX_PRIORITY-1,
                    &xTestHandle);
    
}