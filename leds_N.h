/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#ifndef LEDS_N_H
	

#include "stm32f4xx_hal.h"

//// Inicializa los LEDs
//void LEDs_Init(void);

//// Enciende un LED específico
//void LED_On(uint8_t led_number);

//// Apaga un LED específico
//void LED_Off(uint8_t led_number);

//// Apaga todos los LEDs
//void LEDs_AllOff(void);

	#define LED1_ON 0x00000001U
	#define LED1_OFF 0x00000010U
	#define LED1 0x00000011U
	
	#define LED2_ON 0x00000002U
	#define LED2_OFF 0x00000020U
	#define LED2 0x00000022U
	
	#define LED3_ON 0x00000004U
	#define LED3_OFF 0x00000040U
	#define LED3 0x00000044U

	#define LEDS_N_H
		int Init_led1 (void);
		int Init_led2 (void);
		int Init_led3 (void);

		void initLED1(void);
		void initLED2(void);
		void initLED3(void);

		typedef struct{ //Estructura de un gpio para mayor comodidad
			GPIO_InitTypeDef gpio;
			GPIO_TypeDef *port;
		} gpioled;

#endif // LEDS_N_H
