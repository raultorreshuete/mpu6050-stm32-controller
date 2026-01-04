/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#include "cmsis_os2.h"
#include "joystick.h"	//Llamada a su .h (suele contener structs, Flags y funciones importantes)

#define MSGQUEUE_OBJECTS 1 //Define el tamaño de la cola

/*----------------------------------------------------------------------------
 *   				Thread 'joystick': Hilo que gestiona el joystick
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_joys; //Id del hilo del joystick
osThreadId_t tid_timerRebo; //Id del hilo que gestiona rebotes
osTimerId_t tid_timRebo; //Id del timer que gestiona los rebotes
osTimerId_t tid_tim1seg; //Id del timer de 1 seg (gestiona tipo pulsación

osMessageQueueId_t IdqueueJoys; //Id de la cola del joystick (guardamos info en struct infoJoys)
 
//Prototipo de la función bucle de ejecución del hilo del joystick
void Thjoys(void *argument);  
//Prototipo de la función bucle de ejecución del hilo del timer que gestiona rebotes
void timerRebo (void *argument);

uint8_t pulsadito = 0; 
uint8_t pulsadito_largo = 0;

//Creación de un struct mygpio para cada acción del joystick
mygpioJ joysR, joysL, joysU, joysD, joysM;
//Inicialización de un struct infoJoys, que será el tipo de dato que se envíe por la cola
infoJoys datosJoys;

uint32_t exec1 = 1U; //Argumento para la funcion callback del timer

//Prototipo de la función bucle de ejecución del hilo del joystick
void Thjoys(void *argument);  
//Prototipo de la función bucle de ejecución del hilo del timer que gestiona rebotes
void timerRebo (void *argument);

//Prototipo del Callback del timer de 1 seg que gestiona el tipo de pulsación
void Callback_timer1seg (void const *arg);
//Prototipo del Callback del timer que gestiona los rebotes
void Callback_timerRebo (void const *arg);

//Función inicialización del hilo del joystick
int Init_joys (void) {
	
	initJoys(); //Llamamos a la inicialización FÍSICA del joystick
	//Inicializamos la cola
	IdqueueJoys = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(infoJoys), NULL);
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_joys = osThreadNew(Thjoys, NULL, NULL);
  if (tid_joys == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thjoys (void *argument) {

  HAL_NVIC_EnableIRQ( EXTI15_10_IRQn );  //Activamos las interrupciones del joystick
	
	osStatus_t status; //Estado de las funciones que retornan
	
  while (1) {

		// Insert thread code here...
		status = osThreadFlagsWait(TIMER_REBO, osFlagsWaitAll, osWaitForever); //Espera hasta que el timer de los rebotes finaliza
		
		//El if comprueba que el PIN por el que llega la interrupción es en el que va a escribir
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)){ //RIGHT
			
			datosJoys.GPIO_PIN = GPIO_PIN_11;
			datosJoys.tipo_pulsacion = 0;
			osMessageQueuePut(IdqueueJoys, &datosJoys, 3, 0); //Indica el mensaje a escribir en la cola
		}
		
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)){ //LEFT
			
			datosJoys.GPIO_PIN = GPIO_PIN_14;
			datosJoys.tipo_pulsacion = 0;
			osMessageQueuePut(IdqueueJoys, &datosJoys, 3, 0); //Indica el mensaje a escribir en la cola
		}

		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){ //UP
			
			datosJoys.GPIO_PIN = GPIO_PIN_10;
			datosJoys.tipo_pulsacion = 0;
			osMessageQueuePut(IdqueueJoys, &datosJoys, 3, 0); //Indica el mensaje a escribir en la cola
		}

		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)){ //DOWN
			
			datosJoys.GPIO_PIN = GPIO_PIN_12;
			datosJoys.tipo_pulsacion = 0;
			osMessageQueuePut(IdqueueJoys, &datosJoys, 3, 0); //Indica el mensaje a escribir en la cola
		}
	
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){ //MIDDLE
									
			tid_tim1seg = osTimerNew((osTimerFunc_t)&Callback_timer1seg, osTimerOnce, &exec1, NULL);
			osTimerStart(tid_tim1seg, 1000U);

		}
			
			
		//De inicio el tick del sistema está configurado a 1 ms, 
		//si se desea cambiar su valor, sin necesidad de tocar el código es:
		/***RTX_Config.h -> System Configuration -> Kernel Tick Frequency[Hz]***/
		
    osThreadYield(); // suspend thread
  }
}


//Función Inicialización de las 5 acciones del Joystick
void initJoys(void){		
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
	
		//Inicialización RIGHT
		joysR.gpio.Mode = GPIO_MODE_IT_RISING;
		joysR.gpio.Pull = GPIO_PULLDOWN;
		joysR.gpio.Pin = GPIO_PIN_11;
		joysR.port = GPIOB;
	
	  HAL_GPIO_Init(joysR.port, &(joysR.gpio));

		//Inicialización LEFT
		joysL.gpio.Mode = GPIO_MODE_IT_RISING;
		joysL.gpio.Pull = GPIO_PULLDOWN;
		joysL.gpio.Pin = GPIO_PIN_14;
		joysL.port = GPIOE;
	
		HAL_GPIO_Init(joysL.port, &(joysL.gpio));
	
		//Inicialización UP
		joysU.gpio.Mode = GPIO_MODE_IT_RISING;
		joysU.gpio.Pull = GPIO_PULLDOWN;
		joysU.gpio.Pin = GPIO_PIN_10;
		joysU.port = GPIOB;
		
	  HAL_GPIO_Init(joysU.port, &(joysU.gpio));
	
		//Inicialización DOWN
		joysD.gpio.Mode = GPIO_MODE_IT_RISING;
		joysD.gpio.Pull = GPIO_PULLDOWN;
		joysD.gpio.Pin = GPIO_PIN_12;
		joysD.port = GPIOE;
		
	  HAL_GPIO_Init(joysD.port, &(joysD.gpio));
		
		//Inicialización MIDDLE
		joysM.gpio.Mode = GPIO_MODE_IT_RISING;
		joysM.gpio.Pull = GPIO_PULLDOWN;
		joysM.gpio.Pin = GPIO_PIN_15;
		joysM.port = GPIOE;
		
		HAL_GPIO_Init(joysM.port, &(joysM.gpio));
	}

//Implementación para gestionar interrupciones de las acciones del Joystick
void EXTI15_10_IRQHandler(void){
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11); //RIGHT
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14); //LEFT
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10); //UP
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12); //DOWN
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15); //MIDDLE
}
        
//Configuración del callback, la rutina que seguirá el programa cada vez que haya una interrupción
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){ 
		
	osThreadFlagsSet(tid_timerRebo, JOYS_pulsado); //Manda al timer la flag de que se ha activado el joystick 
																							 // para que ejecute la gestión de los rebotes
	
}


void Callback_timer1seg (void const *arg) {
	
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){ //MIDDLE
		
		datosJoys.GPIO_PIN = GPIO_PIN_15;
		datosJoys.tipo_pulsacion = 1;
		pulsadito_largo++;
	}
	else{
		
		datosJoys.GPIO_PIN = GPIO_PIN_15;
		datosJoys.tipo_pulsacion = 0;		
	}
	
	osMessageQueuePut(IdqueueJoys, &datosJoys, 3, 0); //Indica el mensaje a escribir en la cola

}


int Init_timerRebo (void) {
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_timerRebo = osThreadNew(timerRebo, NULL, NULL);
  if (tid_timerRebo == NULL) {
    return(-1);
  }
 
  return(0);
}


void timerRebo (void *argument){
  
  osStatus_t status; //Estado de las funciones que retornan
	
	osThreadFlagsWait(JOYS_pulsado, osFlagsWaitAll, osWaitForever);
	
  //Crea un timer one-shoot (que solo se ejecutará una vez, no periódicamente)
  uint32_t exec2 = 1U; //Argumento para la funcion callback del timer
  tid_timRebo = osTimerNew((osTimerFunc_t)&Callback_timerRebo, osTimerOnce, &exec1, NULL);
  if (tid_timRebo != NULL) {  //One-shot timer creado
    // Empieza el timer
    osTimerStart(tid_timRebo, 50U);
  }

  while(1){
		
		//Espera al flag que indica que se ha pulsado el joystick
    osThreadFlagsWait(JOYS_pulsado, osFlagsWaitAll, osWaitForever); 
		//Resetea el timer
    osTimerStart(tid_timRebo, 50U);
  }
}


void Callback_timerRebo (void const *arg) {
	
	osThreadFlagsSet(tid_joys, TIMER_REBO); //Manda al joystick la flag que indica que el timer ya ha finalizado
	pulsadito++;
	
}
