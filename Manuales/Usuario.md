# **MANUAL DE USUARIO - PRACTICA 1**

**
GRUPO 15

## Descripcion

**
Se solicita crear un prototipo para un dispositivo interactivo que está formado por un display, compuesto por dos matrices LED de 8x8, y algunos botones para interactuar con el dispositivo. El prototipo debe ser capaz de presentar un mensaje de forma dinámica, esto es con un efecto de desplazamiento contínuo en cierta dirección. También debe proveer de ciertos mecanismos para poder iniciar una partida de un juego de un pequeño avion cuya meta es destruir una serie de objetivos y permitir al usuario interactuar con el juego más allá de solamente jugarlo.

**

## Diseño

**
Se decidió que el prototipo sería una simulación realizada en Proteus y se brindó un diseño preliminar el cuál muestra las dos matrices organizadas de forma horizontal, un conjunto de botones: IZQUIERDA, DERECHA, DIRECCION y CONFIGURACION y dos potenciómetros. La Figura 1 muestra el diseño descrito. Como característica especial se considerará manipular una matriz sin driver y otra con driver (MAX7219).

![](./imagenes/matricesYBotones)
**

## Funcionamiento del Juego

**
**INICIO:**
Al iniciar el juego se muestra un mensaje que muestra los siguientes datos:

 - Número de práctica
 - Número de Grupo
 - Sección de laboratorio

El mensaje se muestra de la siguiente manera:
![](./imagenes/mensaje.jpg)


**Cambio direccion Mensaje:**
La dirección inicial del mensaje será de derecha a izquierda. 

El boton de dirección deberá controlar la dirección del mensaje.

Y para realizar estos movimientos se utiliza el siguiente boton:
 ![](./imagenes/boton_direccion.jpg)

**Potenciometros:**
Los potenciometro se utiliza para poder hacer cambios en la velocidad en la que se recorre el mensaje y tambien se utiliza para poder aumentar o disminuir las vidas del jugador. 
![](./imagenes/potenciometros.jpg)
 **Como iniciar el juego:**
 Para poder iniciar el juego se debe presionar el boton IZQUIERDA durante el menu principal y empezará el juego. 
![](./imagenes/menuConfiguracion.jpg)
![](./imagenes/boton_izquierdo.jpg)

**Como jugar:**


**Pausa y configuracion**


**Estadisticas**