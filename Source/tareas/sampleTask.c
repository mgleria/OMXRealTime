#include <stdio.h>

#include    "tareas/sampleTask.h"
#include    "funciones/memory.h"
#include    "utilities.h"
#include    "tmr3.h"

#include    "FreeRTOS.h"
#include    "freeRTOS/semphr.h"

#include    "ezbl.h"


static void prvPasiveCallback (TimerHandle_t xTimer);
static void prvActiveCallback (TimerHandle_t xTimer);
static void prvDataCallback (TimerHandle_t xTimer);
static void prvDebouncingRainCallback (TimerHandle_t xTimer);

static void FSM_SampleTask(uint32_t status);
void  sensorsConfig();
uint16_t getAccumulatedRain();
void clearAccumulatedRain();
static void setStatusFSM(uint32_t nextStatus);
static void resetSyncVariables();
uint16_t getTemperature(uint16_t adcValue);

//Handlers software timers
TimerHandle_t xPassiveSamplingTime;
TimerHandle_t xActiveSamplingTime;
TimerHandle_t xSampleData;
TimerHandle_t xRainDebouncing;

//Tiempos usados en software timers
TickType_t  xTimePasive, xTimeActive, xTimeData, xTimeRainDebouncing, xTimeMemoryMutex;

TaskHandle_t xSampleHandle;
extern TaskHandle_t xGprsHandle;

static muestra_t sample;

static uint32_t status;
static bool waitForNotify, SyncServerTime;
static uint16_t syncCounter;

static uint16_t potentiometer;
static uint16_t temperature;

SemaphoreHandle_t xMutexMemory = NULL;

void startSampleTask(){
    
    xTaskCreate(    vTaskSample,
                    "vTaskSample",
                    1000,
                    NULL,
                    MAX_PRIORITY,
                    &xSampleHandle);
    
    xTimeMemoryMutex = xMsToTicks(T_ESPERA_MUTEX_MEM_MS);
    softwareTimers_create();
    sensorsConfig();
    TMR3_Start();
    
    debugUART1("startSampleTask()\r\n");
}

void vTaskSample( void *pvParameters ){
    UBaseType_t uxHighWaterMarkSample;
    
// Seccion de inicializacion
    waitForNotify = false;
    
    
    xMutexMemory = xSemaphoreCreateMutex();
    if(!xMutexMemory){
//        printf("ERROR en la creación del mutex de Memoria");
        debugUART1("ERROR en la creación del mutex de Memoria");
        //El programa no puede seguir, hay que detenerlo.
    }
    
    setStatusFSM(SYNC_SERVER_TIME); 
    debugUART1("Initial section Sample Task\r\n");
    
        
    // Cuerpo de la tarea
    for( ;; ){
//        printf("--------------------Sample Task--------------------\r\n");
//        printf("////////////////////Sample Task\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\r\n");
        
        if(waitForNotify){
            //Se bloquea la tarea a la espera de nuevas notificaciones que llegarán desde
            //las callbacks timers o desde una interrupción generada por el driver de un sensor
            status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before
                                                    exiting. */
                                        portMAX_DELAY ); /* Block indefinitely. */
            waitForNotify = false;
            
        }
        else{
            FSM_SampleTask(status);
        }
        
        
        
        uxHighWaterMarkSample = uxTaskGetStackHighWaterMark( NULL );
    }
}

static void FSM_SampleTask(uint32_t status){
       
    uint32_t asyncEvents;
    uint32_t syncEvents;
    rtcc_t rtcc;
    
    /*Los cambios de estado ocurren en los callback de los timers o en otras
     funciones externas*/
    switch(status){  
        case SYNC_SERVER_TIME:
            debugUART1("SYNC_SERVER_TIME\r\n");

            //Chequeo si la estacion ya se registro
//            SyncServerTime = isRegistered();
            SyncServerTime = true;
            //Si no esta registrado el equipo, aguardo
            if( !SyncServerTime ){
                status = ulTaskNotifyTake(  pdTRUE,  portMAX_DELAY ); 
                
                if(status == SYNC_SERVER_TIME_NOTIFICATION){
                    setStatusFSM(ASYNC_SAMPLING);
                    /*Cuando este softTimer expire, se cambia de estado a
                    * SYNC_SAMPLING*/
                    xTimerStart(xPassiveSamplingTime,0);
                
                    /*ACA TENGO QUE ESPERAR EL TIEMPO ADECUADO PARA QUE TOME LAS 
                    * MUESTRAS A LAS HORAS CORRESPONDIENTES*/
                }
            }
            else{
                setStatusFSM(ASYNC_SAMPLING);
                //Cuando este softTimer expire, se cambia de estado a SYNC_SAMPLING
                xTimerStart(xPassiveSamplingTime,0);
            }
            break;
            
        //to-do: Cambiar portMAX_DELAY por el valor apropiado    
        case ASYNC_SAMPLING:
            debugUART1("ASYNC_SAMPLING\r\n");
//            printf("ASYNC_SAMPLING");
            
            //Wait for async events
            asyncEvents = ulTaskNotifyTake( 
                pdTRUE,  /* Clear the notification value before exiting. */
                xSegToTicks(T_MUESTREO_PASIVO_S) ); /* Max Block time. */ 
            
            switch(asyncEvents){
                case ASYNC_SENSOR1:
                    //code ASYNC_SENSOR1
                    break;
                case ASYNC_SENSOR2:
                    //code ASYNC_SENSOR1
                    break;
                case ASYNC_SENSOR3:
                    //code ASYNC_SENSOR1
                    break;
                case ASYNC_SENSOR4:
                    //code ASYNC_SENSOR1
                    break;      
            }
            break;
        //to-do: Cambiar portMAX_DELAY por el valor apropiado
        case SYNC_SAMPLING:
//            printf("SYNC_SAMPLING %d\r",syncCounter);
            debugUART1("SYNC_SAMPLING");
            syncEvents = ulTaskNotifyTake( 
                pdTRUE,  /* Clear the notification value before exiting. */
                xSegToTicks(T_MUESTREO_DATO_S) ); /* Max Blocking time. */
            
            switch(syncEvents){
                case SYNCHRONOUS:
//                    printf("case SYNCHRONOUS:");
                    syncCounter++;
                    //Para cada medición sincrónica debe definirse un acumulador
                    //acum_sensor_n += functionToGetSampleSensorN();
                    potentiometer += ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
                    temperature += ADC_Read10bit( ADC_CHANNEL_TEMPERATURE_SENSOR );
//                    printf("syncCounter: %d\r\n",syncCounter);
                    break;
                default:
                    //indicar con algun codigo de error que algo anduvo mal
                    debugUART1("syncEvents default\r\n");
                    break;
            }
            break;
        case SAVE_AND_PACKAGE:
            debugUART1("SAVE_AND_PACKAGE\r\n");
//            printf("SAVE_AND_PACKAGE");
            //Preparar la muestra
/////////////////////MUESTRAS TOMADAS SINCRONICAMENTE///////////////////////////
            if(syncCounter>0){
                sample.clima.hum = potentiometer/syncCounter;
                sample.clima.temper = getTemperature(temperature/syncCounter);
//                printf("sample.clima.hum: %d\r\n",sample.clima.hum);
//                printf("sample.clima.temper: %d\r\n",sample.clima.temper);
            }
            else{
                //to-do: Ver que se hace en caso de que no se hayan tomado 
                //muestras sincronicas. Reinicio?
                debugUART1("syncEvents syncCounter<0\r\n");
            }
            syncCounter = 0;
/////////////////////MUESTRAS TOMADAS ASINCRONICAMENTE//////////////////////////
            //to-do: Hacer una función que me permita escalar esto mejor
            sample.clima.lluvia = getAccumulatedRain();
//            printf("sample.clima.lluvia: %d\r\n",sample.clima.lluvia);
            //Limpio el contador por soft del TMR3
            clearAccumulatedRain();
            
/////////////////////////////////TIMESTAMP//////////////////////////////////////            
                  
            get_rtcc_datetime(&rtcc);

            sample.anio = rtcc.anio;
            sample.mes = rtcc.mes;
            sample.dia = rtcc.dia;
            sample.hora = rtcc.hora;
            sample.minutos = rtcc.minutos;        
/////////////////////GUARDANDO MUESTRA EN MEM PERSISENTE////////////////////////
            /* Ver si puedo obtener el semaforo. Si este no está disponible
             * esperar xTimeMemoryMutex y volver a probar */
            uint8 resultPutSample = 0;
            char resultGetSample = 0;
            muestra_t returnedSample;
            if( xSemaphoreTake( xMutexMemory, xTimeMemoryMutex ) == pdTRUE )
            {
                resultPutSample = putSample(&sample);
//                printMemoryPointers();
//                printf("resultPutSample->%d.\r\n",resultPutSample);
                if(resultPutSample){
//                    printf("Muestra guardada exitosamente.\r\n");
                    debugUART1("Muestra guardada exitosamente.");
//                    printMemoryPointers();
                }
                else{
//                    printf("ERROR al guardar la muestra.\r\n");
                    debugUART1("ERROR al guardar la muestra.");
                }
                resultGetSample = getSample(&returnedSample,0);
//                printf("getSample(&sample,0)->%s \r\n",resultGetSample);
//                printf("sample.clima.hum: %d\r\n",returnedSample.clima.hum);
//                printf("sample.clima.temper: %d\r\n",returnedSample.clima.temper);
//                printf("sample.clima.lluvia: %d\r\n",returnedSample.clima.lluvia);
//                printMemoryPointers();
                
                /* Terminamos de usar el recurso, por lo que devolvemos el
                 * mutex */
                xSemaphoreGive( xMutexMemory );
                
                 // Notifico a la tarea GPRS que hay una nueva trama para enviar
                xTaskNotify(    xGprsHandle,
                                NEW_SAMPLE_NOTIFICATION,
                                eSetValueWithOverwrite);
                
                setStatusFSM(ASYNC_SAMPLING);
            }
            else{
//                printf("ERROR no se pudo tomar el mutex de memoria.\r\n");
                debugUART1("ERROR no se pudo tomar el mutex de memoria.");
            }
            break;
    }
}

static void setStatusFSM(uint32_t nextStatus){
    status = nextStatus;
}

static void softwareTimers_create(){
    
    xTimePasive = xSegToTicks(T_MUESTREO_PASIVO_S);
    xTimeActive = xSegToTicks(T_MUESTREO_ACTIVO_S);
    xTimeData   = xSegToTicks(T_MUESTREO_DATO_S);
    
    xTimeRainDebouncing = xMsToTicks(T_ANTIREBOTE_LLUVIA_MS);
    
    xPassiveSamplingTime    = xTimerCreate("pasivePeriod",xTimePasive,pdTRUE,0,prvPasiveCallback);
    xActiveSamplingTime     = xTimerCreate("activePeriod",xTimeActive,pdTRUE,0,prvActiveCallback);
    xSampleData             = xTimerCreate("dataPeriod",xTimeData,pdTRUE,0,prvDataCallback) ;
    
    xRainDebouncing       = xTimerCreate("rainDebouncingPeriod",xTimeRainDebouncing,pdTRUE,0,prvDebouncingRainCallback) ;
    
}

void  sensorsConfig(){
    //Potenciï¿½metro - ADC
    ADC_SetConfiguration ( ADC_CONFIGURATION_DEFAULT );
    
    ADC_ChannelEnable ( ADC_CHANNEL_POTENTIOMETER );
    ADC_ChannelEnable ( ADC_CHANNEL_TEMPERATURE_SENSOR );
       
}

static void prvPasiveCallback (TimerHandle_t xTimer){
    
//    vLedToggleLED(0);
    xTimerStop(xPassiveSamplingTime,0);
    xTimerStart(xActiveSamplingTime,0);
    xTimerStart(xSampleData,0);
//    xTaskNotify(xSampleHandle,SENSOR_1,eSetValueWithOverwrite);
    
    setStatusFSM(SYNC_SAMPLING);
    resetSyncVariables();
}

static void prvActiveCallback (TimerHandle_t xTimer){
//    vLedToggleLED(1);
    
    xTimerStop(xSampleData,0);
    xTimerStop(xActiveSamplingTime,0);
    xTimerStart(xPassiveSamplingTime,0);
    
    setStatusFSM(SAVE_AND_PACKAGE);
    
//    xTaskNotify(xSampleHandle,SENSOR_2,eSetValueWithOverwrite);
}

static void prvDataCallback (TimerHandle_t xTimer){
//    vLedToggleLED(2);
    xTaskNotify(xSampleHandle,SYNCHRONOUS,eSetValueWithOverwrite);
    
}

static void prvDebouncingRainCallback (TimerHandle_t xTimer){
    
}

void init_sample(muestra_t *muestra)
{    
    muestra->cmd=0x01;
    muestra->tipo=0x03;
    muestra->num_serie=0x0727;
    muestra->hora=0x05;
    muestra->minutos=0x06;
    muestra->dia=0x07;
    muestra->mes=0x08;
    muestra->anio=0x09;
    muestra->senial =0x0A;
    muestra->clima.temper = 0x0C0B;
    muestra->clima.hum = 0x0E0D;
    muestra->clima.presion = 972.12345; //0x447307e7
    muestra->clima.lluvia = 0x1413;
    muestra->clima.viento.direccionP = 0x15;
    muestra->clima.viento.velocidadP = 0x1716;
    muestra->clima.viento.direccionM = 0x1918;
    muestra->clima.viento.velocidadM = 0x1B1A;
    muestra->clima.luzDia = 0x1D1C;
    muestra->clima.radiacionSolar = 0x1F1E;
    muestra->clima.tempSuelo1 = 0x2120;
    muestra->clima.humSuelo1 = 0x2322;
    muestra->clima.tempSuelo2 = 0x2524;
    muestra->clima.humSuelo2 = 0x2726;
    muestra->clima.tempSuelo3 = 0x2928;
    muestra->clima.humSuelo3 = 0x2B2A;
    muestra->clima.humHoja = 0x2D2C;
    muestra->clima.tempHoja = 0x2F2E;
    muestra->corriente1=0x3130;
    muestra->corriente2=0x3332;  
    muestra->bateria=0x3534; 
    muestra->periodo=0x36;
    muestra->sensorHab1=0x37;
    muestra->sensorHab2=0x38;
    muestra->sensorHab3=0x39;
//    strcpy(muestra->_reserved_,"\x40\x3F\x3E\x3D\x3C\x3B");
       
}

void assembleSample(muestra_t *muestra)
{
    
//    rtcc_t rtcc;
//    
//    get_rtcc_datetime(&rtcc);
//    
//    muestra->anio = rtcc.anio;
//    muestra->mes = rtcc.mes;
//    muestra->dia = rtcc.dia;
//    muestra->hora = rtcc.hora;
//    muestra->minutos = rtcc.minutos;
    
    muestra->clima.hum = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
   
}

static void resetSyncVariables(){
    syncCounter = 0;
    potentiometer = 0;
    temperature = 0;
}

//Función de transferencia del sensor TC1047A embebido en Explorer16/32
uint16_t getTemperature(uint16_t adcValue){
    // VOUT = (10 mV/°C) (Temperature °C) + 500 mV
    // 3300mV/1024*ADC=(10mV/ºC)*(TºC)+500mV 
    uint16_t temp = (adcValue*165/512)-50;
    return temp;
}

uint16_t getAccumulatedRain(){
    /*Se está usando el contador provisto por el driver de TMR3 ya que no se 
     pudo configurar el TMR3 para que sea un contador de eventos asincronicos
     como se pretendía*/
    return TMR3_SoftwareCounterGet();
}

void clearAccumulatedRain(){
    TMR3_SoftwareCounterClear();
}
