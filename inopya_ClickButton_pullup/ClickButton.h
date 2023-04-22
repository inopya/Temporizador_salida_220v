/*
//   Libreria ClickButton
//
//   AUTOR:    Eulogio Lopez Cayuela
//   version:  1.0   Fecha:    14/10/2018 
//   version:  1.1   Fecha:    26/03/2020 
//             - Modificada deteccion del doble click 
//             - y añadido metodo para poder controlar el tiempo entre clicks (en las dobles pulsaciones)
//
//             https://github.com/inopya
*/

 
#ifndef CLICKBUTTON_H
#define CLICKBUTTON_H
 
//Cambia Arduino.h por Wprogram.h si usas una version antigua de la IDE.
#include <Arduino.h> //Permite utilizar los comandos de Arduino
 
class ClickButton //Definicion de la clase
{
  private:
    	/* 
	 *   podemos definir variables que se usen solo a nivel interno 
     	 *   y que no son accesibles por el usuario desde el programa 
	 */
	
    	uint8_t _PIN_entrada;                   // el numero del PIN que se usara
    	bool _estadoActual;                	// indica la lectura real del pulsador
    	uint8_t _estadoAnterior;              	// informa de la lectura anterior del pulsador
    	uint32_t _inicioPulsacion;       	// controla el momento en que se pulsa el boton
	uint32_t _finPulsacion;       		// controla el momento en que se pulsa el boton
	uint16_t _tiempoPulsacionNormal;  	// tiempo minimo que se debe mantener la pulsacion para ser pulsacion normal
    	uint16_t _tiempoPulsacionLarga;  	// tiempo minimo que se debe mantener la pulsacion para ser pulsacion larga
	uint8_t _antirrebotes;			// numero de ms para descartar si es un "rebote"
	uint16_t _intervaloClicks;
	bool _FLAG_PULLUP = false;	 	// control de pullup
	void init(uint8_t); 			// establecer el pin como entrada
	//void reset(); 			// reiniciar valores del pulsador
    
    
  public:
    	/* Constructores de la clase */
    	//--- Solo con numero de pin y resto de parametros por defecto
    	ClickButton(uint8_t, bool =false);
	
	//--- Completa: pin, tiempo antirrebotes, tiempo pulsacion corta, tiempo pulsacion larga  
	ClickButton(uint8_t, uint8_t, uint16_t, uint16_t, bool =false);
    	//--- Funcion Read: revisa el estado de un pulsador/boton y decide...
    	uint8_t Read();
	//--- Funcion DobleClick: modifica el tiempo entre dos clicks
	void DobleClick(uint16_t);	
	//--- reiniciar valores del pulsador	
	void reset(); 								
};
 
#endif
