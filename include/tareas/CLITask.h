/* 
 * File:   CLITask.h
 * Author: Tincho
 *
 * Created on 25 de febrero de 2019, 09:46
 */

#ifndef CLITASK_H
#define	CLITASK_H

#include "typedef.h"

#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/timers.h"
#include "freeRTOSConfig.h"

#include "funciones/shell.h"

#include "ezbl.h"

#include <string.h>

void startCLITask();

#endif	/* CLITASK_H */

