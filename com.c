/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "com.h"		//LLamamos al .h que contiene las funciones del Thjoystick

#include  CMSIS_device_header

#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include <stdio.h>

#define MSGQUEUE_OBJECTS 16

//CONSTANTES IMPORTANTES
	//recepcion
static const char cambioHora = 0x20;
static const char Ax_r = 0x25;
static const char Ay_r = 0x26;
static const char Az_r = 0x27;
static const char pedirTodasMedidas = 0x55;
static const char borrarRecep = 0x60;

	//transmisión
static const char nuevaHora = 0xDF;
static const char nuevaAx_r = 0xDA;
static const char nuevaAy_r = 0xD9;
static const char nuevaAz_r = 0xD8;
static const char enviarTodasMedidas = 0xAF;
static const char borrarTrans = 0x9F;

static const char SOH = 0x01;
static const char EOT = 0xFE;

//TEST
osThreadId_t tid_ThTest;
void Test (void *argument);

/*----------------------------------------------------------------------------------------
 * Thread 'Thcom': Hilo que se encarga del envío de datos del pc al principal y viceversa
 *---------------------------------------------------------------------------------------*/
 
osThreadId_t tid_comRecep;                        // thread id
osThreadId_t tid_comTrans;                        // thread id

uint8_t estadoRecep = 1;
 
void ThCom (void *argument);                   // thread function

osMessageQueueId_t IdqueueComRecep; //Declaración del id de la cola de recepcion
osMessageQueueId_t IdqueueComTrans; //Declaración del id de la cola de transmision
infoMensaje mensajeAEnviar;
infoMensaje mensajeRecibido;

extern ARM_DRIVER_USART Driver_USART3;/* USART Driver */
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;

void myUSART_callback(uint32_t event);

static int8_t Init_USART(void);

static char datoTrama;
static char tramaRecibida[44];
static char tramaAEnviar[44];
static uint8_t posicionTrama = 0;

static void leerTrama(void);
static void enviarTrama(void);

int Init_Com (void) {
	
	Init_USART();

	IdqueueComRecep = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(infoMensaje), NULL);
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del ThComRecep,
	// le pasamos como parámetros la función que ejecutará el hilo 
  tid_comRecep = osThreadNew(ThComRecep, NULL, NULL);
  if (tid_comRecep == NULL) {
    return(-1);
  }
	
	IdqueueComTrans = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(infoMensaje), NULL);
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del ThComTrans,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_comTrans = osThreadNew(ThComTrans, NULL, NULL);
  if (tid_comTrans == NULL) {
    return(-1);
  }
 
  return(0);
}

//Función bucle del hilo de recepción del PC (TeraTerm)
void ThComRecep(void *argument) {
		
    while(1) {

			if(estadoRecep == 1){
				
				USARTdrv->Receive(&datoTrama, 1);
				osThreadFlagsWait(TRAMA_RECIBIDA, osFlagsWaitAll, osWaitForever);

					if(datoTrama == SOH){
						tramaRecibida[posicionTrama] = datoTrama;
						estadoRecep = 2;
					}		
				} else if(estadoRecep == 2){
					leerTrama();
				}

				osThreadYield();
    }
}

//Función bucle del hilo de transmisión al módulo principal
void ThComTrans(void *argument) {
	
    while(1) {
			
				osMessageQueueGet(IdqueueComTrans, &mensajeRecibido, NULL, osWaitForever);
				enviarTrama();
			
				USARTdrv->Send(tramaAEnviar, mensajeRecibido.LENGTH);
				osThreadFlagsWait(TRAMA_TRANSMITIDA, osFlagsWaitAll, osWaitForever);
			
				osThreadYield();
    }
}


//Función de inicialización del USART3
static int8_t Init_USART(void){
	// Inicializamos el USART driver, con su callbacsk
	if(USARTdrv->Initialize(myUSART_callback) != ARM_DRIVER_OK ||
    // Activamos el driver (lo enciende)
    USARTdrv->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK ||
    //Configuramos el USART a 4800 Bits/sec
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, 
											115200) != ARM_DRIVER_OK |
     
    //Habilitamos su recepción y tranmisión
    USARTdrv->Control (ARM_USART_CONTROL_TX, 1) != ARM_DRIVER_OK |
    USARTdrv->Control (ARM_USART_CONTROL_RX, 1) != ARM_DRIVER_OK){
			return(-1);
		}
		return 0;
}

//Función Callback del USART3
void myUSART_callback(uint32_t event){
	
	switch (event){
		case ARM_USART_EVENT_SEND_COMPLETE:
			osThreadFlagsSet(tid_comTrans, TRAMA_TRANSMITIDA);
		break;
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
			osThreadFlagsSet(tid_comRecep, TRAMA_RECIBIDA);
		break;
	}	
}


//Función que lee las tramas indicadas con SOH, guardasu info para la posterior transmision
// y comprueba la integridad de la trama)
static void leerTrama(void){
	uint8_t i = 0; //posicion del payload dentro de lo que env?o
	uint8_t j = 3; //el payload siempre empieza en la posici?n 3: 0inicio, 1comando, 2longitud, 3iniciopayload
	
	while(datoTrama != EOT || posicionTrama > 11){
		posicionTrama++;
		USARTdrv->Receive(&datoTrama, 1);
		osThreadFlagsWait(TRAMA_RECIBIDA, osFlagsWaitAll, osWaitForever);
		tramaRecibida[posicionTrama] = datoTrama;
	}
	
	if(posicionTrama < 11 | posicionTrama > 2){		
		mensajeAEnviar.CMD = tramaRecibida[1];
		mensajeAEnviar.LENGTH = tramaRecibida[2];
		
		while(tramaRecibida[j] != EOT){
			mensajeAEnviar.Payload[i] = tramaRecibida[j];
			i++;
			j++;
		}
		
		if(mensajeAEnviar.LENGTH == posicionTrama + 1 && 
			(mensajeAEnviar.CMD == cambioHora |
			 mensajeAEnviar.CMD == Ax_r |
			 mensajeAEnviar.CMD == Ay_r |
			 mensajeAEnviar.CMD == Az_r |
			 mensajeAEnviar.CMD == pedirTodasMedidas |
			 mensajeAEnviar.CMD == borrarRecep )){
				osMessageQueuePut(IdqueueComRecep, &mensajeAEnviar, NULL, NULL);
				estadoRecep = 1;

		}
		
	}
	posicionTrama = 0;
}

//Función que monta la trama que se va a enviar al módulo principal
static void enviarTrama(void){
	uint8_t sizeTrama = mensajeRecibido.LENGTH;
	uint8_t sizePayload = mensajeRecibido.LENGTH-4;
	uint8_t j = 3;
	uint8_t i = 0;
	
	tramaAEnviar[0] = SOH;
	if(mensajeRecibido.CMD == pedirTodasMedidas){
		tramaAEnviar[1] = enviarTodasMedidas;
	}else{
		tramaAEnviar[1] = ~mensajeRecibido.CMD; //complemento a 1 del comando recibido
	}
	tramaAEnviar[2] = mensajeRecibido.LENGTH;
	while(i < sizePayload){
		tramaAEnviar[j] = mensajeRecibido.Payload[i];
		j++;
		i++;
	}
	
	tramaAEnviar[sizeTrama - 1] = EOT;
}


////TEST
//int Init_Test(void) {
// 
//  tid_ThTest = osThreadNew(Test, NULL, NULL);
//  if (tid_ThTest == NULL) {
//    return(-1);
//  }	
//  return(0);
//}
// 
//void Test (void *argument) {
//	infoMensaje tramaPrueba;
//	tramaPrueba.CMD = cambioHora;
//	tramaPrueba.LENGTH = 0x0C;
//	tramaPrueba.Payload[0] = 0x31;
//	tramaPrueba.Payload[1] = 0x35;
//	tramaPrueba.Payload[2] = 0x3A;
//	tramaPrueba.Payload[3] = 0x32;
//	tramaPrueba.Payload[4] = 0x30;
//	tramaPrueba.Payload[5] = 0x3A;
//	tramaPrueba.Payload[6] = 0x33;
//	tramaPrueba.Payload[7] = 0x34;
//	osDelay(1000);
//	osMessageQueuePut(IdqueueComTrans, &tramaPrueba, NULL, NULL);
//	
//	
//  while (1) {
//		osThreadYield();
//  }
//}

