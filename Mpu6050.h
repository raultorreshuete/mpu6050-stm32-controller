/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */
	
#ifndef _MPU6050_

	#include "stm32f4xx_hal.h"
	
	#define TIM_PERIODICO 0x00000001U
	#define SENSOR_ADDRESS 0x68
	
	#define _SENSOR_
	
		int Init_Mpu6050(void); //Función de inicialización del hilo
		
		void Callback_timerPeriod1seg (void const *arg);

		// Data structure for sensor readings	
		typedef struct {
				float accel_x;
				float accel_y;
				float accel_z;
				float temperature;
		} infoMPU;
		
#endif