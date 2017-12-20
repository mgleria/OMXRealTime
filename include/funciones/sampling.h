/* 
 * File:   sampling.h
 * Author: Tincho
 *
 * Created on 12 de diciembre de 2017, 17:17
 */

#ifndef SAMPLING_H
#define	SAMPLING_H

typedef enum
{
    SENSOR_1,
    SENSOR_2,
    SENSOR_3,
    CLOSE_SAMPLE        
} SAMPLING_STATUS;

uint16_t getData_Sensor_1();
uint16_t getData_Sensor_2();


#endif	/* SAMPLING_H */

