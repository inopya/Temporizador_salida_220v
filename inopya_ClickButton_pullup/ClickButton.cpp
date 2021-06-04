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

#include "ClickButton.h"
 
/* definicion del constructor de la clase solo con pin */
ClickButton::ClickButton (uint8_t pin, bool pullup=false) 
{
	_antirrebotes = 30;            		// tiempo para evitar rebotes en pulsadores mecanicos
	_tiempoPulsacionNormal = 100;   	// tiempo minimo necesario para detectar una pulsacion corta
	_tiempoPulsacionLarga = 380;   		// tiempo minimo necesario para detectar una pulsacion larga
	_intervaloClicks = 110;      		// tiempo max entre clicks para que se detecten como doble click
	_FLAG_PULLUP = pullup;
	init(pin);
}

/* definicion del constructor de la clase con sobrecarga */
ClickButton::ClickButton (uint8_t pin, uint8_t antirrebotes, uint16_t corta, uint16_t larga , bool pullup=false) 
{
	_antirrebotes = antirrebotes;      	// tiempo para evitar rebotes en pulsadores mecanicos
	_tiempoPulsacionNormal = corta;		// tiempo minimo necesario para detectar una pulsacion corta
	_tiempoPulsacionLarga = larga; 		// tiempo minimo necesario para detectar una pulsacion larga
	_intervaloClicks = 110;             // tiempo max entre clicks para que se detecten como doble click
	_FLAG_PULLUP = pullup;
	init(pin);
}


void ClickButton::init(uint8_t pin) 
{
	_PIN_entrada = pin;	
	if(_FLAG_PULLUP){
		pinMode(_PIN_entrada, INPUT_PULLUP);
	}
	else{
		pinMode(_PIN_entrada, INPUT);
	}
	reset(); 	
}
 
void ClickButton::reset() 
{
	_estadoActual = 0; 
	_estadoAnterior = 0;
	_inicioPulsacion = 0;	
	_finPulsacion = 0;
}


void ClickButton::DobleClick(uint16_t intervaloDobleClick)
{
	_intervaloClicks = intervaloDobleClick; 
}


uint8_t ClickButton::Read()   
{
	uint32_t _momento_actual = millis(); 
	
	_estadoActual = digitalRead(_PIN_entrada);
	if(_FLAG_PULLUP){
		_estadoActual  = !_estadoActual;
	}
	
	/* NO existe pulsacion actual ni nada en curso*/
	if(_estadoActual == false && _estadoAnterior == 0){
		return 0;      
	}

	/* EXISTE pulsacion actual y es la primera*/
	if(_estadoActual == true && _estadoAnterior == 0){
		_estadoAnterior  = 1;
		_inicioPulsacion = _momento_actual;
		return 0;      
	}
	
	/* EXISTE pulsacion actual pero es el fin de una mantenida...*/
	if(_estadoActual == false && _estadoAnterior == 4){
		reset();
		return 0;  //retornar otra cosa si queremos aprovechar este tipo de pulsacion extra larga    
	}

	/* Ha TERMINADO la primera pulsacion (o un rebote)*/
	if(_estadoActual == false && _estadoAnterior == 1){
		_finPulsacion = _momento_actual; 
		/* Detectar y descartar REBOTES */
		if ((_finPulsacion-_inicioPulsacion)<_antirrebotes){
			//_estadoAnterior  = 0;  //se hace 0 en el reset()
			reset();
			return 0;
		} 
		/* Confirmada pulsacion LARGA */
		else if((_finPulsacion-_inicioPulsacion)>_tiempoPulsacionLarga){
			reset(); 
			return 3;
		}
		/* Detectando pulsacion CORTA o DOBLE - parte 1*/
		else if((_finPulsacion-_inicioPulsacion)>_tiempoPulsacionNormal){
			//estamos en camino de una pulsacion buena: sea corta o doble...
			_estadoAnterior  = 2;  //control de doble click
			_finPulsacion = _momento_actual;
			return 0;
		}
	}
	
	/* Detectando pulsacion CORTA o DOBLE - parte 2*/
	if(_estadoActual == true && _estadoAnterior == 2 && (_momento_actual-_finPulsacion)<_intervaloClicks){
		//es posible que vayamos a por el segundo click
		_estadoAnterior  = 3;
		_inicioPulsacion = _momento_actual;
		return 0;
	}
	/* Confirmada pulsacion CORTA */
	if(_estadoActual == false && _estadoAnterior == 2 && (_momento_actual-_finPulsacion)>_intervaloClicks){
		//no, es solo una pulsacion simple
		reset();
		return 1;
	}
	/* Confirmada pulsacion DOBLE */
	if(_estadoActual == false && _estadoAnterior >= 3 && (_momento_actual-_inicioPulsacion)>_antirrebotes){
		//sí, es una pulsacion doble
		reset();
		return 2;
	}

	//en proceso. deteccion de pulsacion mantenida
	if( _estadoActual == true && (_estadoAnterior == 1 || _estadoAnterior == 4) ){
		if((_momento_actual-_inicioPulsacion)>_tiempoPulsacionLarga*5){ 
			_estadoAnterior = 4;
			return 9; 
		}
	}  

		
	return 0;
}

