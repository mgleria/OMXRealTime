/* 
 * File:   typedef.h
 * Author: Tincho
 *
 * Created on 20 de septiembre de 2017, 18:43
 */

#ifndef TYPEDEF_H
#define	TYPEDEF_H


//#include "HardwareProfile.h"


/*	unsigned variables */
typedef	unsigned char uint8;			///<	8-bit unsigned variable
typedef	unsigned short int uint16;		///<	16-bit unsigned variable
typedef	unsigned long uint32;			///<	32-bit unsigned variable

/*	signed variables */
typedef	signed char sint8;				///<	8-bit signed variable
typedef	signed short int sint16;		///<	16-bit signed variable
typedef signed long sint32;				///<	32-bit signed variable


/**********************************************************************************************/
/**	\typedef viento_t
 * Wind variable type definition
 */
#if	defined	(__OMX_S__)	||	defined	(__OMX_S_C__) || defined	(__OMX_T__)
typedef struct 
{
	uint8				direccionP;		//	20
    uint16				velocidadP;		//	21,22
	uint16				direccionM;		//	23,24
	uint16				velocidadM;		//	25,26
}viento_t;								//	7 bytes

#else
	#warning	The model does not include WIND variable
	#endif
/**********************************************************************************************/
/*
 * Weather variable definition  - sensors
 */
typedef struct 
{    
	uint16				temper;			//	10,11
	uint16				hum;			//	12,13
	float				presion;		//	14 a 17
	uint16				lluvia;			//	18,19	
	#if	defined	(__OMX_S__)	||	defined	(__OMX_S_C__) || defined	(__OMX_T__)
	viento_t			viento;			//	de 20 a 26
    #elif defined (__OMX_N__)
    uint8               reserved_1;      //	20      reservado
    uint16              reserved_2;      //  21,22
    uint16              reserved_3;      //  23,24
    uint16              reserved_4;      //  25,26
    #endif
    uint16				luzDia;			//	27,28	add	fase minima
	uint16				radiacionSolar;	//	29,30	add fase maxima
	uint16				tempSuelo1;		//	31,32	add	corriente minima
	uint16				humSuelo1;		//	33,34	add corriente maxima
	uint16				tempSuelo2;		//	35,36	***	corriente
	uint16				humSuelo2;		//	37,38	***	tension pack baterias
	#if	defined	(__OMX_S__)	||	defined	(__OMX_S_C__) || defined	(__OMX_T__)
    uint16				tempSuelo3;		//	39,40
    #elif defined (__OMX_N__)
    uint16				nivel;          //	39,40 
    #endif
	uint16				humSuelo3;		//	41,42	***	fase 1
	uint16				humHoja;		//	43,44	***	fase 2
	uint16				tempHoja;		//	45,46	***	fase 3
}clima_t;								//	37 bytes

/**********************************************************************************************/
/*
 * Largo de la muestra: 56 bytes
 * Total de muestras en memoria de 64KB / 64 = 1008 muestras maximo
 * Autonomia c/muestras c/10minutos = 7 dias
 */
typedef struct
{
	uint8			cmd;				//	00
	uint8			tipo;				//	01
	uint16			num_serie;			//	02,03
	uint8			hora;				//	04
	uint8			minutos;			//	05
	uint8			dia;				//	06
	uint8			mes;				//	07
	uint8			anio;				//	08
	uint8			senial; 			//	09
	clima_t			clima;				//	10 a 46 para modelo OMX_S / OMX_L
	uint16			corriente1;			//	47,48
	uint16			corriente2;			//	49,50
	uint16			bateria;			//	51,52
	uint8			periodo;			//	53
	uint8			sensorHab1;			//	54
	uint8			sensorHab2;			//	55
	uint8			sensorHab3;			//	56
	char			nullE;				//	57
	char			_reserved_[6];		//	reserved to complete 64 bytes
}muestra_t;								//	64 bytes

/**********************************************************************************************/
/*
 * definicion de variable tipo estacion
 */
typedef struct
{
	uint8				cmd;			//	0
	uint8				tipo;			//	1
	uint16				num_serie;		//	2,3
	uint8				hora;			//	4
	uint8				min;			//	5
	uint8				dia;			//	6
	uint8				mes;			//	7
	uint8				anio;			//	8
	uint8				signal;			//	9
	uint16				battLevel;		//	10,11
	uint16				pEscritura;		//	12,13
	uint16				pLectura;		//	14,15
	uint16				dAlmacenados;	//	16,17
	uint8				InicioMuestras;	//	18
	sint8				tiporeset;		//	19
	uint16				intentosConex;	//	20,21
	uint16				fw_version;		//	22,23
}estacion_t;							//	tamaño: 24 caracteres

/**********************************************************************************************/
/*
 * definicion de variable tipo modem
 */
typedef struct
{

	char				imei[8];		//	24 a 31
	char				imsi[8];		//	32 a 39
	char				fsn[16];		//	40 a 55
	char				fw_version[16];	//	56 a 71
	uint8				ipAddress[4];	//	72 a 75
}modem_t;								//	tamaño: 52 caracteres
/**********************************************************************************************/
/*
 * definicion de variable tipo trama muestra
 */
typedef struct 
{
	uint8       cmd         ;		//	0
	uint8		tipo        ;		//	1
	uint16		num_serie;                                  //	2,3
	uint8		hora        ;		//	4
	uint8		min         ;		//	5
	uint8		dia         ;		//	6
	uint8		mes         ;		//	7
	uint8		anio        ;		//	8
	uint8		signal      ;		//	9
	clima_t		clima       ;		//	10 a 46 para modelo OMX_S / OMX_L
	uint16		corriente1;                                 //	47,48
	uint16		corriente2;                                 //	49,50
	uint16		bateria;                                    //	51,52
	uint8		periodo     ;		//	53
	uint8		sensorHab1  ;		//	54
	uint8		sensorHab2  ;		//	55
	uint8		sensorHab3  ;		//	56
	char		nullE       ;		//	57 para indicar fin de trama
}trama_muestra_t;                                           //	57 caracteres en total

/**********************************************************************************************/
/*
 * Estructura de registro del equipo en la base de datos
 */
typedef struct
{
	uint8				cmd;			//	0
	uint8				tipo;			//	1
	uint16				num_serie;		//	2,3
	uint8				dia;			//	4
	uint8				mes;			//	5
	uint8				anio;			//	6
	uint8				dow;			//	7
	uint8				hora;			//	8
	uint8				min;			//	9
	uint8				alarmaMuestras;	//	10
	uint8				tiempoMedicion;	//	11
	uint8				inicioMuestras;	//	12
	uint16				pEscritura;		//	13,14
	uint16				pLectura;		//	15,16
	uint16				dAlmacenados;	//	17,18
	uint8				pMaxMin;		//	19
	uint8				periodo;		//	20
	uint8				sensorHab1;		//	21
	uint8				sensorHab2;		//	22
	uint8				sensorHab3;		//	23
	char				nullE;			//	para indicar fin de trama
}trama_inicio_t;						//	tamaño: 24 caracteres

/**********************************************************************************************/
/*
 * Estructura de la trama de configuracion del equipo.
 */
typedef struct
{
	uint8				cmd;			//	0
	uint8				tipo;			//	1
	uint16				num_serie;		//	2,3
	uint16				reservado1;		//	4,5
	uint16				reservado2;		//	6,7
	uint16				capMemoria;		//	8,9
	uint8				tiempoProm;		//	10
	uint8				periodo;		//	11
	uint8				sensorHab1;		//	12
	uint8				sensorHab2;		//	13
	uint8				sensorHab3;		//	14
	char				nullE;			//	para indicar fin de trama
}trama_config_t;						//	tamaño: 15 caracteres

/**********************************************************************************************/
/*
 * variable para informacion del servidor.
 */
typedef struct
{
	uint16				port;			//	puerto de conexion al servidor
	char				dns[30];		//	direccion de conexion con el servidor
}server_t;								//	32 bytes de tamaño de variable

/*	estructura de configuracion del equipo */
typedef struct
{
	uint8	type;
	uint16	serial;
	uint16	fw_version;
	uint8	factoryReset;
}configDevice_t;

/**********************************************************************************************/
//Existe el tipo de datos bool. Usar 'true' y 'false' en vez de 'TRUE' Y 'FALSE'
//#if	!defined	(__XC)	&&	!defined	(HI_TECH_C)
//typedef enum
//{
//	FALSE = 0,
//	TRUE
//}bool;
//#endif

/**********************************************************************************************/
typedef const char string;

/**********************************************************************************************/
typedef enum
{
	NOACK,
	ACK
}acknowledge;

//Para función delay
#define FCY 8000000UL
#include <libpic30.h>

#endif	/* TYPEDEF_H */

