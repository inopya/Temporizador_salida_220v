/*
 * DESCRIPCION DEL PROYECTO
  Control temporizado, partiendo de un temporizador electromecanico canibalizado debido a su grandisma imprecision
  
*/


#define _VERSION_ "Temporizador 24h en pantalla OLED v1.0\n"




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        ALGUNAS DEFINICIONES PERSONALES PARA MI COMODIDAD AL ESCRIBIR CODIGO
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/


#define RELE_ON               1     
#define RELE_OFF              0   



// FORMA COMODA DE ACTIVAR Y DESACTIVAR LAS FUNCIONES SERIAL

/* descomentar para permitir funciones serial (modo DEBUG) y comentar el bloque inferior */
//#define SERIAL_BEGIN  Serial.begin
//#define PRINTLN  Serial.println
//#define PRINT  Serial.print

/* Anular funciones serial para ahorrar memoria y dar velocidad (uso normal) Comentar el bloque superior */
#define SERIAL_BEGIN  //Serial.begin
#define PRINTLN  //Serial.println
#define PRINT  //Serial.print

        
