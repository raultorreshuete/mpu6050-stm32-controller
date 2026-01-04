/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "leds_N.h"
#include "stm32f4xx_hal.h"

osThreadId_t tid_led1, tid_led2, tid_led3;

void thled1(void *argument);
void thled2(void *argument);
void thled3(void *argument);

gpioled led1, led2, led3;

uint32_t statusL1, statusL2, statusL3;

/******************************IMPLEMENTACIÓN BUZZER*****************************/
/*****ENCENDEREMOS EL BUZZER EMITIENDO UN PITIDO CUANDO SE SOBREPASE LA AZ_R*****/
static GPIO_InitTypeDef GPIO_InitStruct_pwm;
static TIM_OC_InitTypeDef htim2OCconf;
static TIM_HandleTypeDef htim2;

static void Init_PWM(void); //funcion para inicializar la señal PWM


int Init_led1 (void) {
	
	initLED1();//Llamamos a la inicialización del LED
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_led1 = osThreadNew(thled1, (void *)&led1, NULL);
  if (tid_led1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void thled1 (void *argument) {
	
	gpioled *gpio = (gpioled *)argument; //Ajuste de punteros para que el hilo pueda recibir la informaci?n necesaria
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) ); //Indica que se inicialice el GPIO en el puerto y con las caracter?sticas confirmadas anteriormente,
																						 // es el equivalente al HAL_GPIO_Init(GPIOB, &GPIO_InitStructureLD1) que us?bamos en el otro formato
  while (1) {

		//Espera hasta que llegue flag de activaci?n de Led (ya sea LED_ON o LED_OFF)
		statusL1 = osThreadFlagsWait(LED1, osFlagsWaitAny, osWaitForever); 

		if(statusL1 == LED1_ON){ //Si la flag es LED_ON, enciende el Led
				HAL_GPIO_WritePin(gpio->port, gpio->gpio.Pin, 1);
		}
		if(statusL1 == LED1_OFF){ //Si la flag es LED_OFF, apaga el Led
				HAL_GPIO_WritePin(gpio->port, gpio->gpio.Pin, 0);
		}	

		//De inicio el tick del sistema est? configurado a 1 ms, 
		//si se desea cambiar su valor, sin necesidad de tocar el c?digo es:
		/***RTX_Config.h -> System Configuration -> Kernel Tick Frequency[Hz]***/
		
    osThreadYield(); // suspend thread
  }
}

int Init_led2 (void) {
	
	initLED2();//Llamamos a la inicialización del LED
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_led2 = osThreadNew(thled2, (void *)&led2, NULL);
  if (tid_led2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void thled2 (void *argument) {
	
	gpioled *gpio = (gpioled *)argument; //Ajuste de punteros para que el hilo pueda recibir la informaci?n necesaria
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) ); //Indica que se inicialice el GPIO en el puerto y con las caracter?sticas confirmadas anteriormente,
																						 // es el equivalente al HAL_GPIO_Init(GPIOB, &GPIO_InitStructureLD1) que us?bamos en el otro formato
  while (1) {

		//Espera hasta que llegue flag de activación de Led (ya sea LED_ON o LED_OFF)
		statusL2 = osThreadFlagsWait(LED2, osFlagsWaitAny, osWaitForever); 

		if(statusL2 == LED2_ON){ //Si la flag es LED_ON, enciende el Led
				HAL_GPIO_WritePin(gpio->port, gpio->gpio.Pin, 1);
		}
		if(statusL2 == LED2_OFF){ //Si la flag es LED_OFF, apaga el Led
				HAL_GPIO_WritePin(gpio->port, gpio->gpio.Pin, 0);
		}	

		//De inicio el tick del sistema est? configurado a 1 ms, 
		//si se desea cambiar su valor, sin necesidad de tocar el c?digo es:
		/***RTX_Config.h -> System Configuration -> Kernel Tick Frequency[Hz]***/
		
    osThreadYield(); // suspend thread
  }
}

int Init_led3 (void) {
	
	initLED3();//Llamamos a la inicialización del LED
	
	Init_PWM();
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_led3 = osThreadNew(thled3, (void *)&led3, NULL);
  if (tid_led3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void thled3 (void *argument) {
	
	gpioled *gpio = (gpioled *)argument; //Ajuste de punteros para que el hilo pueda recibir la informaci?n necesaria
	HAL_GPIO_Init(gpio->port, &(gpio->gpio) ); //Indica que se inicialice el GPIO en el puerto y con las caracter?sticas confirmadas anteriormente,
																						 // es el equivalente al HAL_GPIO_Init(GPIOB, &GPIO_InitStructureLD1) que us?bamos en el otro formato
  while (1) {

		//Espera hasta que llegue flag de activaci?n de Led (ya sea LED_ON o LED_OFF)
		statusL3 = osThreadFlagsWait(LED3, osFlagsWaitAny, osWaitForever); 

		if(statusL3 == LED3_ON){ //Si la flag es LED_ON, enciende el Led
				HAL_GPIO_WritePin(gpio->port, gpio->gpio.Pin, 1);
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); //Encendido del buzzer
		}
		if(statusL3 == LED3_OFF){ //Si la flag es LED_OFF, apaga el Led
				HAL_GPIO_WritePin(gpio->port, gpio->gpio.Pin, 0);
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4); //Apagado del buzzer
		}	

		//De inicio el tick del sistema est? configurado a 1 ms, 
		//si se desea cambiar su valor, sin necesidad de tocar el c?digo es:
		/***RTX_Config.h -> System Configuration -> Kernel Tick Frequency[Hz]***/
		
    osThreadYield(); // suspend thread
  }
}

//Función Inicialización LED1
void initLED1(void){
	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		led1.gpio.Mode = GPIO_MODE_OUTPUT_PP;
		led1.gpio.Pull = GPIO_PULLUP; 
		led1.gpio.Speed = GPIO_SPEED_FREQ_LOW; 
		led1.gpio.Pin = GPIO_PIN_0; 
	
		led1.port = GPIOB;
}

//Función Inicialización LED1
void initLED2(void){
	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		led2.gpio.Mode = GPIO_MODE_OUTPUT_PP;
		led2.gpio.Pull = GPIO_PULLUP; 
		led2.gpio.Speed = GPIO_SPEED_FREQ_LOW; 
		led2.gpio.Pin = GPIO_PIN_7; 
	
		led2.port = GPIOB;
}

//Función Inicialización LED1
void initLED3(void){
	
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		led3.gpio.Mode = GPIO_MODE_OUTPUT_PP;
		led3.gpio.Pull = GPIO_PULLUP; 
		led3.gpio.Speed = GPIO_SPEED_FREQ_LOW; 
		led3.gpio.Pin = GPIO_PIN_14; 
	
		led3.port = GPIOB;
}


static void Init_PWM(void){
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();
  
  GPIO_InitStruct_pwm.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct_pwm.Pull = GPIO_PULLUP;
  GPIO_InitStruct_pwm.Alternate = GPIO_AF1_TIM2; //timer 2 de alternativo para pin 3
  
  GPIO_InitStruct_pwm.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_pwm);

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 999;
  htim2.Init.Period = 167;// resutlado de 500hz

  HAL_TIM_PWM_Init(&htim2);
  htim2OCconf.OCMode = TIM_OCMODE_PWM1;
  htim2OCconf.Pulse = 84; //50% de ciclo de trabajo
  HAL_TIM_PWM_ConfigChannel(&htim2,&htim2OCconf,TIM_CHANNEL_4);
}
