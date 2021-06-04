
/*
#       _\|/_   A ver..., ¿que tenemos por aqui?
#       (O-O)        
# ---oOO-(_)-OOo---------------------------------
 
 
##########################################################
# ****************************************************** #
# *            ARDUINO PARA PRINCIPIANTES              * #
# *    EJEMPLO DE USO DE LA LIBRERIA Pulsador          * #
# *          Autor:  Eulogio López Cayuela             * #
# *                                                    * #
# *       Versión v1.0      Fecha: 14/10/2018          * #
# ****************************************************** #
##########################################################
*/

/*
 # NOTAS DE LA VERSION
  Ejemplo de uso para la libreria ClickButton
  https://github.com/inopya
  
  Esta libreria permite el control de pulsadores en los que podremos detectar tres tipos de eventos 
  mediante el metodo Read() de los objetos ClickButton.
  Disponemos de varios parametros de configuracion:
  - uint8_t pin: el numero de pin en el que esta conectado nuestro pulsador
  - uint8_t antirrebotes: tiempo (ms) minimo para que una pulsacion no sea despreciada como un ruido/rebote del pulsador
  - uint16_t corta: tiempo (ms) para que una pulsacion sea considerada pulsacion corta 
  - uint16_t larga: tiempo (ms) para que una pulsacion sea considerada pulsacion larga 
  
  # Este valor es accesible explicitamente mediante su metodo DobleClick(uint16_t doble)**
    En caso contrario se usa el valor por defecto de 110 ms, tanto para instancias creadas con numero de pin, como 
	las que se creen con el resto de parametros en manual
  - uint16_t doble: tiempo (ms) intervalo maximo entre dos pulsaciones para que estas sean consideradas pulsacion Doble
   
   ** Se debe tener en cuenta que este tiempo afecta al tiempo de reaccion para detecciones SIMPLES

  El estado de un pulsador se obtiene mediante la respuesta de su metodo Read() y puede ser:
      0 --> sin pulsar o en el proceso de identificar una pulsacion
      1 --> pulsacion corta
      2 --> pulsacion doble
      3 --> pulsacion larga 

  *******************************************************
					Ejemplo de uso:
  *******************************************************
   >> solo con el numero de pin
      ClickButton mi_pulsador_1(uint8_t pin); el resto de parametros por defecto: son (30,150,380)
   
   >> o bien definiendo todos los parametros disponibles
      ClickButton mi_pulsador_2(uint8_t pin, uint8_t antirrebotes, uint16_t corta, uint16_t larga);

   >> Modificar el tiempo entre clicks para deteccion de DobleClick (por defecto 110 ms)
      mi_pulsador_1.DobleClick(uint16_t doble)
	  mi_pulsador_2.DobleClick(uint16_t doble)

   >> para recoger el estado de un pulsador hemos de llamar a su metodo Read():
      estado_pulsador = mi_pulsador_1.Read();
      0 --> sin pulsar o en el proceso de identificar una pulsacion
      1 --> pulsacion corta
      2 --> pulsacion doble
      3 --> pulsacion larga
 */


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        IMPORTACION DE LIBRERIAS 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#include <ClickButton.h>


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//------------------------------------------------------------------------
// Algunas definiciones personales para mi comodidad al escribir codigo
//------------------------------------------------------------------------
#define AND   &&
#define OR    ||
#define NOT   !

//------------------------------------------------------------------------
// Otras definiciones para pines y variables
//------------------------------------------------------------------------
#define LED_OnBoard         13  	// led OnBoard de la placa Arduino UNO

#define PIN_pulsador         3

uint16_t contadorSimple = 0;         //se incrementa en cada deteccion de un click
uint16_t contadorDoble = 0;          //se incrementa en cada deteccion de una pulsacion doble
uint16_t contadorLargo = 0;          //se incrementa en cada deteccion de una pulsacion larga


//------------------------------------------------------------------------
// Creacion de objetos de la clase ClickButton
//------------------------------------------------------------------------

/* Podemos llamar al constructor de dos formas */
//ClickButton pulsador_1(PIN_pulsador);        // creacion de un objeto definiendo solo el pin de nuestro pulsador
											                         // el resto de valores por defecto (30, 100, 380)
                                
ClickButton pulsador(PIN_pulsador, 40, 120, 380);   	// creacion de un objeto definiendo todos sus parametros:
														                          // uint8_t pin, uint8_t antirrebotes, uint16_t corta, uint16_t larga



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup()  
{
  Serial.begin(115200);   	// Se inicia el puerto serie para depuracion
  Serial.println(F("Ejemplo de uso para la libreria ClickButton"));
  Serial.println(F("https://github.com/inopya\n"));
  Serial.println(F("Esta libreria permite el control de pulsadores PULLDOWN"));
  Serial.println(F("en los que podremos detectar tres tipos de eventos:"));
  Serial.println(F(" 1) Una pulsacion SIMPLE"));
  Serial.println(F(" 2) Una pulsacion DOBLE"));
  Serial.println(F(" 3) Una Pulsacion LARGA\n\n"));

  pinMode(LED_OnBoard, OUTPUT);
  digitalWrite(LED_OnBoard, LOW);
}




//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BUCLE PRINCIPAL DEL PROGRAMA   (SISTEMA VEGETATIVO)
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 


void loop()
{ 
  /* lectura del pulsador */
  uint8_t estado_pulsador = pulsador.Read();

  /* Deteccion de pulsacion corta */
  if (estado_pulsador == 1){
    Serial.print(F("SIMPLE "));Serial.println(++contadorSimple);
    digitalWrite(LED_OnBoard, HIGH);
    delay(150);
    digitalWrite(LED_OnBoard, LOW);
    
  }
  /* Deteccion de doble pulsacion */
  else if (estado_pulsador == 2){
    Serial.print(F("DOBLE "));Serial.println(++contadorDoble);
    digitalWrite(LED_OnBoard, HIGH);
    delay(150);
    digitalWrite(LED_OnBoard, LOW);
    delay(150);
    digitalWrite(LED_OnBoard, HIGH);
    delay(150);
    digitalWrite(LED_OnBoard, LOW);
  }
  /* Deteccion de pulsacion larga */
  else if (estado_pulsador == 3){
    Serial.print(F("LARGA "));Serial.println(++contadorLargo);
    digitalWrite(LED_OnBoard, HIGH);
    delay(350);
    digitalWrite(LED_OnBoard, LOW);
  }
}



//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************
