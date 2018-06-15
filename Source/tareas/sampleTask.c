#include <stdio.h>

#include    "tareas/sampleTask.h"
#include    "funciones/memory.h"
#include    "utilities.h"
#include    "tmr3.h"

#include    "FreeRTOS.h"
#include    "freeRTOS/semphr.h"


static void prvPasiveCallback (TimerHandle_t xTimer);
static void prvActiveCallback (TimerHandle_t xTimer);
static void prvDataCallback (TimerHandle_t xTimer);
static void prvDebouncingRainCallback (TimerHandle_t xTimer);

static void FSM_SampleTask(uint32_t status);
static void  sensorsConfig();
static uint16_t getAccumulatedRain();
static void clearAccumulatedRain();
static void setStatusFSM(uint32_t nextStatus);
static void resetSyncVariables();
static uint16_t getTemperature(uint16_t adcValue);

//Handlers software timers
TimerHandle_t xPassiveSamplingTime;
TimerHandle_t xActiveSamplingTime;
TimerHandle_t xSampleData;
TimerHandle_t xRainDebouncing;

//Tiempos usados en software timers
TickType_t  xTimePasive, xTimeActive, xTimeData, xTimeRainDebouncing, xTimeMemoryMutex;

TaskHandle_t xSampleHandle;

static muestra_t sample;

static uint32_t status;
static bool waitForNotify;
static uint16_t syncCounter;

static uint16_t potentiometer;
static uint16_t temperature;

SemaphoreHandle_t xMutexMemory = NULL;

void debug_enable()
{
    __C30_UART=1;
}

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
    
//    debug_enable();
}

void vTaskSample( void *pvParameters ){
    UBaseType_t uxHighWaterMarkSample;
    
// Seccion de inicializacion
    waitForNotify = false;
    
    xMutexMemory = xSemaphoreCreateMutex();
    if(!xMutexMemory){
        printf("ERROR en la creación del mutex de Memoria");
        //El programa no puede seguir, hay que detenerlo.
    }
    
    setStatusFSM(SYNC_SERVER_TIME); //Descomentar cuando se implemente el estado SYNC_SERVER_TIME
//    setStatusFSM(ASYNC_SAMPLING);
        
    // Cuerpo de la tarea
    for( ;; ){
        
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
    
    /*Los cambios de estado ocurren en los callback de los timers o en otras
     funciones externas*/
    switch(status){  
        case SYNC_SERVER_TIME:
            debugUART1("SYNC_SERVER_TIME\r\n");
//            printf("SYNC_SERVER_TIME");
            //Code to sync RTCC with server

            setStatusFSM(ASYNC_SAMPLING);
            //Cuando este softTimer expire, se cambia de estado a SYNC_SAMPLING
            xTimerStart(xPassiveSamplingTime,0);
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
            printf("SYNC_SAMPLING %d\r\n",syncCounter);
//            debugUART1("SYNC_SAMPLING\r\n");
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
                sample.clima.luzDia = potentiometer/syncCounter;
                sample.clima.temper = getTemperature(temperature/syncCounter);
                printf("sample.clima.luzDia: %d\r\n",sample.clima.luzDia);
                printf("sample.clima.temper: %d\r\n",sample.clima.temper);
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
            printf("sample.clima.lluvia: %d\r\n",sample.clima.lluvia);
            //Limpio el contador por soft del TMR3
            clearAccumulatedRain();
            
/////////////////////GUARDANDO MUESTRA EN MEM PERSISENTE////////////////////////
            /* Ver si puedo obtener el semaforo. Si este no está disponible
             * esperar xTimeMemoryMutex y volver a probar */
            if( xSemaphoreTake( xMutexMemory, xTimeMemoryMutex ) == pdTRUE )
            {
                if(putSample(&sample)){
                    setSamplesRead(55);
                    printf("getSamplesRead()->%d\r\n",getSamplesRead());
                    printf("getSamplesWrite()->%d\r\n",getSamplesWrite());
                    printf("Muestra guardada exitosamente.\r\n");
                }
                else{
                    printf("ERROR al guardar la muestra.\r\n");
                }
                printf("getSample(&sample,0)-> \r\n",getSample(&sample,0));
                printf("sample.clima.luzDia: %d\r\n",sample.clima.luzDia);
                printf("sample.clima.temper: %d\r\n",sample.clima.temper);
                printf("sample.clima.lluvia: %d\r\n",sample.clima.lluvia);
                
                /* Terminamos de usar el recurso, por lo que devolvemos el
                 * mutex */
                xSemaphoreGive( xMutexMemory );
                setStatusFSM(ASYNC_SAMPLING);
                break;
            }
            else{
                printf("ERROR no se pudo tomar el mutex de memoria.\r\n");
            }
    }
}

static uint16_t getAccumulatedRain(){
    /*Se está usando el contador provisto por el driver de TMR3 ya que no se 
     pudo configurar el TMR3 para que sea un contador de eventos asincronicos
     como se pretendía*/
    return TMR3_SoftwareCounterGet();
}

static void clearAccumulatedRain(){
    TMR3_SoftwareCounterClear();
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
    
    vLedToggleLED(0);
    xTimerStop(xPassiveSamplingTime,0);
    xTimerStart(xActiveSamplingTime,0);
    xTimerStart(xSampleData,0);
//    xTaskNotify(xSampleHandle,SENSOR_1,eSetValueWithOverwrite);
    
    setStatusFSM(SYNC_SAMPLING);
    resetSyncVariables();
}

static void prvActiveCallback (TimerHandle_t xTimer){
    vLedToggleLED(1);
    
    xTimerStop(xSampleData,0);
    xTimerStop(xActiveSamplingTime,0);
    xTimerStart(xPassiveSamplingTime,0);
    
    setStatusFSM(SAVE_AND_PACKAGE);
    
//    xTaskNotify(xSampleHandle,SENSOR_2,eSetValueWithOverwrite);
}

static void prvDataCallback (TimerHandle_t xTimer){
    vLedToggleLED(2);
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
    muestra->nullE=NULL; 
//    strcpy(muestra->nullE,"\x3A");
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
    
    muestra->clima.luzDia = ADC_Read10bit( ADC_CHANNEL_POTENTIOMETER );
   
}

void prepareSample(trama_muestra_t *tramaMuestra, muestra_t *muestraAlmacenada)
{
    //	copia los datos a la trama - estacion
    tramaMuestra->cmd = muestraAlmacenada->cmd;
    tramaMuestra->tipo = muestraAlmacenada->tipo;  //estacion.tipo; //configDevice.type = 0;
    tramaMuestra->num_serie = muestraAlmacenada->num_serie; //swapBytes( estacion.num_serie ); //configDevice.serial = 0;
    tramaMuestra->hora = bcd2dec( muestraAlmacenada->hora );
    tramaMuestra->min = bcd2dec( muestraAlmacenada->minutos );
    tramaMuestra->dia = bcd2dec( muestraAlmacenada->dia );
    tramaMuestra->mes = bcd2dec( muestraAlmacenada->mes );
    tramaMuestra->anio = bcd2dec( muestraAlmacenada->anio );
    tramaMuestra->signal = muestraAlmacenada->senial;
//    #ifdef	USE_HIH6131
    #if !defined	USE_HIH6131
    tramaMuestra->clima.temper = muestraAlmacenada->clima.temper | 0x8000;
    tramaMuestra->clima.hum = muestraAlmacenada->clima.hum | 0x8000;
    #else
    tramaMuestra->clima.temper = muestraAlmacenada->clima.temper;
    tramaMuestra->clima.hum = muestraAlmacenada->clima.hum;
    #endif
    tramaMuestra->clima.presion = muestraAlmacenada->clima.presion;
    tramaMuestra->clima.lluvia = muestraAlmacenada->clima.lluvia;
    tramaMuestra->clima.radiacionSolar = muestraAlmacenada->clima.radiacionSolar;
    //	el modelo de pluviometro chino se guarda con la mascara->
//    #if	defined	(__OMX_S_C__)
//    tramaMuestra->clima.lluvia |= 0x4000;		
//    #endif
    #if	defined	(__OMX_T__)
    tramaMuestra->clima.radiacionSolar |= 0x8000;
    tramaMuestra->clima.lluvia |= 0x8000;
    #endif
    #ifdef	USE_RK200
    tramaMuestra->clima.radiacionSolar |= 0x4000;
    #endif
    #if	defined	(__OMX_S__)	||	defined	(__OMX_S_C__) ||  defined	(__OMX_T__)
    tramaMuestra->clima.viento.direccionP = muestraAlmacenada->clima.viento.direccionP;
    tramaMuestra->clima.viento.velocidadP = muestraAlmacenada->clima.viento.velocidadP;
    tramaMuestra->clima.viento.direccionM = muestraAlmacenada->clima.viento.direccionM;
    tramaMuestra->clima.viento.velocidadM = muestraAlmacenada->clima.viento.velocidadM;
    tramaMuestra->clima.luzDia = muestraAlmacenada->clima.luzDia;
    tramaMuestra->clima.tempSuelo1 = muestraAlmacenada->clima.tempSuelo1;
    tramaMuestra->clima.humSuelo1 = muestraAlmacenada->clima.humSuelo1;
//    tramaMuestra->clima.tempSuelo2 = muestraAlmacenada->clima.tempSuelo2 | 0x4000;
    tramaMuestra->clima.tempSuelo2 = muestraAlmacenada->clima.tempSuelo2;
    tramaMuestra->clima.humSuelo2 = muestraAlmacenada->clima.humSuelo2;
    tramaMuestra->clima.tempSuelo3 = muestraAlmacenada->clima.tempSuelo3;
    tramaMuestra->clima.humSuelo3 = muestraAlmacenada->clima.humSuelo3;
    tramaMuestra->clima.humHoja = muestraAlmacenada->clima.humHoja;
    tramaMuestra->clima.tempHoja = muestraAlmacenada->clima.tempHoja;
    #elif   defined (__OMX_N__)
    //tramaMuestra->clima.luzDia = muestraAlmacenada->clima.luzDia;
    //tramaMuestra->clima.radiacionSolar = muestraAlmacenada->clima.radiacionSolar;
    tramaMuestra->clima.tempSuelo1 = muestraAlmacenada->clima.tempSuelo1;
    tramaMuestra->clima.humSuelo1 = muestraAlmacenada->clima.humSuelo1;
    tramaMuestra->clima.tempSuelo2 = muestraAlmacenada->clima.tempSuelo2;
    tramaMuestra->clima.humSuelo2 = muestraAlmacenada->clima.humSuelo2;
    tramaMuestra->clima.nivel = muestraAlmacenada->clima.nivel;               //Muestra nivel
    tramaMuestra->clima.humSuelo3 = muestraAlmacenada->clima.humSuelo3;
    tramaMuestra->clima.humHoja = muestraAlmacenada->clima.humHoja;
    tramaMuestra->clima.tempHoja = muestraAlmacenada->clima.tempHoja;
    #endif

    #if	defined	(__OMX_T__)
    tramaMuestra->clima.tempSuelo2 += 0x4000;
    #endif

    #if	defined	(__OMX_S__) 
    tramaMuestra->corriente1 = muestraAlmacenada->corriente1;
    tramaMuestra->corriente2 = muestraAlmacenada->corriente2;
    #endif
    tramaMuestra->bateria = muestraAlmacenada->bateria;
    tramaMuestra->periodo = muestraAlmacenada->periodo;
    tramaMuestra->sensorHab1 = muestraAlmacenada->sensorHab1;
    tramaMuestra->sensorHab2 = muestraAlmacenada->sensorHab2;
    tramaMuestra->sensorHab3 = muestraAlmacenada->sensorHab3;
    tramaMuestra->nullE = muestraAlmacenada->nullE;
}

uint8_t prepareSampleToSend(trama_muestra_t *tramaMuestra, char *tramaGPRS)
{
//    printf("prepareSampleToSend()\r\n");
    uint8_t n,k;
    
    char *p = NULL; char *t = NULL;
//    const char string_cabecera[] = "";
//    const char string_cierre[] = "\x001A";

//    char *t = tramaGPRS + strlen((char*)&string_cabecera);
    
    
    
    // Armo el buffer a transmitir: concatenado de cadenas cabecera, datos y cierre.
    // @todo quitar el caracter null de fin de trama ya que se controla la cantidad con sizeof
//    strncpy( (char*)&tramaGPRS, (char*)&string_cabecera, strlen((char*)&string_cabecera) );

    n = 0;
	t = tramaGPRS;
    p = (char*)tramaMuestra;
    
    
//    printf("sizeof(trama_muestra_t): %d\r\n",sizeof(trama_muestra_t));
    
    while( n < sizeof(trama_muestra_t)-2 ){ //No agarra el byte nullE
        sprintf( (char*)t + (2*n), (const char*)"%02X", *(p+n));
        n++;
//        printf("n:%d | (char*)t+(2*n):%s\r\n",n,(char*)t + (2*n));
    }
    
//    printf("n*2:%d | (char*)t+(2*(n-1)):%s\r\n",n*2,(char*)t+(2*(n-1)));
    
//No es necesario el caracter de fin de trama porque hay el estado 'putData' lo hace
//    strncpy( tramaGPRS + n*2, string_cierre, strlen(string_cierre) );
    
    
    //Para eliminar el byte de padding generado al comienzo del struct viento
    for(k=42;k<116;k++){
        tramaGPRS[k]=tramaGPRS[k+2];
    }

    return true;        
}

static void resetSyncVariables(){
    syncCounter = 0;
    potentiometer = 0;
    temperature = 0;
}

//Función de transferencia del sensor TC1047A embebido en Explorer16/32
static uint16_t getTemperature(uint16_t adcValue){
    // VOUT = (10 mV/°C) (Temperature °C) + 500 mV
    // 3300mV/1024*ADC=(10mV/ºC)*(TºC)+500mV 
    uint16_t temp = (adcValue*165/512)-50;
    return temp;
}
