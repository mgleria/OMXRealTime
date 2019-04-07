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
static void setStatusFSM(uint32_t nextStatus);
static void resetSyncVariables();
//static void softwareTimers_create();

void  sensorsConfig();
uint16_t getAccumulatedRain();
void clearAccumulatedRain();
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
    
    debug("startSampleTask()");
}

void vTaskSample( void *pvParameters ){
    UBaseType_t uxHighWaterMarkSample;
    
// Seccion de inicializacion
    waitForNotify = false;
    
    
    xMutexMemory = xSemaphoreCreateMutex();
    debug("xMutexMemory created");
    if(!xMutexMemory){
        debug("ERROR en la creacion del mutex de Memoria");
        //El programa no puede seguir, hay que detenerlo.
    }
    
    setStatusFSM(SYNC_SERVER_TIME); 
    debug("Initial section Sample Task");
    
        
    // Cuerpo de la tarea
    for( ;; ){
        debug("--------------------Sample Task--------------------");
        LEDToggle(0x1);
        
        if(waitForNotify){
            debug("waitForNotify true");
            //Se bloquea la tarea a la espera de nuevas notificaciones que llegaran desde
            //las callbacks timers o desde una interrupcion generada por el driver de un sensor
            status = ulTaskNotifyTake(  pdTRUE,  /* Clear the notification value before
                                                    exiting. */
                                        portMAX_DELAY ); /* Block indefinitely. */
            //to-do: Cambiar portMAX_DELAY por el valor apropiado
            waitForNotify = false;
            
        }
        else{
            debug("FSM_SampleTask()");
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
        /*El estado SYNC_SERVER_TIME debe ser removido tras implementar el RF10
         * el cual asegurará que el equipo tenga la hora correcta antes de 
         * comenzar a tomar muestras. */
        case SYNC_SERVER_TIME:
            debug("SYNC_SERVER_TIME");

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
                
        /*to-do: Revisar si este estado es o no necesario. Quizá sea necesario 
         cuando se requiera pos procesamiento de los datos capturados asincronicamente*/
        case ASYNC_SAMPLING:
            debug("ASYNC_SAMPLING");
            
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
        case SYNC_SAMPLING:
            debug("SYNC_SAMPLING");
            syncEvents = ulTaskNotifyTake( 
                pdTRUE,  /* Clear the notification value before exiting. */
                xSegToTicks(T_MUESTREO_DATO_S) ); /* Max Blocking time. */
            
            switch(syncEvents){
                case SYNCHRONOUS:
//                    printf("case SYNCHRONOUS:");
                    syncCounter++;
                    //Para cada mediciï¿½n sincrï¿½nica debe definirse un acumulador
                    //acum_sensor_n += functionToGetSampleSensorN();
                    potentiometer += ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
                    temperature += ADC_Read10bit( ADC_CHANNEL_TEMPERATURE_SENSOR );
                    break;
                default:
                    //indicar con algun codigo de error que algo anduvo mal
                    debug("syncEvents default");
                    break;
            }
            break;
        case SAVE_AND_PACKAGE:
            debug("SAVE_AND_PACKAGE");
            //Preparar la muestra
/////////////////////MUESTRAS TOMADAS SINCRONICAMENTE///////////////////////////
            if(syncCounter>0){
                sample.clima.hum = potentiometer/syncCounter;
                sample.clima.temper = getTemperature(temperature/syncCounter);
            }
            else{
                //to-do: Ver que se hace en caso de que no se hayan tomado 
                //muestras sincronicas. Reinicio?
                debug("syncEvents syncCounter<0");
            }
            syncCounter = 0;
/////////////////////MUESTRAS TOMADAS ASINCRONICAMENTE//////////////////////////
            //to-do: Hacer una funcion que me permita escalar esto mejor
            sample.clima.lluvia = getAccumulatedRain();
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
            /* Ver si puedo obtener el semaforo. Si este no esta disponible
             * esperar xTimeMemoryMutex y volver a probar */
            uint8 resultPutSample = 0;
            char resultGetSample = 0;
            muestra_t returnedSample;
            if( xSemaphoreTake( xMutexMemory, xTimeMemoryMutex ) == pdTRUE )
            {
                resultPutSample = putSample(&sample);
                if(resultPutSample){
                    debug("Muestra guardada exitosamente.");
                }
                else{
                    debug("ERROR al guardar la muestra.");
                }
                resultGetSample = getSample(&returnedSample,0);
                
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
                /*to-do: Reinicio?*/
                debug("ERROR no se pudo tomar el mutex de memoria.");
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

//Funciï¿½n de transferencia del sensor TC1047A embebido en Explorer16/32
uint16_t getTemperature(uint16_t adcValue){
    // VOUT = (10 mV/ï¿½C) (Temperature ï¿½C) + 500 mV
    // 3300mV/1024*ADC=(10mV/ï¿½C)*(Tï¿½C)+500mV 
    uint16_t temp = (adcValue*165/512)-50;
    return temp;
}

uint16_t getAccumulatedRain(){
    /*Se estï¿½ usando el contador provisto por el driver de TMR3 ya que no se 
     pudo configurar el TMR3 para que sea un contador de eventos asincronicos
     como se pretendï¿½a*/
    return TMR3_SoftwareCounterGet();
}

void clearAccumulatedRain(){
    TMR3_SoftwareCounterClear();
}
