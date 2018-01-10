/**
 * @file                shell.c
 * @original-file		CmdLineInterpreter.c
 *
 * @brief
 * Archivo principal del interpretador de linea de comandos para el control del equipo.
 *
 * ---------------------------------------------------------------------------------------------
 *
 * @date		29 de noviembre de 2013, 14:14
 *
 * ---------------------------------------------------------------------------------------------
 *
 * @copyright 	Omixom Ingenieria Electronica
 *
 * ---------------------------------------------------------------------------------------------
 *
 * @author		Juan Cruz Becerra
 * @edition     Martin Gleria
 *
 * ---------------------------------------------------------------------------------------------
 *
 * @version		1.01.00
 * 
 * @modified    18 de septiembre de 2017, 16:31
 * 
 */

/**********************************************************************************************/
/*	standard includes */
#include    "typedef.h"
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

/**********************************************************************************************/
/*	application includes */
#include	"funciones/shell.h"
#include    "sistema/ext_rtcc.h"
//#include	"funciones/memory.h"
//#include	"procesos/cmd_process.h"
//#include	"funciones/puts.usb.h"
//
//#include    "drivers/at_cmds.h"
//#include    "procesos/sub_procesos/conexion.h"
//#include    "funciones/simINFO.h"

/*	sensors includes	*/
#ifdef	USE_SHT1x
#include	"sensores/shtxx.h"
#endif
#ifdef	USE_HIH6131
#include	"sensores/HIH6131-021-001.h"
#endif
//#include	"sensores/battery.h"
//#include	"sensores/presure.h"
//#include	"sensores/rain.h"
//#include	"sensores/solar.rad.h"
//#include	"sensores/temp.suelo.h"
//#include	"sensores/wind.h"
//#include    "sensores/level.h"

/**********************************************************************************************/
/*	Defines	*/
//#define CANTIDAD_COMANDOS 37

/**********************************************************************************************/
/*	external global variables	*/
extern configDevice_t configDevice;
extern muestra_t muestra;

/**********************************************************************************************/
/*	internal global variables	*/
									/*	Comandos de tipo parámetro	*/
string	_commands_[][10]	=	{	"--",				///<	instruccion de linea
									"--model",			///<	query the model device
									"--version",
									"--type",
									"--serial",
									"--dns",
									"--port",
									"--prs",
									"--pws",
									"--pts",
									"--date",
									"--apn",
									"--vpn",
									"--sens",
									/*	Comandos de tipo sensores	*/
									"--temp",			///<	temperatura
									"--hum",			///<	humedad
									"--press",			///<	presion atm.
									"--wdir",			///<	direccion viento
									"--wspd",			///<	velocidad viento
									"--wsmax",
									"--wsmin",
									"--rain",			///<	lluvia
									"--srad",			///<	radiacion solar
									"--tsoil",			///<	temp. suelo
									"--batt",			///<	bateria
                                    "--nivel",          ///<    nivel
									/*	comandos de consulta modem	*/
									"--ip",             ///<	consulta ip del modem
									"--fw",             ///<	consulta firmware modem
									"--imei",			///<	consulta imei del modem
									"--imsi",			///<	consulta imsi del modem
									"--sgnl",			///<	consulta se?al modem
                                    "--mno",            ///<    consulta la operadora activa
									/*	comandos para obtension de muestras	*/
									"--samp",
									/*	Comandos de control avanzado	*/
									"--wipe",			///<	limpieza de fabrica
									"--reset",			///<	reiniciar equipo
									/*	Comandos de ayuda	*/
									"--help",			///<	ayuda lista comandos
/*Comandos experimentales para el monitoreo de SMS*/
                                    "--smsg",           ///<    env?a un mensaje al numner0 +54 9 3516193580
									"\0"				///<	commands ends
								};

/**********************************************************************************************/
/*	Agregar aqui los comandos en forma numerica. Respetar orden de los textos previos */
enum
{
	__model__ = 1,		///!
	__version__,		///!
	__type__,			///!
	__serial__,			///!
	__dns__,			///!
	__port__,			///!
	__prs__,			///!
	__pws__,			///!
	__pts__,			///!
	__date__,			///!
	__apn__,			///!
	__vpn__,			///!
	__sens__,			///!
	__temp__,			///!
	__hum__,			///!
	__press__,			///!
	__wdir__,			///!
	__wspd__,			///!
	__wsmax__,
	__wsmin__,
	__rain__,			///!
	__srad__,			///!
	__tsoil__,			///!
	__batt__,			///!
    __nivel__,          ///!
	__mip__,
	__mfv__,
	__mimei__,
	__mimsi__,
	__msgnl__,
    __mno__,
	__samp__,
	__wipe__,			///!
	__reset__,			///!
	__help__,			///!
    __smsg__        
}__command__;

/**********************************************************************************************/
/*	local global variables	*/
uint16	_start_memory_address_;				///<
uint16	_stop_memory_address_;				///<
extern string	model[];
extern string	version[];

/**********************************************************************************************/
/*	local function prototypes	*/
uint8	strlength( char *s );
uint32	atox( char *x );

/**********************************************************************************************/
/**
 * \brief
 * Funcion para interpretar y ejecutar la linea de comando recibida.\n
 *
 * @param cmdLine	linea de comando
 * @return			puntero a la respuesta o nulo en caso de no haber respuesta.
 */

//string	*processCmdLine( char * cmdLine )
//{
//    uint8   index = 0, commandPosition = 0, total=0;
//	static char response[MAX_RESP_LENGHT];
////	char bufferAux[65];
////	char* p = NULL;
////	rtcc_t t;
//    char *ret = NULL;
//	response[0] = NULL;
//    if(cmdLine == NULL){
//        total = sprintf( response, "%s No es un comando válido (NULL)\n",cmdLine );      
//        return	(string*)response;
//    }
//    
//    	/*	primero busca si la linea contiene algun comando */
//	for(index=0;index<CANTIDAD_COMANDOS;index++){
//        ret = strstr((string*)&_commands_[index],cmdLine);
//        if(ret != NULL){ //Si es cierto, significa que encontró el comando
//            commandPosition=index;
//            break;
//        }
//    }   
//    if(ret==NULL){
//        total = sprintf( response, "%s No es un comando válido\n",cmdLine );      
//        return	(string*)response;
//    }
//    else{
//        total = sprintf( response, "Comando encontrado: %s. Posición: %d\n",ret,commandPosition );
//        return	(string*)response;
//    }
//}

string	*processCmdLine( char * cmdLine )
{
	uint8	index = 0, n;
	static char response[MAX_RESP_LENGHT];
	char bufferAux[65];
	char* p = NULL;
	auto rtcc_t t;

	response[0] = NULL;

	/*	primero busca si la linea contiene algun comando (--)*/
	if( strstr( cmdLine, (string*)&_commands_[index++][0] ) )
	{
		/*	mientras se encuentren instrucciones ... */
		do
		{
			if( p = strstr( cmdLine, (string*)&_commands_[index][0] ) )
			{
				/*	index contiene el numero de instruccion a ejecutar */
                //action apunta al tipo de argumento, '?' ó '='
				char *action = p + strlen( (string*)&_commands_[index][0] );
                //data apunta a los argumentos
				char *data = action + 1;
				/*	busca la cantidad de caracteres del comando	*/
				auto uint8 dataLength = strlength( data );
				auto char *apnptr = NULL;
				auto uint16 apn_vpn_address = 0;

				__command__ = index;
				switch( __command__ )
				{
					case( __model__ ):
						/*	responde con lo solicitado	*/
                        switch(*action){
                            case('?'):
                                sprintf(bufferAux, "%s:%s\n", (string*)&_commands_[index][0], model );
                                strcat( response, (string*)bufferAux );
                                break;
                            default:
                                sprintf( response, "La acción '%s' no es válida.\n",(string*)&_commands_[index][0]); 
                                break;
                        }        
						break;

					case( __version__ ):
						/*	responde con lo solicitado	*/
                        switch(*action){
                            case('?'):
                                sprintf(bufferAux, "%s:%s\n", (string*)&_commands_[index][0], version );
                                strcat( response, (string*)bufferAux );
                                break;
                            default:
                                sprintf( response, "La acción '%s' no es válida.\n",(string*)&_commands_[index][0]); 
                                break;
                        }
						break;

					case( __type__ ):
						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength == 1) )
//						{
//							configDevice.type = (uint8)atoi( data );
//							i2cbus_write( _24LC512_0, INT_EE_DEVICE_CONFIG, (char*)&configDevice, sizeof (configDevice) );
//							*action = '?';		//	fuerza la respuesta
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							i2cbus_read( _24LC512_0, INT_EE_DEVICE_CONFIG, (char*)&configDevice, sizeof (configDevice) );
//							sprintf( bufferAux, "%s:%u\n", (string*)&_commands_[index][0], configDevice.type );
//							strcat( response, (string*)bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __serial__ ):
						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength < 5) )
//						{
//							configDevice.serial = (uint16)atoi( data );
//							i2cbus_write( _24LC512_0, INT_EE_DEVICE_CONFIG, (char*)&configDevice, sizeof (configDevice) );
//							*action = '?';		//	fuerza la respuesta
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							i2cbus_read( _24LC512_0, INT_EE_DEVICE_CONFIG, (char*)&configDevice, sizeof (configDevice) );
//							sprintf( bufferAux, "%s:%u\n", (string*)&_commands_[index][0], configDevice.serial );
//							strcat( response, (string*)bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __dns__ ):
						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength < 64) )
//						{
//							strncpy( bufferAux, data, dataLength );
//							bufferAux[ dataLength ] = NULL;
//							setServerDNS( bufferAux );
//							*action = '?';		//	fuerza la respuesta
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							sprintf( bufferAux, "%s:%s\n", (string*)&_commands_[index][0], getServerDNS() );
//							strcat( response, (string*)bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __port__ ):
						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength <= 4) )
//						{
//							setServerPort( (uint16)atoi( data ) );
//							*action = '?';		//	fuerza la respuesta
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							sprintf( bufferAux, "%s:%u\n", (string*)&_commands_[index][0], getServerPort() );
//							strcat( response, (string*)bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __prs__ ):
						/*	configura el parametro	*/
						if( (*action == '=') && (dataLength <= 5) )
						{
							setSamplesRead( atoi( data ) );	//	asigna nuevo valor
							*action = '?';		//	fuerza la respuesta
						}
						/*	responde con lo solicitado	*/
						if( *action == '?' )
						{
							sprintf(bufferAux, "%s:%u", &_commands_[index][0], getSamplesRead() );
							strcat( response, (string*)&bufferAux );
							strcat( response, (string*)"\n" );
						}
//                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __pws__ ):
						/*	configura el parametro	*/
						if( (*action == '=') && (dataLength <= 5) )
						{
							setSamplesWrite( atoi( data ) );	//	asigna nuevo valor
							*action = '?';
						}
						/*	responde con lo solicitado	*/
						if( *action == '?' )
						{
							sprintf(bufferAux, "%s:%u", &_commands_[index][0], getSamplesWrite() );
							strcat( response, (string*)&bufferAux );
							strcat( response, (string*)"\n" );
						}
//                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __pts__ ):
						/*	configura el parametro	*/
						if( (*action == '=') && (dataLength <= 5) )
						{
							setSamplesTotal( atoi( data ) );	//	asigna nuevo valor
							*action = '?';		//	fuerza la respuesta
						}
						/*	responde con lo solicitado	*/
						if( *action == '?' )
						{
							sprintf(bufferAux, "%s:%u", &_commands_[index][0], getSamplesTotal() );
							strcat( response, (string*)&bufferAux );
							strcat( response, (string*)"\n" );
						}
//                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __date__ ):
						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength == 12) )
//						{
//							t.hora		=	(data[0]&0xF)<<4 | (data[1]&0xF);
//							t.minutos	=	(data[2]&0xF)<<4 | (data[3]&0xF);
//							t.segundos	=	(data[4]&0xF)<<4 | (data[5]&0xF);
//						//	t.dow		=	0;
//							t.dia		=	(data[6]&0xF)<<4 | (data[7]&0xF);
//							t.mes		=	(data[8]&0xF)<<4 | (data[9]&0xF);
//							t.anio		=	(data[10]&0xF)<<4 | (data[11]&0xF);
//							set_rtcc_datetime(&t);
//							*action = '?';		//	fuerza la respuesta
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							data = &bufferAux;
//							get_rtcc_datetime( &t );
//							auto char *c = &t;
//							data += sprintf( bufferAux, "%s:", (string*)&_commands_[index][0] );
//							auto uint8 b = 0;
//							while( b < sizeof(rtcc_t) )
//								sprintf( data + (2*b++), "%02x", (*c++) );
//							strcat(response, (string*)bufferAux);
//							strcat(response, (string*)"\n");
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __apn__ ):
					case( __vpn__ ):
						/*	busca el numero de APN/VPN a leer/modificar */
//						if( (apnptr = strstr( data, (string*)">" )) == NULL )
//							break;
//						/*	controla que el proximo caracter sea un numero	*/
//						apnptr ++;
//						if( isdigit( (char)*apnptr ) == FALSE )
//							break;
//						/*	coloca un nulo para terminar el comando apn/vpn */
//						*(apnptr+1) = NULL;
//						/*	convierte de ascii a hex y carga la direccion del APN/VPN	*/
//						apn_vpn_address = (uint16)(*apnptr & 0xF);
//						/*	solo se permite hasta el numero 7 inclusive */
//						if( apn_vpn_address > 7 )
//							break;
//						/*	calcula la direccion de apn	*/
//						if( apn_vpn_address < 6 )
//						{
//							apn_vpn_address *= APN_SIZE;
//							apn_vpn_address += INT_EE_APN_START;
//						}
//						/*	calcula la direccion de vpn	*/
//						else
//						{
//							apn_vpn_address &= 1;
//							apn_vpn_address *= APN_SIZE;
//							apn_vpn_address += INT_EE_VPN_START;
//						}
//
//						/*	quita el caracter '>' y el numero de apn a leer/modificar del
//						 * largo de datos	*/
//						dataLength -= 2;
//						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength <= 64) )
//						{
//							/*	copia el apn/vpn */
//							strncpy( bufferAux, data, dataLength );
//							bufferAux[ dataLength ] = NULL;
//							i2cbus_write( _24LC512_0, apn_vpn_address, (char*)&bufferAux, APN_SIZE );
//							*action = '?';
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							sprintf( bufferAux, "%s(%c):", (string*)&_commands_[index][0], *apnptr );
//							strcat( response, (string*)&bufferAux );
//							i2cbus_read( _24LC512_0, apn_vpn_address, (char*)&bufferAux, APN_SIZE );
//							strcat( response, (string*)&bufferAux );
//							strcat( response, (string*)"\n" );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __sens__ ):
//						/*	configura el campo	*/
//						if( (*action == '=') && (dataLength == 8) )
//						{
//							uint32 value = atox( data );		//	convierte ascii a hex
//							i2cbus_write( _24LC512_0, INT_ENABLE_SENSORS_, (char*)&value, dataLength/2 );
//							*action = '?';
//						}
//						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							static uint8 sensors[4];
//							auto char *c = &sensors[3];
//							i2cbus_read( _24LC512_0, INT_ENABLE_SENSORS_, (char*)&sensors, ENABLE_SENSORS_SIZE );
//							data = &bufferAux;
//							data += sprintf( bufferAux, "%s:", (string*)&_commands_[index][0] );
//							auto uint8 b = 0;
//							while( b < ENABLE_SENSORS_SIZE )
//								sprintf( data + (2*b++), "%02X", (*c--) );
//							strcat( response, (string*)&bufferAux );
//							strcat( response, (string*)"\n" );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					#ifdef	USE_SENSOR_TEMP_HUM
					case( __temp__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							uint16 temp = getTemperature(1);
//							sprintf(bufferAux, "%s:%d,%01u(*C)\n", &_commands_[index][0], temp/10, temp%10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __hum__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							auto uint16 hum = getHumidity(1) / 10;
//							sprintf(bufferAux, "%s:%u(%%HR)\n", &_commands_[index][0], hum );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
						#endif
						
					#ifdef	USE_SENSOR_PRESURE
					case( __press__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							sprintf(bufferAux, "%s:%u(hPa)\n", &_commands_[index][0], getPress() );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
						#endif

					#ifdef	USE_SENSOR_WIND
					case( __wdir__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							sprintf(bufferAux, "%s:%s\n", &_commands_[index][0], getCompassDirName() );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __wspd__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							auto uint16 wspeed = GetWindAvgSpeed()*2/10;
//							sprintf(bufferAux, "%s:%u,%01u(Km/h)\n", &_commands_[index][0], wspeed/10, wspeed%10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
					case( __wsmax__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							auto uint16 wspeed = GetWindMaxSpeed()*2/10;
//							sprintf(bufferAux, "%s:%u,%01u(Km/h)\n", &_commands_[index][0], wspeed/10, wspeed%10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
					case( __wsmin__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							auto uint16 wspeed = GetWindMinSpeed()*2/10;
//							sprintf(bufferAux, "%s:%u,%01u(Km/h)\n", &_commands_[index][0], wspeed/10, wspeed%10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
						#endif

					#ifdef	USE_SENSOR_RAIN
					case( __rain__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							uint16 rain_day = 0;
//							read_rtcc_array( RAIN_SAVED_ADDRESS, (char*)&rain_day, sizeof(rain_day) );
//							rain_day += getRain();
//							rain_day *= 2;
//							sprintf(bufferAux, "%s:%u,%01u(mm)\n", &_commands_[index][0], rain_day/10, rain_day%10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
						#endif

					#ifdef	USE_SENSOR_SOLAR_RAD
					case( __srad__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							sprintf(bufferAux, "%s:%u(W/m2)\n", &_commands_[index][0], getSolarRad() );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
						#endif

					#ifdef	USE_SENSOR_SOIL_TEMP
					case( __tsoil__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							auto sint16 tsoil = getSoilTemperature(TRUE);
//							sprintf(bufferAux, "%s:%d(*C)\n", &_commands_[index][0], tsoil/10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
                        #endif

                    #ifdef	USE_SENSOR_LEVEL
					case( __nivel__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							
//							sprintf(bufferAux, "%s:%d(cm)\n", &_commands_[index][0], (levelAVG( getLevel() )>>4) );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
                        
						#endif

					case( __batt__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							auto uint16 batt = getBatt();
//							sprintf( bufferAux, "%s: %u,%01u(V)\n", &_commands_[index][0], batt/10, batt%10 );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;
                    
                    case(__mip__):
                        /*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//						sprintf( bufferAux, "%s: %s\n", &_commands_[index][0], (char*)getIpAddress());
//                        strcat( response, (string*)&bufferAux );	
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;
                        
                    case (__mfv__):
                        /*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//						sprintf( bufferAux, "%s: %s\n", &_commands_[index][0], (char*)fgetModemID());
//                        strcat( response, (string*)&bufferAux );	
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;
                        
                    case (__mimei__):
                        /*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//						sprintf( bufferAux, "%s: %s\n", &_commands_[index][0], (char*)fgetIMEI());
//                        strcat( response, (string*)&bufferAux );	
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;
                    
                    case (__mimsi__):
                        /*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//						sprintf( bufferAux, "%s: %s\n", &_commands_[index][0], (char*)fgetIMSI());
//                        strcat( response, (string*)&bufferAux );	
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;
                    
                    case (__msgnl__):
                        /*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//						sprintf( bufferAux, "%s: %u\n", &_commands_[index][0], getSignalLevel());
//                        strcat( response, (string*)&bufferAux );	
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;
                    
                    case(__mno__):
                        /*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//						sprintf( bufferAux, "%s: %s\n", &_commands_[index][0], (char*) getAPN());
//                        strcat( response, (string*)&bufferAux );	
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;

					case( __samp__ ):
						/*	responde con lo solicitado	*/
//						if( *action == '?' )
//						{
//							uint16 total;
//							//	consulta si hay alguna muestra almacenada
//							if( total = getSamplesTotal() )
//							{
//								/* Se obtiene cantidad de muestras a enviar y puntero de
//								 * muestra inicial.
//								 * Se envia de muestra mas vieja a mas reciente.
//								 */
//								usbSendSamples = total;
//								usbSendSamplesPtr = getSamplesRead();
//							}
//							//	devuelve la cantidad de muestras almacenadas
//							sprintf(bufferAux, "%s: %04u\n|LOG|Stored: %u|end|", &_commands_[index][0], total, total );
//							strcat( response, (string*)&bufferAux );
//						}
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
						break;

					case( __wipe__):
//						strcat(response, (string*)"_wipe_ok_");
//						i2cbus_read( _24LC512_0, INT_EE_DEVICE_CONFIG, (char*)&configDevice, sizeof(configDevice) );
//						configDevice.factoryReset = 0xFF;
//						i2cbus_write( _24LC512_0, INT_EE_DEVICE_CONFIG, (char*)&configDevice, sizeof(configDevice) );

					case( __reset__):
//                      strcat(response, (string*)"Reset in progress");
//						putsLog("Reset device");
//						//__delay_ms(10);
//						//asm("reset");
						break;

					case( __help__ ):
						n = 1;
						strcat( response, "--help:" );
						while( _commands_[n][0] )
						{
							strcat( response, (string*)&_commands_[n++][0] );
							strcat( response, (string*)", " );
						}
						strcat( response, (string*)"\n" );
						break;  
                    
                        
                        
                    case (__smsg__):
//                        if( (*action == '=') && (dataLength == 10)) 
//                        {
//                            sprintf( response, atcmd_sendMsg, (char*)data );
//                            /*	envia el numero destinatario del SMS	*/
//                            SendATCommand( (string*)response, bufferAux,bufferAux,1, 0, 0 );
//                            /*	agrega la respuesta	*/
//                            strcpy( response, (string*)"3G_test");
//                            /*	agrega caracter <ctrl-z>	*/
//                            strcat( response, (string*)"\x001A" );
//                            /*	envia el comando AT	*/
//                            SendATCommand( (string*)response, response,bufferAux,10, 20, 3 );
//                        }
                        sprintf( response, "Comando encontrado: %s. NO IMPLEMENTADO\n",(string*)&_commands_[index][0]);
                        break;

                        
                        
					default: 
						sprintf( bufferAux, "Cmd %s NO IMPLEMENTADO", (string*)&_commands_[index][0] );
						strcat( response, (string*)&bufferAux );
						strcat( response, (string*)"\n" );
						break;
				}
			}
		}while( (char*)_commands_[++index][0] );
	}
	if(response[0] == NULL){
        sprintf( response, "Comando NO encontrado: %s. Obtenga ayuda con --help.\n",cmdLine);
    }
    return	(string*)&response;
}


/**********************************************************************************************/
/**
 * \brief
 * Busca la cantidad de caracteres hasta el caracter espacio o hasta terminar la trama.
 * 
 * @param s	texto
 * @return	cantidad de caracteres
 */
uint8	strlength( char *s )
{
	uint8	length = 0;
	//	mientras sean caracteres permitidos
	while( (32<*s) && (*s<128) )
	{
		length ++;	s ++;
	}

	return	length;
}

/**********************************************************************************************/
/**
 * \brief
 * 
 * @param	x	puntero al texto
 * @return	hasta 8 digitos ascii pasados a hex comprimidos en 4 bytes (uint32).
 */
uint32	atox( char *x )
{
	/*	calcula la cantidad de numeros hexa a convertir	*/
	uint8 n = strlength( x );
	/*	borra el valor donde se almacenara el resultado	*/
	uint32 d = 0;
	/*	convierte los valores	*/
	while( isxdigit( *x ) && n && (n<=8) )
	{
		/*	si el valor es A a la F	*/
		if( !isdigit( *x ) )
			*x -= 7;	//	para obtener los valores de 10 a 15
		d |= ((uint32)(*x++ & 0xF) << ((--n)*4) );

	}
	return	d;
}
