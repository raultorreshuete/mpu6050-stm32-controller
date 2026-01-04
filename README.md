# Controlador de Aceleración - STM32F429ZI
Sistema embebido diseñado para la detección de aceleración y monitorización de estados inerciales utilizando procesamiento en tiempo real. El proyecto enfoca su eficiencia en la gestión de datos y la respuesta inmediata ante eventos críticos.

### 🔧 Hardware e Interfaces
- ***Protocolo SPI*** Comunicación con el display LCD para la interfaz de usuario.
- ***Protocolo I2C:*** Gestión del acelerómetro MPU6050 para lectura de los ejes X, Y, Z y temperatura interna.
- ***Protocolo UART/USART:*** Comunicación bidireccional con PC (TeraTerm) para envío de comandos y recepción de telemetría.
- ***Salidas PWM:*** Activación de zumbador tras exceder umbrales de referencia en el eje Z.

### 🏗️ Arquitectura de Software
- ***Sistema Operativo:*** Basado en CMSIS-RTOS2 para la gestión de hilos independientes (concurrencia) y sincronización mediante colas de mensajes.
- ***Gestión de Datos:*** Uso de Buffer Circular para almacenar de forma eficiente las últimas 10 medidas de aceleración.
- ***Manejo de Interrupciones:*** Gestión de pulsaciones cortas y largas del joystick mediante interrupciones externas (EXTI).

### 🚀 Funcionalidades Clave
- Visualización en tiempo real de componentes de aceleración y temperatura.
- Detección y alerta de superación de valores de referencia mediante señales acústicas y visuales.
- Tres modos de trabajo definidos: Reposo, Activo y Programación/Depuración.

### 🛠️ Herramientas y Tecnología
- ***Entorno:*** Keil uVision 5.
- ***Librerías:*** STM32 HAL y CMSIS-RTOS2.
- ***Equipos:*** NUCLEO-F429ZI y MBED Application Board.

### 👥 Colaboradores
Proyecto académico desarrollado por Raúl Torres y Roberto Vila.
