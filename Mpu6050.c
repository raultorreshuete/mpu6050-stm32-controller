/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Mpu6050.h"		//LLamamos al .h que contiene las funciones del Thjoystick
#include "clock.h"		

#include  CMSIS_device_header

#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include <stdio.h>

#define MSGQUEUE_OBJECTS 16

/*----------------------------------------------------------------------------
 *   			Thread 'Mpu6050': Hilo que se encarga del acelerómetro
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_mpu;                        // thread id

uint32_t exec2 = 1U; //Argumento para la funcion callback del timer

osTimerId_t tid_timPeriod1s;														 // id del timer

uint32_t status; //Estado de las funciones que retornan

infoMPU datosSensor;
uint8_t lecturaSensor[8];
uint8_t direccion[2] = {0x6B, 0x00}; //Despierta al sensor
static const uint8_t addressAccel = 0x3B;
static const double sensibilidadSensorAccel = 16384.0;

extern HoraCompleta hora;
 
void ThMpu6050 (void *argument);                   // thread function

extern ARM_DRIVER_I2C Driver_I2C1;
#define I2C1       (&Driver_I2C1)

osMessageQueueId_t IdqueueMpu6050; //Declaración del id de la cola

I2C_HandleTypeDef hi2c1;  // Ensure the I2C handle is defined in your project

// Queue handle
static osMessageQueueId_t idqueueMpu;
/************************************/

int Init_Mpu6050 (void) {
	
	IdqueueMpu6050 = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(infoMPU), NULL);
	
	//Creamos e iniciamos un nuevo hilo que asignamos al identificador del Thled1,
	// le pasamos como parámetros la función que ejecutará el hilo y la información del gpio del led creado como puntero void
  tid_mpu = osThreadNew(ThMpu6050, NULL, NULL);
  if (tid_mpu == NULL) {
    return(-1);
  }
 
  return(0);
}

static void I2C_Init(void){
	  /* Initialize and configure I2C */
  I2C1->Initialize  (NULL);
  I2C1->PowerControl(ARM_POWER_FULL);
  I2C1->Control     (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
  I2C1->Control     (ARM_I2C_BUS_CLEAR, 0);
	
	I2C1->MasterTransmit(SENSOR_ADDRESS, direccion, 2, false);
	while(I2C1->GetStatus().busy);
	
}

// Read multiple bytes from the MPU6050
static void I2C_Read(void) {
    
	I2C1->MasterTransmit(SENSOR_ADDRESS, &addressAccel, 1, true);
	while(I2C1->GetStatus().busy);
	
	I2C1->MasterReceive(SENSOR_ADDRESS, lecturaSensor, 8, false);
	while(I2C1->GetStatus().busy);
}

// Thread function
void ThMpu6050(void *argument) {
	
		tid_timPeriod1s = osTimerNew((osTimerFunc_t)&Callback_timerPeriod1seg, osTimerPeriodic, &exec2, NULL); //Inicialización timer periódico
		osTimerStart(tid_timPeriod1s, 1000U);
	
		I2C_Init();

    while(1) {
			
				status = osThreadFlagsWait(TIM_PERIODICO, osFlagsWaitAll, osWaitForever);
			
        // Read acceleration data
				I2C_Read();
			
				int16_t accele_x = (lecturaSensor[0] << 8) | lecturaSensor[1];
				int16_t accele_y = (lecturaSensor[2] << 8) | lecturaSensor[3];
				int16_t accele_z = (lecturaSensor[4] << 8) | lecturaSensor[5];
			
				datosSensor.accel_x = (accele_x / sensibilidadSensorAccel);
				datosSensor.accel_y = (accele_y / sensibilidadSensorAccel);
				datosSensor.accel_z = (accele_z / sensibilidadSensorAccel);
			
				int16_t temp = (lecturaSensor[6] << 8) | lecturaSensor[7];
				
				datosSensor.temperature = (temp / 340.0) + 36.53;
			
				osMessageQueuePut(IdqueueMpu6050, &datosSensor, 3, 0);
			
				osThreadYield();
    }
}

void Callback_timerPeriod1seg (void const *arg) {
	
		osThreadFlagsSet(tid_mpu, TIM_PERIODICO);
}
