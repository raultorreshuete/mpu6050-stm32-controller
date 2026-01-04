/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */
	
#ifndef _JOYS_

	#include "stm32f4xx_hal.h"
	
	//Flag que indica que el joystick ha sido pulsado (se envía al hilo del timer que gestiona rebotes)
	#define JOYS_pulsado 0x00000002U
	
	//Flag que indica que ya ha finalizado el timer de los rebotes 
	// (se envía al hilo del jystick para que proceda a leer que acción se ha pulsado)
	#define TIMER_REBO 0x00000004U
	
	#define _JOYS_
	
		int Init_joys(void); //Función de inicialización del hilo del joystick
		int Init_timerRebo (void); //Función de inicialización del hilo del timer de los rebotes
		
		void initJoys(void); //Función de inicialización FÍSICA de las 5 acciones del joystick
		
		//Implementación para gestionar interrupciones de las acciones del Joystick
		void EXTI15_10_IRQHandler(void);
		
		//Estructura de un gpio para mayor comodidad
		typedef struct{
			GPIO_InitTypeDef gpio;
			GPIO_TypeDef *port;
		} mygpioJ;
		
		//Estructura InfoJoys, se introducirá en la cola que va al principal.
		// Proprociona información sobre que acción se ha pulsado en el josytick,
		// y si esta es larga (tipo_pulsacion = 1) o corta (tipo_pulsacion = 0)
		typedef struct{
			uint16_t GPIO_PIN;
			uint8_t tipo_pulsacion;
		} infoJoys;
		
//		typedef uint8_t entero;
//		
//		struct infoJoy{
//			uint16_t GPIO_PIN;
//			entero pulsacion;
//		};
		
#endif
