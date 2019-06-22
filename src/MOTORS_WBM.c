/*
 * MOTORS_WBM.c
 *
 *  Created on: Jun 06, 2019
 *      Author: Lorena B. Zacharewicz
 */

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

// includes mios
//#include "../inc/LCD_I2C_PCF8574.h"
#include "../inc/KEYBRD_5C.h"
#include "../inc/MOTORS_WBM.h"
#include "../inc/wire_bending_RTOS.h"
#include "sapi.h"               // <= sAPI header
#include "sapi_delay.h"         /* <= sAPI Delay header */


//#include <math.h>

xSemaphoreHandle employee_signal_motorAvanzar = 0;   //Semáforo binario
xSemaphoreHandle employee_signal_motorRotar   = 0;   //Semáforo binario
xSemaphoreHandle employee_signal_motorGirar   = 0;   //Semáforo binario

void setupMOTORS_WBM()
{
	cleanInstrucciones();

	bServicioDeMotores_WBM = FALSE;

	gateKeeper = xSemaphoreCreateMutex();

	vSemaphoreCreateBinary( employee_signal_motorAvanzar );
	vSemaphoreCreateBinary( employee_signal_motorRotar   );
	vSemaphoreCreateBinary( employee_signal_motorGirar   );

	//Creo las tareas

	xTaskCreate( servicioDeMOTORES_WBM, (const char *) "servicioDeMOTORES_WBM", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, NULL );

	xTaskCreate( motorAvanzar, (const char *) "motorAvanzar", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( motorRotar,   (const char *) "motorRotar",   configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( motorGirar,   (const char *) "motorGirar",   configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+2, NULL );

}

void cleanInstrucciones()
{
	nInst = 0;

	for ( int i = 0; i < N_INST_MAX; i++ )
	{
		//inst [ i ].movimiento = PARAR;
		inst [ i ].valor      = 0;
	}
}

void pushInstMOTORS_WBM(mov_t mov, float valor)
{
	inst [ nInst ].movimiento = mov;
	inst [ nInst ].valor      = valor;

	nInst++;
}

void servicioDeMOTORES_WBM( void *P )
{
	while (1)
	{
		if ( xQueueReceive(Global_Queue_Handle, &bServicioDeMotores_WBM, 1000 / portTICK_RATE_MS )){
			printf ( "\nGrabando y Ejecutando WBM\n\n");
		}
		if ( bServicioDeMotores_WBM )
		{
			//printf ( "\nGrabando y Ejecutando WBM\n\n");
			for ( int i = 0; i <= nInst ; i++ )
			{
				poscGlobalMOTORES = inst[ i ].valor;  //llevo la posición a cada motor

				switch (inst[ i ].movimiento )
				{
				case PARAR:
					 printf ( "Motores parados\n" );
					 //motorAvanzar( 0 );
					 //motorRotar  ( 0 );
					 //motorGirar  ( 0 );
					 bServicioDeMotores_WBM = FALSE;
					 break;

				case AVANZAR:
					 xSemaphoreGive( employee_signal_motorAvanzar );
					 break;

				case ROTAR:
					 xSemaphoreGive( employee_signal_motorRotar );
					 break;

				case GIRAR:
					 xSemaphoreGive( employee_signal_motorGirar );
					 break;
				}
			}
		}
		vTaskDelay( 1000 / portTICK_RATE_MS );
	}
}


void motorAvanzar( void *p )  //(float parametro)
{
	while (1){
		if ( xSemaphoreTake( employee_signal_motorAvanzar, portMAX_DELAY ) ){
//			if ( xSemaphoreTake( gateKeeper, 100 / portTICK_RATE_MS ) ){
//				printf( "motorAvanzar iniciando\n" );

					 if ( !bParadaDeEmergencia ) printf ( "Avanzando: %\n", poscGlobalMOTORES );

//				 xSemaphoreGive( gateKeeper );
//			}
//			printf( "motorAvanzar teminando\n\n");
		}
	}
}

void motorRotar( void *p )  //(float parametro)
{
	while (1){
		if ( xSemaphoreTake( employee_signal_motorRotar, portMAX_DELAY ) ){
//			if ( xSemaphoreTake( gateKeeper, 100 / portTICK_RATE_MS ) ){
//				printf( "motorRotar iniciando\n" );

				 	 if ( !bParadaDeEmergencia ) printf ( "Rotando: %f\n", poscGlobalMOTORES );

//				 xSemaphoreGive( gateKeeper );
//			}
//			printf( "motorRotar teminando\n\n");
		}
	}
}

void motorGirar( void *p )  //(float parametro)
{
	while (1){
		if ( xSemaphoreTake( employee_signal_motorGirar, portMAX_DELAY ) ){
//			if ( xSemaphoreTake( gateKeeper, 100 / portTICK_RATE_MS ) ){
//				printf( "motorGirar iniciando\n" );

				 	 if ( !bParadaDeEmergencia ) printf ( "Girando: %e\n", poscGlobalMOTORES );

//				 xSemaphoreGive( gateKeeper );
//			}
//			printf( "motorGirar teminando\n\n");
		}
	}
}



