

#define _VERSION_ "Temporizador OLED v2.0 NTC autoTemp\n"

/*
 * DESCRIPCION DEL PROYECTO
  Control temporizado, partiendo de un temporizador electromecanico canibalizado debido a su grandisma imprecision
  
*/


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DEFINICION DE CONSTANTES DEL PROGRAMA
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#define ON                    1     // 
#define OFF                   0     // 

#define LIMITE_ADC         1023 





/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        ALGUNAS DEFINICIONES PERSONALES PARA MI COMODIDAD AL ESCRIBIR CODIGO
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#define AND    &&
#define OR     ||
#define NOT     !
#define ANDbit  &
#define ORbit   |
#define XORbit  ^
#define NOTbit  ~

#define getBit(data,y)       ((data>>y) & 1)           // Obtener el valor  del bit (data.y)
#define setBit(data,y)       data |= (1 << y)          // Poner a 1 el bit (data.y) 
#define clearBit(data,y)     data &= ~(1 << y)         // Poner a 0 el bit (data.y)
#define togleBit(data,y)     data ^= (1 << y)          // Invertir el valor del bit (data.y)
#define togleByte(data)      data = ~data              // Invertir el valor del byte (data)

#define ISNAN(X) (!((X)==(X)))


/* Anular funciones serial para ahorrar memoria y dar velocidad (uso normal) / permitir (modo DEBUG) */

#define SERIAL_BAUD_RATE    115200
#define SERIAL_END          Serial.end
#define SERIAL_FLUSH        Serial.flush 

//#define DEBUG_PRINT_MODE
//#define NORMAL_PRINT_MODE

#ifdef DEBUG_PRINT_MODE 
  #define NORMAL_PRINT_MODE
  #define PRINT_DEBUG         Serial.print
  #define PRINTLN_DEBUG       Serial.println
#else
  #define PRINT_DEBUG         //
  #define PRINTLN_DEBUG       //
#endif


#ifdef NORMAL_PRINT_MODE 
  #define SERIAL_BEGIN        Serial.begin
  #define PRINT_VERSION       Serial.println
  #define PRINT               Serial.print
  #define PRINTLN             Serial.println
#else
  #define SERIAL_BEGIN        //
  #define PRINT_VERSION       //
  #define PRINT               //
  #define PRINTLN             //
#endif




//#ifdef DEBUG_PRINT_MODE 
//  #define SERIAL_BEGIN        Serial.begin
//  #define PRINT_VERSION       Serial.println
//  #define PRINT               Serial.print
//  #define PRINTLN             Serial.println
//  #define PRINT_DEBUG         Serial.print
//  #define PRINTLN_DEBUG       Serial.println
//#else
//  #define SERIAL_BEGIN        //
//  #define PRINT_VERSION       //
//  #define PRINT               //
//  #define PRINTLN             //
//  #define PRINT_DEBUG         //
//  #define PRINTLN_DEBUG       //
//#endif


//OLED
  //size 1, 21 caracteres maximo
  //size 2, 10 caracteres maximo
  //size 3, 7 caracteres maximo 
  //size 4, 5 caracteres maximo 
  //size 5, 4 caracteres maximo 
  //size 6, 3 caracteres maximo
