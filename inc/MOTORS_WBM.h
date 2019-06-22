/*
 * MOTORS_WBM.h
 *
 *  Created on: Jun 20, 2019
 *      Author: pablo
 */

#ifndef WIRE_BENDING_INTERFAZ_INC_MOTORS_WBM_H_
#define WIRE_BENDING_INTERFAZ_INC_MOTORS_WBM_H_

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

#define N_INST_MAX  20

typedef enum
{
	PARAR   = 0,
	AVANZAR = 1,
	ROTAR   = 2,
	GIRAR   = 3
} mov_t;

typedef struct
{
	mov_t movimiento;
	float valor;
} instrucion_t;

xSemaphoreHandle gateKeeper;        //Mutex

xSemaphoreHandle employee_signal_motorAvanzar;   //Semáforo binario
xSemaphoreHandle employee_signal_motorRotar;     //Semáforo binario
xSemaphoreHandle employee_signal_motorGirar;     //Semáforo binario


static instrucion_t inst[ N_INST_MAX ];

static uint8_t nInst = 0;

bool_t bServicioDeMotores_WBM;

float poscGlobalMOTORES;

void setupMOTORS_WBM( void );
void cleanInstrucciones( void );
void pushInstMOTORS_WBM( mov_t mov, float valor );
void servicioDeMOTORES_WBM( void *p);
void motorGirar( void *p );  //( float parametro );
void motorAvanzar( void *p );//( float parametro );
void motorRotar( void *p );  //( float parametro );


#endif /* EXAMPLES_C_SAPI_I2C_WIRE_BENDING_INTERFAZ_INC_MOTORS_WBM_H_ */
