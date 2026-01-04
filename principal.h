/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#ifndef PRINCIPAL_H

	#include "stm32f4xx_hal.h"

	#define LED1_ON 0x00000001U
	#define LED1_OFF 0x00000010U
	#define LED1 0x00000011U
	
	#define LED2_ON 0x00000002U
	#define LED2_OFF 0x00000020U
	#define LED2 0x00000022U
	
	#define LED3_ON 0x00000004U
	#define LED3_OFF 0x00000040U
	#define LED3 0x00000044U
	
	#define PRINCIPAL_H

		int Init_principal (void);

		// Función para actualizar el reloj cada segundo
		void Thprincipal(void *argument);
		
//		typedef struct {
//				uint8_t hora;
//				uint8_t minutos;
//				uint8_t segundos;
//				float temperature;
//				float accel_x;
//				float accel_y;
//				float accel_z;
//		} infoBufCircular;

#endif // PRINCIPAL_H
