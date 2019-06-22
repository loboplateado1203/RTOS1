/*
 * KEYBRD_5C.h
 *
 *  Created on: May 20, 2019
 *      Author: Lorena B. Zacharewicz
 */


/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef _KEYBRD_5C_H_
#define _KEYBRD_5C_H_

/*=====[Inclusiones de dependencias de funciones publicas]===================*/

// Includes de FreeRTOS
#include "../inc/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"     //"mutex"
#include "timers.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "stream_buffer.h"

#include "sapi.h"               // <= sAPI header

/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*=======================[Macros de definicion de constantes publicas]=========================*/

/*=======================[Macros estilo funcion publicas]======================================*/

/*=======================[Definiciones de tipos de datos publicos]=============================*/

//PROBLEMA SERIO
//static double  acumulador = 0; // Compila bien y SI crea el .elf exitosamente
//       double  acumulador = 0; // Compila bien y NO crea el .elf y da error indefinido, tuve que dejar en el .c donde no da error

/*=======================[Tipo de datos enumerado y otros]=====================================*/
static gpioMap_t GPIO00;				  //GPIOs redefinidos para uso interno
static gpioMap_t GPIO01;
static gpioMap_t GPIO02;
static gpioMap_t GPIO03;
static gpioMap_t GPIO04;
static gpioMap_t GPIOINT;

static uint8_t KeyPulsada = -1;           //variables para no tener repeticiones indeseables de pulsadas
static uint8_t ultimaKeyPulsada = -1;

static uint8_t bUsarDecimales = FALSE;    //looleano para saber si ya se pulsó el punto decimal
static uint8_t nDeDecimales = 0;          //índice para indicar la posc. del decimal que se debe introducir

static uint8_t bPrimerKeyPulsada = FALSE; //Lo uso solo una vez por arranque

static uint8_t bOperacionSeteada = FALSE; //para no ingresar números si no hay operación seteada

static uint8_t bGrabarYEjecutar  = FALSE; //para no ingresar dos veces seguidas esta operación

static uint8_t nDeOperacion = 1;          //contador de operaciones en HexDEC
static uint8_t nDeOperacionAscii[4] = {'0', '1', ')', ' '}; //String con el número de operación , el paréntesis y el espacio

typedef enum estadosWBM
{
	Avanzar         = 10,
	Rotar           = 11,
	Girar           = 12,
	GrabarYEjecutar = 13,
	ResetearWBM     = 15
} estWBM_t;

xQueueHandle Global_Queue_Handle;   //Queue


/*=======================[Prototipos de funciones del KEYBRD_5C]====================================*/


//Función de selección de GPIOs para el teclado
//Los 4 primeros son para los bits 0 a 3 del código de tecla pulsada y el último para indicar que se pulsó alguna tecla
void setupKEYBRD_5C( gpioMap_t, gpioMap_t, gpioMap_t, gpioMap_t, gpioMap_t );


//Función de servicio del teclado, construye las variables según las teclas y secuencias pulsadas
/*
 * [0] --> 0   [8] --> 8
 * [1] --> 1   [9] --> 9
 * [2] --> 2   [A] --> 10
 * [3] --> 3   [B] --> 11
 * [4] --> 4   [C] --> 12
 * [5] --> 5   [D] --> 13
 * [6] --> 6   [*] --> 14
 * [7] --> 7   [#] --> 15
 */
void servicioDeKeyBrd( void *P );

//Función que devuelve el divisor cuando se introducen números después del punto decimal
double _divisor( uint8_t );

//Función que incrementa y devuelve el string indicando el número de operación
void _inCchar1HexToDecAscii ( unsigned char );


/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* _KEYBRD_5C_H_ */
