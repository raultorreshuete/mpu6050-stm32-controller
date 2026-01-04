/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#ifndef CLOCK_H

	#include "stm32f4xx_hal.h"
	
	#define CLOCK_H

		//Función inicialización del hilo del reloj
		int Init_clock (void);

		//Estructura HoraCompleta, que contiene un uint8_t con cada componente de la hora
		typedef struct{ 
			uint8_t horas;
			uint8_t minutos;
			uint8_t segundos;
		} HoraCompleta;

#endif // CLOCK_H
