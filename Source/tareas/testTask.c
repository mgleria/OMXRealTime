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
    #define TASK_PERIOD_MS 3000
    TickType_t taskDelay;
    taskDelay = xMsToTicks(TASK_PERIOD_MS);
    muestra_t muestra;
    static uint16_t potentiometer;
    static uint16_t temperature;
    uint8 resultPutSample = -1;
    
    sensorsConfig();
    TMR3_Start();
    
    init_sample(&muestra);
    
//    printMemoryPointers();
    printf("sizeof(muestra_t):%d\r\n",sizeof(muestra_t));
    printf("sizeof(trama_muestra_t):%d\r\n",sizeof(trama_muestra_t));
    printf("sizeof(trama_config_t):%d\r\n",sizeof(trama_config_t));
    printf("sizeof(trama_inicio_t):%d\r\n",sizeof(trama_inicio_t));
    
    //Loop principal
    for(;;)
    {
        potentiometer = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
        temperature = ADC_Read10bit( ADC_CHANNEL_TEMPERATURE_SENSOR );
        muestra.clima.luzDia = potentiometer;
        muestra.clima.temper = getTemperature(temperature);
        muestra.clima.lluvia = getAccumulatedRain();
        
        printf("sample.clima.luzDia: %d\r\n",muestra.clima.luzDia);
        printf("sample.clima.temper: %d\r\n",muestra.clima.temper);
        printf("sample.clima.lluvia: %d\r\n",muestra.clima.lluvia);
        
        //Limpio el contador por soft del TMR3
        clearAccumulatedRain();
        
        resultPutSample = putSample(&muestra);
//        printMemoryPointers();
        if(resultPutSample){
            printf("Muestra guardada exitosamente:%d\r\n",resultPutSample);
        }
        else{
            printf("ERROR al guardar la muestra:%d\r\n",resultPutSample);
        }       
        
        printf("TRAMA TIPO MUESTRA\r\n");
        setServerFrame(muestras,0);
        
        vTaskDelay(taskDelay); 
        
        printf("TRAMA TIPO REGISTRO\r\n");
        setServerFrame(registro,0);
        
        vTaskDelay(taskDelay); 
        
        printf("TRAMA TIPO CONFIGURACION\r\n");
        setServerFrame(configuracion,0);
        
//        Espera arbitraria para dormir la tarea        
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