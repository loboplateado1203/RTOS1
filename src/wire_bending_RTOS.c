/* Copyright 2019, Lorena B. Zacharewicz.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Date: 2019-05-29 */

/*==================[inclusions]=============================================*/

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
#include "sapi_i2c.h"           /* <= sAPI I2C header */
#include "sapi_delay.h"         /* <= sAPI Delay header */


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
        //Mutex
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
//void servicioDeKeyBrd1 ( void *P );

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){
   /* ------------- INICIALIZACIONES ------------- */

   // Inicialización del Board de la CIAA
   boardConfig();

   bParadaDeEmergencia = FALSE;
   bIniciando_WBM = TRUE;

   // Inicialización del I2C de la CIAA
//   i2cInit(I2C0, 100000);


   // Inicialización del LCD_I2C_PCF8574
//   setup_LCD_I2C_PCF8574( 0x27, 4, 20, LCD_5x8DOTS  );


   // Inicialización del KEYBRD_5C
   setupKEYBRD_5C( GPIO0, GPIO1, GPIO2, GPIO3, GPIO4 );

   // Inicialización de los Motores
   setupMOTORS_WBM();

   // Mensaje probando el puerto serie a la PC
   printf("Inicializando wire bending interfaz i2c 5c\r\n" );
   printf("Lorena B. Zacharewicz CESE 8va Cohorte 2019\r\n" );


   xTaskCreate( servicioDeKeyBrd,
		   (const char *) "servicioDeKeyBrd",
		   configMINIMAL_STACK_SIZE*2,
		   NULL,
		   tskIDLE_PRIORITY+1,
		   NULL
		   );
   xTaskCreate( taskParadaDedEmergencia,
		   (const char *)"taskParadaDedEmergencia",
		   configMINIMAL_STACK_SIZE*2,
		   0,
		   tskIDLE_PRIORITY+3,
		   0
		   );

   bIniciando_WBM = FALSE;

    // Iniciar scheduler
    vTaskStartScheduler();

   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1){

	   //printf( "algo salió mal\n" );
	}

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

void taskParadaDedEmergencia( void* taskParmPtr )
{
   gpioWrite( LED1, ON );
   // Envia la tarea al estado bloqueado durante 500 mSeg (delay)
   vTaskDelay( 500 / portTICK_RATE_MS );
   gpioWrite( LED1, OFF );

   // Tarea periodica cada 100 ms
   portTickType xPeriodicity =  100 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();

   // Antirrebote
   portTickType xTiempoAntiRebote =  50 / portTICK_RATE_MS; //50 mSeg

   // Variable para almacenar los valores de tecla leído
   bool_t valorAnterior = FALSE;
   bool_t valorActual   = FALSE;

   // Tecla a utilizar
   gpioMap_t BOTON = TEC1;

   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE) {

	   // El antirrebote. . .
	   valorAnterior = !gpioRead(BOTON);
	   vTaskDelay( xTiempoAntiRebote );
	   valorActual = !gpioRead(BOTON);

	   // Si el botón está activado, cuento las unidades de tiempo mínimo de 50 mS
	   if( valorActual ){
		  if(valorAnterior == valorActual){
			 bParadaDeEmergencia = TRUE;
			 printf( "Parada de Emergencia!!\n" );
			 vSemaphoreDelete( employee_signal_motorAvanzar );
			 vSemaphoreDelete( employee_signal_motorRotar   );
			 vSemaphoreDelete( employee_signal_motorGirar   );
		   }
	   }
	   //  Repetir por siempre, cada 100 mSeg
	   vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
   }
}

