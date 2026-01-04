/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "principal.h"		//LLamamos al .h que contiene las funciones del Thjoystick
#include "clock.h"
#include "joystick.h"
#include "lcd.h"
#include "Mpu6050.h"
#include "com.h"
#include <stdio.h>
#include <string.h>  // Para usar memcpy



#define MSGQUEUE_OBJECTS 16

/*---------------------------------------------------------------------------------
 * Thread 'Principal': Hilo principal que se encarga de coordinar todos los demás
 *--------------------------------------------------------------------------------*/
 
osThreadId_t tid_principal;                        // thread id
osThreadId_t tid_principTramas;                        // thread id


uint8_t estado = 1;
uint8_t estadoDepuracion = 0;
uint8_t estadoProgramacion = 0;
uint8_t pulsacion = 1;

extern osMessageQueueId_t IdqueueJoys; //Declaración del id de la cola
extern infoJoys datosJoys;

extern osMessageQueueId_t IdqueueMpu6050; //Declaración del id de la cola
extern infoMPU datosSensor;

extern osMessageQueueId_t IdqueueLCD; //Declaración del id de la cola
extern infoLCD datosLCD;

extern HoraCompleta hora;
int8_t posicionHora = 0;
uint8_t valorHora = 0;
static void depurar(void);
static void ajustarHora(void);

int8_t posicionAccel = 0;
uint8_t valorAccel = 0;
static void ajustarAccel(void);


extern osMessageQueueId_t IdqueueComTrans; //Declaración del id de la cola de transmision
extern osMessageQueueId_t IdqueueComRecep; //Declaración del id de la cola de transmision


extern infoMensaje mensajeAEnviar;
extern infoMensaje mensajeRecibido;

static void programar(void);

void ThprincipTramas (void *argument);


void rellenarBufCircular(void);
static char bufferCircular[10][44];
static uint8_t punteroBuffer = 0;

extern osThreadId_t tid_led1, tid_led2, tid_led3;

int Init_principal (void) {
	
//	IdqueueJoys = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJECTS), NULL);
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_principal = osThreadNew(Thprincipal, NULL, NULL);
  if (tid_principal == NULL) {
    return(-1);
  }
	
//	tid_principTramas = osThreadNew(ThprincipTramas, NULL, NULL);
//  if (tid_principTramas == NULL) {
//    return(-1);
//  }
 
  return(0);
}
 
void Thprincipal (void *argument) {

	osStatus_t status; //Estado de las funciones que retornan
	
  while (1) {
		// Insert thread code here...
		switch(estado){
			case 1: //MODO REPOSO
				//recoge info de la cola  del joystick
				osMessageQueueGet(IdqueueJoys, &datosJoys, 3, 0);
				datosLCD.modo = 1;
				datosLCD.hora = hora.horas;
				datosLCD.minuto = hora.minutos;
				datosLCD.segundo = hora.segundos;
			
				//envía info a la cola del lcd
				osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
			
				if(datosJoys.tipo_pulsacion == 1){
					estado = 2;
					datosJoys.tipo_pulsacion = 0;
					datosJoys.GPIO_PIN = 0x00;
				}
				osDelay(500);
				break;
			
			case 2: //MODO ACTIVO
				//recoge info de la cola del acelerómetro
				osMessageQueueGet(IdqueueMpu6050, &datosSensor, 3, 0); 
			
				//recoge info de la cola  del joystick
				osMessageQueueGet(IdqueueJoys, &datosJoys, 3, 0); 
				datosLCD.modo = 2;
			
				if(datosSensor.accel_x > datosLCD.Ax_r){
						osThreadFlagsSet(tid_led1, LED1_ON);
				} else{
						osThreadFlagsSet(tid_led1, LED1_OFF);
				}
				
				if(datosSensor.accel_y > datosLCD.Ay_r){
						osThreadFlagsSet(tid_led2, LED2_ON);
				} else{
						osThreadFlagsSet(tid_led2, LED2_OFF);
				}
				
				if(datosSensor.accel_z > datosLCD.Az_r){
						osThreadFlagsSet(tid_led3, LED3_ON);
				} else{
						osThreadFlagsSet(tid_led3, LED3_OFF);
				}
				
				datosLCD.acceler_x = datosSensor.accel_x;
				datosLCD.acceler_y = datosSensor.accel_y;
				datosLCD.acceler_z = datosSensor.accel_z;
				datosLCD.temper = datosSensor.temperature;
				
				rellenarBufCircular();

				//envía info a la cola del lcd
				osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
				
				if(datosJoys.tipo_pulsacion == 1){
					estado = 3;
					datosJoys.tipo_pulsacion = 0;
					datosJoys.GPIO_PIN = 0x00;
					osThreadFlagsSet(tid_led1, LED1_OFF);
					osThreadFlagsSet(tid_led2, LED2_OFF);
					osThreadFlagsSet(tid_led3, LED3_OFF);				
				}
				osDelay(500);
			break;
			
			case 3: //MODO PROGRAMACIÓN/DEPURACIÓN
				//recoge info de la cola  del joystick
				osMessageQueueGet(IdqueueJoys, &datosJoys, 3, 0);
				datosLCD.modo = 3;
				datosLCD.hora = hora.horas;
				datosLCD.minuto = hora.minutos;
				datosLCD.segundo = hora.segundos;
				//envía info a la cola del lcd
				osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
				programar();
				depurar();
				osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
				if(datosJoys.tipo_pulsacion == 1){
					estado = 1;
					datosJoys.tipo_pulsacion = 0;
					datosJoys.GPIO_PIN = 0x00;
					posicionHora = 0;
				}
				osDelay(500);
			break;
		}
		//De inicio el tick del sistema está configurado a 1 ms, 
		//si se desea cambiar su valor, sin necesidad de tocar el código es:
		/***RTX_Config.h -> System Configuration -> Kernel Tick Frequency[Hz]***/
		
    osThreadYield(); // suspend thread
  }
}

void rellenarBufCircular(void){
	
	sprintf(bufferCircular[punteroBuffer], "%02d:%02d:%02d--Tm:%02.1f-Ax:%01.1f-Ay:%01.1f-Az:%01.1f", hora.horas, hora.minutos, hora.segundos, datosSensor.temperature, datosSensor.accel_x, datosSensor.accel_y, datosSensor.accel_z);
	punteroBuffer = (punteroBuffer == 9) ? 0 : (punteroBuffer + 1);
	
}

// sprintf( --- , "%02d:%02d:%02d--Tm:02.1f*-Ax:%01.1f-Ay:%01.1f-Az:%01.1f", datosLCD.hora, datosLCD.minuto, datosLCD.segundo);


static void depurar(void){

	datosLCD.estadoDepu = estadoDepuracion;
	osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);

	switch(estadoDepuracion){
		case 0: //ajuste de hora
			osMessageQueueGet(IdqueueJoys, &datosJoys, 3, 0);
			if(datosJoys.GPIO_PIN == GPIO_PIN_11){ //derecha
				posicionHora++;
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_14){ //izquierda
				posicionHora--;
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_10){ //arriba
				valorHora = 1; //Suma hora
				ajustarHora();
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_12){ //abajo
				valorHora = 2; //Resta hora
				ajustarHora();
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_15){ //centro
				estadoDepuracion = 1;
				posicionHora = 0;
				valorHora = 0;
			}
			datosJoys.GPIO_PIN = 0;
		break;
		case 1: //ajuste de Ax_r, Ay_r, Az_r
			osMessageQueueGet(IdqueueJoys, &datosJoys, 3, 0);
			if(datosJoys.GPIO_PIN == GPIO_PIN_11){ //derecha
				posicionAccel++;
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_14){ //izquierda
				posicionAccel--;
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_10){ //arriba
				valorAccel = 1; //Suma accel
				ajustarAccel();
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_12){ //abajo
				valorAccel = 2; //Resta accel
				ajustarAccel();
			}
			else if(datosJoys.GPIO_PIN == GPIO_PIN_15){ //centro
				estadoDepuracion = 0;
				posicionAccel = 0;
				valorAccel = 0;
			}
			datosJoys.GPIO_PIN = 0;
		break;
	}
}

static void ajustarHora(void){
	//Ajuste para que siempre esté la selección de la hora en un estado de los disponibles
	if(posicionHora > 5){
		posicionHora = 0;
	}
	if(posicionHora < 0){
		posicionHora = 5;
	}
	
	if(valorHora == 1){ //Suma
		switch(posicionHora){
			case 0: //decenas horas
				hora.horas = hora.horas + 10;
			break;
			case 1: //unidades horas
				hora.horas++;
			break;
			case 2: //decenas minutos
				hora.minutos = hora.minutos + 10;
			break;
			case 3: //unidades minutos
				hora.minutos++;
			break;
			case 4: //decenas segundos
				hora.segundos = hora.segundos + 10;
			break;
			case 5: //unidades segundos
				hora.segundos++;
			break;				
		}
		//Si los segundos llegan a 60, se resetean y sumamos un minuto
		if (hora.segundos > 59 && hora.minutos < 150) {
				hora.segundos = 59;
		}
		else if (hora.segundos > 150) {
				hora.segundos = 0;
		}
		//Si los minutos llegan a 60, se resetean y sumamos una hora
		if (hora.minutos > 59 && hora.minutos < 150) {
				hora.minutos = 59;
		}
		else if (hora.minutos > 150) {
				hora.minutos = 0;
		}
		//Si las horas llegan a 24, se resetean
		if(hora.horas > 23 && hora.horas < 50){
			hora.horas = 23;
		}
		else if(hora.horas > 50){
			hora.horas = 0;
		}
	}
	if(valorHora == 2){ //Resta
			switch(posicionHora){
			case 0: //decenas horas
				hora.horas = hora.horas - 10;
			break;
			case 1: //unidades horas
				hora.horas--;
			break;
			case 2: //decenas minutos
				hora.minutos = hora.minutos - 10;
			break;
			case 3: //unidades minutos
				hora.minutos--;
			break;
			case 4: //decenas segundos
				hora.segundos = hora.segundos - 10;
			break;
			case 5: //unidades segundos
				hora.segundos--;
			break;				
		}
		//Si los segundos llegan a 60, se resetean y sumamos un minuto
		if (hora.segundos > 59 && hora.minutos < 150) {
				hora.segundos = 59;
		}
		else if (hora.segundos > 150) {
				hora.segundos = 0;
		}
		//Si los minutos llegan a 60, se resetean y sumamos una hora
		if (hora.minutos > 59 && hora.minutos < 150) {
				hora.minutos = 59;
		}
		else if (hora.minutos > 150) {
				hora.minutos = 0;
		}
		//Si las horas llegan a 24, se resetean
		if(hora.horas > 23 && hora.horas < 50){
			hora.horas = 23;
		}
		else if(hora.horas > 50){
			hora.horas = 0;
		}
	}
}
	
static void ajustarAccel(void){
	//Ajuste para que siempre esté la selección de la hora en un estado de los disponibles
	if(posicionAccel > 8){
		posicionAccel = 0;
	}
	if(posicionAccel < 0){
		posicionAccel = 8;
	}
	
	if(valorAccel == 1){ //Suma
		switch(posicionAccel){
			case 0: //unidades Ax_r
				datosLCD.Ax_r = datosLCD.Ax_r + 1.0f;
			break;
			case 1: //decimales Ax_r
				datosLCD.Ax_r = datosLCD.Ax_r + 0.1f;
			break;
			case 2: //segundos decimales Ax_r
				datosLCD.Ax_r = datosLCD.Ax_r + 0.01f;
			break;
			case 3: //unidades Ay_r
				datosLCD.Ay_r = datosLCD.Ay_r + 1.0f;
			break;
			case 4: //decimales Ay_r
				datosLCD.Ay_r = datosLCD.Ay_r + 0.1f;
			break;
			case 5: //segundos decimales Ay_r
				datosLCD.Ay_r = datosLCD.Ay_r + 0.01f;
			break;
			case 6: //unidades Az_r
				datosLCD.Az_r = datosLCD.Az_r + 1.0f;
			break;
			case 7: //decimales Az_r
				datosLCD.Az_r = datosLCD.Az_r + 0.1f;
			break;
			case 8: //segundos decimales Az_r
				datosLCD.Az_r = datosLCD.Az_r + 0.01f;
			break;
		}			
	}
	if(valorAccel == 2){ //Resta
		switch(posicionAccel){
			case 0: //unidades Ax_r
				datosLCD.Ax_r = datosLCD.Ax_r - 1.0f;
			break;
			case 1: //decimales Ax_r
				datosLCD.Ax_r = datosLCD.Ax_r - 0.1f;
			break;
			case 2: //segundos decimales Ax_r
				datosLCD.Ax_r = datosLCD.Ax_r - 0.01f;
			break;
			case 3: //unidades Ay_r
				datosLCD.Ay_r = datosLCD.Ay_r - 1.0f;
			break;
			case 4: //decimales Ay_r
				datosLCD.Ay_r = datosLCD.Ay_r - 0.1f;
			break;
			case 5: //segundos decimales Ay_r
				datosLCD.Ay_r = datosLCD.Ay_r - 0.01f;
			break;
			case 6: //unidades Az_r
				datosLCD.Az_r = datosLCD.Az_r - 1.0f;
			break;
			case 7: //decimales Az_r
				datosLCD.Az_r = datosLCD.Az_r - 0.1f;
			break;
			case 8: //segundos decimales Az_r
				datosLCD.Az_r = datosLCD.Az_r - 0.01f;
			break;
		}	
	}
}

void ThprincipTramas (void *argument) {

	osStatus_t status; //Estado de las funciones que retornan
	
  while (1) {
		//programar();
    osThreadYield(); // suspend thread
  }
}


static void programar(void){

	osMessageQueueGet(IdqueueComRecep, &mensajeAEnviar, 3, 0);
	
	static char infoPayloadHora[9];
	static char infoPayloadAx[4];
	static char infoPayloadAy[4];
	static char infoPayloadAz[4];
	
	static uint8_t horaProgramada;
	static uint8_t minutoProgramado;
	static uint8_t segundoProgramado;
	static float Axprogramado;
	static float Ayprogramado;
	static float Azprogramado;
	
	switch(mensajeAEnviar.CMD){
		case 0x20: //programacion de hora
	
			memcpy(infoPayloadHora, mensajeAEnviar.Payload, sizeof(infoPayloadHora));

			int scanhora = sscanf(infoPayloadHora, "%d:%d:%d", &horaProgramada, &minutoProgramado, &segundoProgramado);
		
			if(scanhora == 3){
				hora.horas = horaProgramada;
				hora.minutos = minutoProgramado;
				hora.segundos = segundoProgramado;
				
				datosLCD.hora = hora.horas;
				datosLCD.minuto = hora.minutos;
				datosLCD.segundo = hora.segundos;
			}
			
			osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
			
			mensajeRecibido.CMD = ~mensajeAEnviar.CMD;
			mensajeRecibido.LENGTH = mensajeAEnviar.LENGTH;
			
			int tam_pequenoH = sizeof(infoPayloadHora);
			int tam_grandeH = sizeof(mensajeRecibido.Payload);

			// Rellenar el arreglo grande
			for (int i = 0; i < tam_grandeH; i++) {
					if (i < tam_pequenoH * (tam_grandeH / tam_pequenoH)) {
							// Copiar elementos del arreglo pequeño repetidamente
							mensajeRecibido.Payload[i] = infoPayloadHora[i % tam_pequenoH];
					} else {
							// Rellenar con ceros
							mensajeRecibido.Payload[i] = 0;
					}
			}
			
			osMessageQueuePut(IdqueueComTrans, &mensajeRecibido, 3, 0);		
		
//			datosLCD.hora = mensajeAEnviar.Payload[0]/**10 + mensajeAEnviar.Payload[1]*/;
//			datosLCD.minuto = mensajeAEnviar.Payload[3]/**10 + mensajeAEnviar.Payload[4]*/;
//			datosLCD.segundo = mensajeAEnviar.Payload[6]/**10 + mensajeAEnviar.Payload[7]*/;
		
		break;
		case 0x25: //programacion de Ax_r
			
			memcpy(infoPayloadAx, mensajeAEnviar.Payload, sizeof(infoPayloadAx));

			int scanAx = sscanf(infoPayloadAx, "%f", &Axprogramado);
		
			if(scanAx == 1){
				datosLCD.Ax_r = Axprogramado;
			}
			osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
			
			mensajeRecibido.CMD = ~mensajeAEnviar.CMD;
			mensajeRecibido.LENGTH = mensajeAEnviar.LENGTH;
			
			int tam_pequenoAx = sizeof(infoPayloadAx);
			int tam_grandeAx = sizeof(mensajeRecibido.Payload);

			// Rellenar el arreglo grande
			for (int i = 0; i < tam_grandeAx; i++) {
					if (i < tam_pequenoAx * (tam_grandeAx / tam_pequenoAx)) {
							// Copiar elementos del arreglo pequeño repetidamente
							mensajeRecibido.Payload[i] = infoPayloadAx[i % tam_pequenoAx];
					} else {
							// Rellenar con ceros
							mensajeRecibido.Payload[i] = 0;
					}
			}
			
			osMessageQueuePut(IdqueueComTrans, &mensajeRecibido, 3, 0);		


			//datosLCD.Ax_r = mensajeAEnviar.Payload[1]*1.0f + mensajeAEnviar.Payload[3]/10.0f + mensajeAEnviar.Payload[4]/100.0f;
		break;
		case 0x26: //programacion de Ay_r
			
			memcpy(infoPayloadAy, mensajeAEnviar.Payload, sizeof(infoPayloadAy));

			int scanAy = sscanf(infoPayloadAy, "%f", &Ayprogramado);
		
			if(scanAy == 1){
				datosLCD.Ay_r = Ayprogramado;
			}
			
			osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);	
			
			mensajeRecibido.CMD = ~mensajeAEnviar.CMD;
			mensajeRecibido.LENGTH = mensajeAEnviar.LENGTH;
			
			int tam_pequenoAy = sizeof(infoPayloadAy);
			int tam_grandeAy = sizeof(mensajeRecibido.Payload);

			// Rellenar el arreglo grande
			for (int i = 0; i < tam_grandeAy; i++) {
					if (i < tam_pequenoAy * (tam_grandeAy / tam_pequenoAy)) {
							// Copiar elementos del arreglo pequeño repetidamente
							mensajeRecibido.Payload[i] = infoPayloadAy[i % tam_pequenoAy];
					} else {
							// Rellenar con ceros
							mensajeRecibido.Payload[i] = 0;
					}
			}
			
			osMessageQueuePut(IdqueueComTrans, &mensajeRecibido, 3, 0);		
		
			//datosLCD.Ay_r = mensajeAEnviar.Payload[1]*1.0f + mensajeAEnviar.Payload[3]/10.0f + mensajeAEnviar.Payload[4]/100.0f;
		break;
		case 0x27: //programacion de Az_r
		
			memcpy(infoPayloadAz, mensajeAEnviar.Payload, sizeof(infoPayloadAz));

			int scanAz = sscanf(infoPayloadAz, "%f", &Azprogramado);
		
			if(scanAz == 1){
				datosLCD.Az_r = Azprogramado;
			}
			osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);			
			
			mensajeRecibido.CMD = ~mensajeAEnviar.CMD;
			mensajeRecibido.LENGTH = mensajeAEnviar.LENGTH;
			
			int tam_pequenoAz = sizeof(infoPayloadAz);
			int tam_grandeAz = sizeof(mensajeRecibido.Payload);

			// Rellenar el arreglo grande
			for (int i = 0; i < tam_grandeAz; i++) {
					if (i < tam_pequenoAz * (tam_grandeAz / tam_pequenoAz)) {
							// Copiar elementos del arreglo pequeño repetidamente
							mensajeRecibido.Payload[i] = infoPayloadAz[i % tam_pequenoAz];
					} else {
							// Rellenar con ceros
							mensajeRecibido.Payload[i] = 0;
					}
			}
			
			osMessageQueuePut(IdqueueComTrans, &mensajeRecibido, 3, 0);			

		
			//datosLCD.Az_r = mensajeAEnviar.Payload[1]*1.0f + mensajeAEnviar.Payload[3]/10.0f + mensajeAEnviar.Payload[4]/100.0f;
		break;
		case 0x55: //entregar todas medidas (buffer circular)
			for(uint8_t i = 0; i < 10; i++){
				if(bufferCircular[i] != NULL){
					snprintf(mensajeRecibido.Payload, sizeof(mensajeRecibido.Payload), "%s", bufferCircular[i]);
					
					mensajeRecibido.LENGTH = 0x2B;
					mensajeRecibido.CMD = 0x55;
					
					osMessageQueuePut(IdqueueComTrans, &mensajeRecibido, 3, 0);			
				}			
			} 
		break;
		case 0x60: //borrar medidas (buffer circular)
			for(uint8_t i = 0; i < 10; i++){
				memset(bufferCircular[i], 0x00, sizeof(bufferCircular[i]));
			} 
		break;
	}
		osMessageQueuePut(IdqueueLCD, &datosLCD, 3, 0);
		mensajeAEnviar.CMD = 0x00;
}
