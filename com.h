/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */
	
#ifndef _COM_

	#include "stm32f4xx_hal.h"
	#include "Driver_USART.h"
	
	#define TRAMA_RECIBIDA 0x00000001U
	#define TRAMA_TRANSMITIDA 0x00000002U
	
	#define _SENSOR_
	
	#define MSGQUEUE_OBJECTS_COM 10
	
		int Init_Com(void); //Función de inicialización del hilo

		int Init_Test(void); //Función de inicialización del hilo


		void ThComRecep(void *argument);
		void ThComTrans(void *argument);

		typedef struct {
			char CMD;
			char LENGTH;
			char Payload[44];
		} infoMensaje;
		
#endif