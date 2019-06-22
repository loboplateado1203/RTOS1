/*
 * wire_bending_RTOS.h
 *
 *  Created on: May 20, 2019
 *      Author: Lorena B. Zacharewicz
 */


/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef _wire_bending_RTOS_H_
#define _wire_bending_RTOS_H_

/*=====[Inclusiones de dependencias de funciones publicas]===================*/

#include "sapi.h"
/*=====[C++ comienzo]========================================================*/




#ifdef __cplusplus
extern "C" {
#endif


/*=======================[Macros de definicion de constantes publicas]=========================*/

/*=======================[Macros estilo funcion publicas]======================================*/

/*=======================[Definiciones de tipos de datos publicos]=============================*/

/*=======================[Tipo de datos enumerado y otros]=====================================*/

/*=======================[Prototipos de funciones ]===========================================*/

bool_t bParadaDeEmergencia;
bool_t bIniciando_WBM;

void taskParadaDedEmergencia( void* taskParmPtr );


/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* _wire_bending_interfaz_H_ */
