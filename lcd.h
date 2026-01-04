/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#ifndef LCD_H

	#include <stdint.h>
	#include "stm32f4xx_hal.h"
	#include "Driver_SPI.h"

	#define LCD_H
	
		int Init_display (void);

		void initRESET(void);
		void initCS(void);
		void initA0(void);
		void delay(volatile uint32_t n_microsegundos);
		void LCD_reset(void);
		void LCD_wr_data(unsigned char data);
		void LCD_wr_cmd(unsigned char cmd);
		void LCD_init(void);
		void LCD_update(void);
		void LCD_symbolToLocalBuffer(uint8_t line,uint8_t symbol, uint8_t reset);
		void LCD_modificarByteBuffer(uint8_t pos, uint8_t page, char byte);

		
		void clk_enable(void);
		
		extern ARM_DRIVER_SPI Driver_SPI1; //driver del protocolo SPI
		
		typedef struct{ //Estructura de un gpio para mayor comodidad
			uint8_t modo;
			uint8_t estadoDepu;
			float acceler_x;
			float acceler_y;
			float acceler_z;
			float Ax_r; 
			float Ay_r;
			float Az_r;
			float temper;
			uint8_t hora;
			uint8_t minuto;
			uint8_t segundo;
		} infoLCD;

#endif // LCD_H
