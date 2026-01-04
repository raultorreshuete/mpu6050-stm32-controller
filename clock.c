/**
SBM - PROYECTO
Raúl Torres Huete
Roberto Vila García
(M01M02J05J06B)
  */

#include "cmsis_os2.h"
#include "clock.h" //Llamada a su .h (suele contener structs, Flags y funciones importantes)


//Inicialización de un struct HoraCompleta
//Esta es la struct de variables globales que se utiliza en otros hilos (lcd y principal)
HoraCompleta hora;

//Prototipo de la función bucle de ejecución del hilo del reloj
void clock(void *argument);

//Prototipo del Callback del timer periódico de 1 seg
void Callback_timerHora (void const *arg);

osThreadId_t tid_clock; //Id del hilo clock
osTimerId_t tid_timHora;	//Id del timer de 1 seg

//Función inicialización del hilo del reloj
int Init_clock (void) {
	
	//Ponemos las 3 variables a 0 (reset de los valores)
	hora.horas = 0;
	hora.minutos = 0;
	hora.segundos = 0;
	
	//Creación e iniciación del hilo del reloj, asignándoselo a su Id y como parámetro su bucle de ejecución
  tid_clock = osThreadNew(clock, NULL, NULL);
  if (tid_clock == NULL) {
    return(-1);
  }
  return(0);
}

//Bucle de ejecución del hilo del reloj
void clock (void *argument) {

	//Inicializamos un timer periódico que se encarga de contar la hora
	tid_timHora = osTimerNew((osTimerFunc_t)&Callback_timerHora, osTimerPeriodic, NULL, NULL);

	//Damos la orden de comienzo al timer periódico y le decimos que sea de 1 seg
	osTimerStart(tid_timHora, 1000U);

	osThreadExit();	//Libera el hilo en memoria
}

//Callback del timer periódico de 1 seg, es la rutina que seguirá cada vez que finalice el timer
void Callback_timerHora (void const *arg) {
	
	//Sumamos segundos en cada ejecución
	hora.segundos++;

	//Si los segundos llegan a 60, se resetean y sumamos un minuto
	if (hora.segundos > 59) {
			hora.segundos = 0;
			hora.minutos++;
	}
	//Si los minutos llegan a 60, se resetean y sumamos una hora
	if (hora.minutos > 59) {
			hora.minutos = 0;
			hora.horas = (hora.horas + 1) % 24;
	}
	//Si las horas llegan a 24, se resetean
	if(hora.horas > 23){
		hora.horas = 0;
	}
			
		
}
