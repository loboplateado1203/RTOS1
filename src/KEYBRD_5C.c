/*
 * KEYBRD_5C.c
 *
 *  Created on: May 20, 2019
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

static mov_t ultOperacion;

/*=======================[Definiciones de tipos de datos publicos]=============================*/

//aqui se guarda el número ingresado por el keyboard
float  acumulador = 0;  // Compila bien y SI crea el .elf exitosamente VER ERROR DEL .h


//Divisor usado cuando se ingresan sucesivos decimales
double _divisor(uint8_t n)
{
	double mult = 1;

	for (uint8_t i = 1; i <= n; i++ ){
		mult *= 10;
	};

	return mult;
};

//Toma un HexDec lo incrementa en 1 y lo convierte en string
uint8_t _incChar1HexToDecAscii ( unsigned char x ){
	unsigned char y;

	y = x & 0b00001111;	//elimino el 2do nibble
	x =	x & 0b11110000;	//elimino el 1er nibble
	x = x / 16;			//desplazo  4 bits a la derecha

	if (y == 9){ x++; y = 0;} else { y++; }; //incremento en 1

	nDeOperacionAscii[0] = x + 48;  //convierto es ascii y guardo
	nDeOperacionAscii[1] = y + 48;

	//x = x * 16; 		//desplazo  4 bits a la izquierda
	return x*16 + y;    //reconvierto en HexDec
	//return x*10 + y;	//reconvierto en entero
}

void setupKEYBRD_5C( gpioMap_t KeyBit0, gpioMap_t KeyBit1, gpioMap_t KeyBit2, gpioMap_t KeyBit3, gpioMap_t KeyINT )
{

	Global_Queue_Handle = xQueueCreate(1, sizeof(bool_t));

	GPIO00  = KeyBit0;
	GPIO01  = KeyBit1;
	GPIO02  = KeyBit2;
	GPIO03  = KeyBit3;
	GPIOINT = KeyINT;

	gpioInit( GPIO00,  GPIO_INPUT );
	gpioInit( GPIO01,  GPIO_INPUT );
	gpioInit( GPIO02,  GPIO_INPUT );
	gpioInit( GPIO03,  GPIO_INPUT );
	gpioInit( GPIOINT, GPIO_INPUT );

};


void servicioDeKeyBrd( void *P )
{
   // Tarea periodica cada 200 ms
   portTickType xPeriodicity =  200 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();

   while(TRUE)
   {
		//if ( xSemaphoreTake( gateKeeper2, 1000 / portTICK_RATE_MS ) )
		//{
		   if ( gpioRead(GPIO00) ){ gpioWrite( LED3, ON );} else {gpioWrite( LED3, OFF );}; //diversión con los LEDS para ver el código del teclado
		   if ( gpioRead(GPIO01) ){ gpioWrite( LED2, ON );} else {gpioWrite( LED2, OFF );};
		   if ( gpioRead(GPIO02) ){ gpioWrite( LED1, ON );} else {gpioWrite( LED1, OFF );};
		   if ( gpioRead(GPIO03) ){ gpioWrite( LEDB, ON );} else {gpioWrite( LEDB, OFF );};

		   if (!gpioRead(GPIOINT) ) {
			   //delay(10);   //esto es para solucionar un bug del teclado que hace que el aviso de tecla pulsada se apague por 35uS aunque no se suelte la tecla
			   vTaskDelay( 10 / portTICK_RATE_MS );
			   if (!gpioRead(GPIO4) ) {
				   ultimaKeyPulsada = -1; //Esto me asegura pulsadas sucesivas de la misma tecla
			   }
		   }

		   if ( gpioRead(GPIOINT) ){
			   KeyPulsada = 0;
			   if ( gpioRead(GPIO00) ) KeyPulsada += 1;
			   if ( gpioRead(GPIO01) ) KeyPulsada += 2;
			   if ( gpioRead(GPIO02) ) KeyPulsada += 4;
			   if ( gpioRead(GPIO03) ) KeyPulsada += 8;

			   if ( ultimaKeyPulsada != KeyPulsada ){
					ultimaKeyPulsada  = KeyPulsada;

					if ( !bPrimerKeyPulsada && KeyPulsada != ResetearWBM )
					{
						bPrimerKeyPulsada = TRUE;
		//				clear();
		//			    setCursor( 1, 0 );
		//			    print_LCD2( "WBM LBZ En Servicio;");
						printf( "WBM LBZ En Servicio\n");
		//			    setCursor( 2, 0 );
		//			    print_LCD2( " Operacion = Valor;" );
						printf( " Operacion = Valor\n" );
		//			    setCursor( 3, 0 );
					}

					if ( bOperacionSeteada )
					{

						if ( KeyPulsada < 10 ){
							if ( bUsarDecimales ){
								nDeDecimales += 1;
								acumulador =  acumulador + KeyPulsada / _divisor( nDeDecimales );
		//						write( 48 + KeyPulsada ); //El ascii del número
								printf ( "%c", 48 + KeyPulsada ); fflush ( NULL );
							}else{
								acumulador =  10 * acumulador + KeyPulsada;
		//						write( 48 + KeyPulsada ); //El ascii del número
								printf ( "%c", 48 + KeyPulsada ); fflush ( NULL );
							}
						}
						if ( KeyPulsada == 14 ){
							if ( !bUsarDecimales ){ //el if es para que solo se active una vez
								  bUsarDecimales = TRUE;
		//						  write( '.' );
								  printf ( "." );
								  nDeDecimales = 0;
							}
						}
					}
					switch (KeyPulsada){
					   case Avanzar:
							   //TODO guardar operación anterior
							   if ( acumulador != 0 ){
		//				    	   pullUpLineas();
								   printf ( "\n" );
								   nDeOperacion = _incChar1HexToDecAscii( nDeOperacion );
								   pushInstMOTORS_WBM( ultOperacion, acumulador);
							   } else {
		//				    	   clearLinea( 3 );
							   }; //piso la linea si no se han introducido valores

							   ultOperacion = AVANZAR;

							   acumulador = 0;
							   bUsarDecimales =  FALSE;
							   nDeDecimales = 0;

							   bOperacionSeteada =  TRUE;

		//				   	   print_LCD( nDeOperacionAscii, 4);
		//				   	   printf ( &nDeOperacionAscii );
		//					   print_LCD( "Avanzar=", 8);
							   printf ( "Avanzar=" ); fflush ( NULL );
		//					   setCursor( 3, 13 );
						   break;
					   case Girar:
							   //TODO guardar operación
							   if ( acumulador != 0 ){
		//						   pullUpLineas();
								   printf ( "\n" );
								   nDeOperacion = _incChar1HexToDecAscii( nDeOperacion );
								   pushInstMOTORS_WBM( ultOperacion, acumulador);
							   } else {
		//						   clearLinea( 3 );
							   }; //piso la linea si no se han introducido valores

							   ultOperacion = GIRAR;

							   acumulador = 0;
							   bUsarDecimales =  FALSE;
							   nDeDecimales = 0;

							   bOperacionSeteada =  TRUE;

		//					   print_LCD( nDeOperacionAscii, 4)
		//				   	   printf ( &nDeOperacionAscii );
		//					   print_LCD( "Girar  =", 8);
							   printf ( "Girar=" ); fflush ( NULL );
		//					   setCursor( 3, 13 );
						   break;
					   case Rotar:
							   //TODO guardar operación
							   if ( acumulador != 0 ){
		//						   pullUpLineas();
								   printf ( "\n" );
								   nDeOperacion = _incChar1HexToDecAscii( nDeOperacion );
								   pushInstMOTORS_WBM( ultOperacion, acumulador);
							   } else {
		//						   clearLinea( 3 ); //piso la linea si no se han introducido valores
							   };

							   ultOperacion = ROTAR;

							   acumulador = 0;
							   bUsarDecimales =  FALSE;
							   nDeDecimales = 0;

							   bOperacionSeteada =  TRUE;

		//					   print_LCD( nDeOperacionAscii, 4);
		//				   	   printf ( &nDeOperacionAscii );
		//					   print_LCD( "Rotar  =", 8);
							   printf ( "Rotar=" ); fflush ( NULL );
		//					   setCursor( 3, 13 );
						   break;
					   case GrabarYEjecutar:
							   if ( nDeOperacion ) //si hay operaciones puedo grabar y ejecutar
							   {
								   if ( !bGrabarYEjecutar ) //solo se hace esto la primera vez que gravo y ejecuto
								   {
									   bGrabarYEjecutar = TRUE;
									   if ( acumulador != 0 ){
			//							   pullUpLineas();
										   printf ( "\n" );
										   pushInstMOTORS_WBM( ultOperacion, acumulador);
									   } else {
			//							   clearLinea( 3 );
									   };

									   acumulador = 0;
									   bUsarDecimales     = FALSE;
									   nDeDecimales = 0;

									   bOperacionSeteada =  FALSE;
									   //print_LCD2( "Grabar y Ejecutar;");
									   printf ( "Grabar y Ejecutar\n" );
			//						   setCursor( 3, 18 );
								   }
								   //servicioDeMOTORES_WBM( ); //arranco el doblado de alambre siempre se pulsa esta tecla
								   bServicioDeMotores_WBM = TRUE;
								   //xQueueSend(Global_Queue_Handle, &bServicioDeMotores_WBM, 1000 / portTICK_RATE_MS );
								   xQueueSend(Global_Queue_Handle, (bool_t *)TRUE, 1000 / portTICK_RATE_MS );
							   }
						   break;
					   case ResetearWBM:
							   acumulador = 0;
							   bUsarDecimales    = FALSE;
							   nDeDecimales = 0;
							   bOperacionSeteada = FALSE;
							   bGrabarYEjecutar  = FALSE;
							   bPrimerKeyPulsada = FALSE;
							   nDeOperacion = 1;
							   nDeOperacionAscii[0] = '0';
							   nDeOperacionAscii[1] = '1';
							   nDeOperacionAscii[2] = ')';
							   nDeOperacionAscii[3] = ' ';

		//					   caratula_LCD();
		//					   clear();
		//					   setCursor( 1, 0 );
		//					   print_LCD2( "Wire Bending Machine;" );
							   printf ( "\nWire Bending Machine\n" );
		//					   setCursor( 2, 0 );
		//					   print_LCD2( " Zacharewicz Tools;" );
							   printf ( " Zacharewicz Tools\n" );
		//					   setCursor( 3, 0 );

							   //setupMOTORS_WBM();
							   cleanInstrucciones();

						   break;
					   default:
						   break;
				   }
			   }
	       }
		//xSemaphoreGive( gateKeeper2 );
		//}
   }
   // Repetir por siempre, cada 200 mSeg
   vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
}






