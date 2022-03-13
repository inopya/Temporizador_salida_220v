
/*

  #       _\|/_   A ver..., ¿que tenemos por aqui?
  #       (O-O)        
  # ---oOO-(_)-OOo-----------------------------------------
   
   
  ##########################################################
  # ****************************************************** #
  # *            ARDUINO PARA PRINCIPIANTES              * #
  # *   TEMPORIZADOR CON PANTALLA OLED Y SALIDA TRIAC    * #
  # *          Autor:  Eulogio López Cayuela             * #
  # *                  https://github.com/inopya         * #
  # *                                                    * #
  # *       Versión v2.0      Fecha: 17/12/2021          * #
  # ****************************************************** #
  ##########################################################
  
*/

/*
 * DESCRIPCION DEL PROYECTO:
    Control temporizado, partiendo de un temporizador electromecanico canibalizado debido a su grandisma imprecision
    El temporizador original desfasaba aproximadamente unos 20 minutos (de retraso) cada hora.
    Se aprovecha la carcasa por el tema del enchufe pasante,
    (unque queda demasiado pequeña apra la electronica y no cierra bien).
    Se cambia el cerebro a un arduino NANO sin RTC que consigue un error de 1 segundo de adelando por hora
    El control de alimentacion 220v se hace con Triac BT137 controlado por Arduino mediante un MOC3021
    (Seria recomendable apra una version definitiva usar un MOC304x o MOC306x o MOC308x, 
     que disponen de deteccion de paso por CERO, pudiendo asi realizar un "arranque suave" de las cargas)
    Dispone de pantalla OLED 128x64 como interfaz para visualizacion de tiempos y la programacion
    y de dos pulsadores para programar tiempos y otros controles de uso.

    Presenta un menu al iniciar en el que podemos elegir entre dos modos:
    1) TIMER con tiempo a dexconexion que muestra la cuenta atras hasta terminar la tarea 
       mas una barra de progreso que tambien va disminuyendo conforme se agota el tiempo
    2) RELOJ, que se matiene activa la salida y muestra el tiempo que lleva en marcha.
       posibilidad de pausar o parar la actividad (solo en modo reloj)

    NOTA:  
    La programacion modifica el valor de horas de 1 en 1.
    El valor de minutos los hace por defecto de 5 en 5, pero si mantenemos pulsada la tecla contratia a lo que deseamos hacer
    es decir tecla menos durante el incremento, o tecla mas durante un decremento de minutos, 
    veremos que el cursor parpadea rapidamente, y en ese momento los incrementos y decrementos seran de 1 en 1   ;-)



 * ESTADO DEL ARTE: 
 
   Modificada la carcasa con partes impresas en 3D
   Posibilidad de activar y desactivar el uso de sonda y el precalentamiento
   Añadidos iconos en el menu principal junto a cada opcion (Timer, Clock, Sonda NTC, Precalentar)

   Pendiente de implementar menu para programar el tiempo y tempeaturade precalentamiento
   Pendiente de implementar menu para programar los rangos de temperatura para ON/OFF
 

 * DETALLES DE COMPILACION CON ARDUINO IDE 1.8.15
 
   Compilado para ARDUINO NANO R3 Old Bootloader  (con chip 328p)

   - MODO SERIAL DEBUG ACTIVO - 
   El Sketch usa 21316 bytes del espacio de almacenamiento de programa. 
   Las variables Globales usan 630 bytes de RAM.

   - MODO OPERATIVO - 
   El Sketch usa 19664 bytes del espacio de almacenamiento de programa. (usando serial apra debug)
   Las variables Globales usan 458 bytes de RAM.
   
 
 *  CONEXIONES, Consultar el fichero "pinout.h"
 *  VARIABLES GLOBALES en "variables.h"
 *  OTRAS DEFINICIONES DE INTERES en "definiciones.h"
 
*/




/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        IMPORTACION DE LIBRERIAS 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#include  <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ClickButton.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


#include "definiciones.h"
#include "pinout.h"
#include "variables.h"



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE CREACION DE OBJETOS
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

ClickButton pulsador_MENOS(PIN_TECLA_PROG, true); 
ClickButton pulsador_MAS(PIN_TECLA_ENTER, true);



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        PROTOTIPADO DE FUNCIONES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void start_Timer1( void );                  
void stop_Timer1( void );                  
void start_Timer2( void );                 
void stop_Timer2( void );                  
void Reloj_ISR( void );                   
void CuentaAtras_ISR( void);                


void DEBUG( void );                      
void mostar_pantalla_espera( void );      
void pantalla_select_mode( void );        
void programar_tiempo( void );             
void cargarTemporizador( void );         
void runTimer( void );                    
void waitReset( void );                  
float read_NTC( uint8_t _pinNTC );         
int comprobar_estado_bateria( byte modo );  
void set_output( bool _state );
void print_reloj( uint32_t segundos );
void esperarTemperaturaMinima( void );



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup()  
{
  pinMode(PIN_TRIAC, OUTPUT);
  pinMode(PIN_LED_TRIAC, OUTPUT);
  digitalWrite(PIN_TRIAC, LOW);
  digitalWrite(PIN_LED_TRIAC, LOW);

  //pinMode(PIN_AUTO_VREF, OUTPUT);
  //digitalWrite(PIN_AUTO_VREF, HIGH);
  //delay(500);
  //analogReference(EXTERNAL);
  
  pinMode(LED_OnBoard, OUTPUT);
  digitalWrite(LED_OnBoard, LOW);
    
  SERIAL_BEGIN( SERIAL_BAUD_RATE );    
  PRINT_VERSION(F(_VERSION_));


  //Wire.setClock(400000); //
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    PRINTLN(F("Error OLED"));
    for(;;);
  }

  //display.flipScreenVertically();  //esta libreria no tiene la opcion, buscar alternativa
  
  estado_actual=MQ_RESTART;
  FLAG_run_clock = false;
  
  start_Timer1();   // usar interrupciones de timer 1
  //start_Timer2(); // usar interrupciones de timer 2
  
  

  //==================================================================================================================
  /* iniciar MANUAL DEBUG como timer */
  //operation_mode = OP_MODE_TIMER;
  //estado_actual = MQ_RESTART;    //iniciar con menu para programar timer
  //estado_actual = MQ_START_TIMER; //iniciar timer directamente con el tiempo de la eeprom
  //tiemposTimer[0] =  0;  // 6 cargamos con 6 horas y 10 minutos
  //tiemposTimer[1] = 0;  // 10
  //tiemposTimer[2] =  15;  // 0
  //FLAG_ntc_control = true;      // forzar el control automatico de temperatura (limites en variables.h)
  //==================================================================================================================
  
  
  //==================================================================================================================
  /* iniciar MANUAL DEBUG como reloj */
  //operation_mode = OP_MODE_CLOCK;
  //estado_actual = MQ_CLOCK;
  //==================================================================================================================
  
  /* iniciar (AUTO) seleccionando el modo  */
  estado_actual = MQ_SELECT_SCREEN;   // Modo normal de funcionamiento
  operation_mode = OP_MODE_SELECT;
  
  //Serial.flush();
  //Serial.end(); 
  
  float error_sonda = read_NTC(PIN_sonda_NTC);
  if(isnan(error_sonda)!=0){ 
    FLAG_ntc_error = true;
    FLAG_ntc_control = false; 
    FLAG_precalentar = false;
  }
  
}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BUCLE PRINCIPAL DEL PROGRAMA   (SISTEMA VEGETATIVO)
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void loop()
{  
  switch (estado_actual) 
  {   
  case MQ_RESTART:
      cargarTemporizador();           //cargar el tiempo en los contadores
      estado_actual=MQ_PAUSE;
      break;
  case MQ_PAUSE:
      mostar_pantalla_espera();      // bucle infinito hasta pulsar tecla
      estado_actual=MQ_MODIFY_PROG;
      break;    
  case MQ_MODIFY_PROG:
      programar_tiempo();             // bucle infinito mientras estado_actual=MQ_MODIFY_PROG;
      estado_actual=MQ_START_TIMER;
      break;
  case MQ_START_TIMER:
      if(FLAG_precalentar){
        esperarTemperaturaMinima();   // sperar hasta alcanzar los 37º para iniciar el temporizador de fermentacion
        FLAG_precalentar = false;
      }
      segmento=126.0/( float(tiemposTimer[0])*3600 + float(tiemposTimer[1])*60 + float(tiemposTimer[2]) ); //unidad basica de la barra de progreso
      contadorMedioSegundo=0;         // reset del "reloj" software
      cargarTemporizador();
      estado_actual=MQ_RUNNIG;
      FLAG_run_clock = true;
      operation_mode = OP_MODE_TIMER;
      set_output(true);
      //digitalWrite(PIN_TRIAC, HIGH);
      //digitalWrite(PIN_LED_TRIAC, HIGH);
      break; 
  case MQ_RUNNIG:   
      runTimer();
      break;
  case MQ_STOP:
      set_output(false);
      //digitalWrite(PIN_TRIAC, LOW);
      //digitalWrite(PIN_LED_TRIAC, LOW);
      FLAG_run_clock = false;
      estado_actual = MQ_WAIT_RESET;
      break; 
  case MQ_WAIT_RESET:
      waitReset();
      contadorOFF=0; //revisar si necesitamos estos reset de contadores...
      contadorON=0;      
      break;         
  case MQ_SELECT_SCREEN:
      pantalla_select_mode();
      FLAG_run_clock = true;
      break;
  case MQ_CLOCK:
      contadorHoras = 0;
      contadorMinutos = 0;
      contadorSegundos = 0;
      operation_mode = OP_MODE_CLOCK;
      FLAG_run_clock = true;
      set_output(true);
      estado_actual = MQ_RUNNIG;
      break; 
  case MQ_CLOCK_PAUSE:  //sin uso
      set_output(false);
      FLAG_run_clock = false;
      break; 
  default:
      break;
  }  
}




//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BLOQUE DE FUNCIONES: LECTURA SENSORES, TOMA DE DECISIONES, ...
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//   FUNCIONES PARA DEBUG
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void DEBUG()
{
  PRINT(F("estado_actual: "));PRINTLN(estado_actual);
  PRINT(F("FLAG_run_clock: "));PRINTLN(FLAG_run_clock);
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//   PANTALLA DE ESPERA AL INICIAR
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void mostar_pantalla_espera()
{
  display.clearDisplay();
  display.setCursor(12, 16);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print(F("EN ESPERA"));
  
  display.setCursor(20, 40); 
  display.setTextSize(1);
  //              "0123456789ABCDEF-+/*"
  display.print(F("Pulsacion larga"));
  display.setCursor(30, 52); 
  display.setTextSize(1);
  //              "0123456789ABCDEF-+/*"
  display.print(F("Para iniciar"));   
  display.display(); 

  /* Espera hasta que se pulse una tecla */
  while( pulsador_MENOS.Read()!=PULSAC_LARGA && pulsador_MAS.Read()!=PULSAC_LARGA ){
    // esperar y esperar...
  }
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//   PANTALLA PARA SELECCCION DE MODO
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
  
void pantalla_select_mode()
{
  bool FLAG_select_timer = true;
  uint8_t pulsador_menos;
  uint8_t pulsador_mas;
  int8_t opcion_menu_1=PROG_TIMER;
  int8_t opcion_menu_old; 
  
  estado_actual=MQ_SELECT_SCREEN; //redundante
  PRINTLN(F("entrada en menu seleccion"));
  contadorMedioSegundo = 0; //para forzar el refresco del menu

  
  while(estado_actual == MQ_SELECT_SCREEN){
    pulsador_menos = pulsador_MENOS.Read();
    pulsador_mas = pulsador_MAS.Read();
    
	  opcion_menu_old = opcion_menu_1;
	
    if( pulsador_menos==PULSAC_CORTA){
      opcion_menu_1++;
  	  if(opcion_menu_1>PROG_PREHOT){  opcion_menu_1=PROG_TIMER;  }
  	}
    else if( pulsador_menos==PULSAC_CORTA || pulsador_mas==PULSAC_CORTA ){
      opcion_menu_1--;
      if(opcion_menu_1 < PROG_TIMER){  opcion_menu_1=PROG_PREHOT;  }
    }	
    else if( pulsador_menos==PULSAC_LARGA || pulsador_mas==PULSAC_LARGA ){
  	  switch (opcion_menu_1) 
      {      
  	    case PROG_TIMER:
          operation_mode = OP_MODE_TIMER;
          estado_actual = MQ_MODIFY_PROG;//MQ_RESTART;
  		    break;
  	    case PROG_CLOCK:
          operation_mode = OP_MODE_CLOCK;
          estado_actual = MQ_CLOCK; 
  		    break;
  	    case PROG_PREHOT:
  		    FLAG_precalentar = !FLAG_precalentar;
          if(FLAG_ntc_error){ FLAG_precalentar=false; }
  		    if(FLAG_precalentar){ FLAG_ntc_control=true; }
  		    break;
  	    case PROG_NTC:
  		    FLAG_ntc_control = !FLAG_ntc_control;
          if(FLAG_ntc_error){ FLAG_ntc_control=false; }
  		    if(!FLAG_ntc_control){ FLAG_precalentar=false; }
  		    break; 		  
  	    default:
  		    break;
      }    
    } 

    //====================================================================
    //============= REDIBUJAR PANTALLA SI ES NECESARIO ===================
	  
    bool FLAG_refrescar_pantalla = false;
    static uint32_t contadorMedioSegundo_old;
    if( contadorMedioSegundo%4==0 && contadorMedioSegundo_old!=contadorMedioSegundo || opcion_menu_1 != opcion_menu_old){ FLAG_refrescar_pantalla =true; }
    if( FLAG_refrescar_pantalla ){ 
      contadorMedioSegundo_old = contadorMedioSegundo;
      display.clearDisplay();
      //===================================================
      //redibujar texto de ayuda sobre uso de menus      
      display.setTextColor(WHITE);
      display.setTextSize(1);

      if( contadorMedioSegundo%8<4 ){
        display.setCursor(0, 0);
        display.print(F("PULSA PARA SELECCION"));
      }
      else{
        display.setCursor(20, 0);
        display.print(F("Pulsacion larga"));
        display.setCursor(30, 8);
        display.print(F("Para iniciar"));  
      }
      
      //===================================================
      //---------------------- menu de seleccion linea 1 ----------------------
	    if(opcion_menu_1==PROG_TIMER){
        display.drawBitmap(6, 26,  icono_timer, 16, 24, 1); 
        display.setTextSize(2);
        display.setCursor(34, 30);
        display.print(textosMenu[PROG_TIMER]);
      }
      else if(opcion_menu_1==PROG_CLOCK){
        display.drawBitmap(4, 26,  icono_clock, 24, 24, 1); 
        display.setTextSize(2);
        display.setCursor(40, 30);
        display.print(textosMenu[PROG_CLOCK]);
      }              
      else if(opcion_menu_1==PROG_NTC){
        display.drawBitmap(6, 26,  icono_ntc, 16, 24, 1); //icono_clock
        display.setTextSize(2);
        display.setCursor(40, 24);
        display.print(textosMenu[PROG_NTC]);
        float error_sonda = read_NTC(PIN_sonda_NTC);
        if(isnan(error_sonda)!=0){ 
          display.setCursor(40, 42);display.print(F("error"));
          FLAG_ntc_error = true;
          FLAG_ntc_control = false; FLAG_precalentar = false;
        }
        else if(FLAG_ntc_control){ display.setCursor(58, 42); display.print(F("on"));  }
        else{ display.setCursor(54, 42);display.print(F("off"));  }
      } 
      else if(opcion_menu_1==PROG_PREHOT){
        display.drawBitmap(6, 26,  icono_prehot, 16, 24, 1); //icono_clock
        display.setTextSize(2);
        display.setCursor(32, 24);
        display.print(textosMenu[PROG_PREHOT]);
        if(FLAG_precalentar){ display.setCursor(54, 42); display.print(F("on"));  } //9 caracteres maximo
        else{ display.setCursor(50, 42); display.print(F("off"));  }           
      }
      display.display();
    }
    //============= FIN REDIBUJAR pantalla si es necesario ===================
  }	
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//   PROGRAMAR TIEMPOS 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void programar_tiempo()
{
  bool FLAG_prog_hour = true;

  while(estado_actual==MQ_MODIFY_PROG){
    uint8_t estado_pulsador_menos = pulsador_MENOS.Read();
    uint8_t estado_pulsador_mas = pulsador_MAS.Read();
    if( estado_pulsador_mas==PULSAC_LARGA ){ estado_actual = MQ_START_TIMER; }
    else if( estado_pulsador_menos==PULSAC_LARGA ){ FLAG_prog_hour = !FLAG_prog_hour; }

    if(estado_pulsador_menos!=PULSAC_NULA || estado_pulsador_mas!=PULSAC_NULA ){
      // si la pulsacion es igual a pulsacion mantenida (PULSAC_MANTENIDA) 
      // provocaremos el parpadeo rapido del cursor lo que nos indicara tal estado
      // y que con ello los saltos de minutos son de 1 en 1 ;-)
      contadorMedioSegundo++;  //para provocar el refresco de pantalla
      PRINT(F("Pulsadores: "));PRINT(estado_pulsador_menos);
      PRINT(F(","));PRINTLN(estado_pulsador_mas);
      PRINT(F("FLAG_prog_hour: "));PRINTLN(FLAG_prog_hour);
      PRINT(F("estado_actual: "));PRINTLN(estado_actual);
    }
    
    if(FLAG_prog_hour){ // MODIFICAR HORAS
      if(estado_pulsador_menos==PULSAC_CORTA){ tiemposTimer[0]-=1; }
      if(estado_pulsador_mas==PULSAC_CORTA){ tiemposTimer[0]+=1; }
         
      if(tiemposTimer[0]<0){ tiemposTimer[0] = 24; }
      if(tiemposTimer[0]>24){ tiemposTimer[0] = 0;}
    }
    else{ // MODIFICAR MINUTOS
      if(estado_pulsador_menos==PULSAC_CORTA){ 
        if(estado_pulsador_mas==PULSAC_MANTENIDA){ tiemposTimer[1]--; }
        else{ tiemposTimer[1]-=5; }
      }
      
      if(estado_pulsador_mas==PULSAC_CORTA){
        if(estado_pulsador_menos==PULSAC_MANTENIDA){ tiemposTimer[1]++; }
        else{ tiemposTimer[1]+=5; } 
      }
         
      if(tiemposTimer[1]<0){ tiemposTimer[1] = 55; }
      if(tiemposTimer[1]>59){ tiemposTimer[1] = 0;}  
    }
      
    if( contadorMedioSegundo!=momentoAnterior ){
      momentoAnterior = contadorMedioSegundo;
      
      contadorHoras=tiemposTimer[0];
      contadorMinutos=tiemposTimer[1];
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.setTextSize(1);
      if(FLAG_prog_hour){ display.print(F("SELECT TIME (HOURS)")); }
      else{ display.print(F("SELECT TIME (MINUTES)")); } 
            
      display.setCursor(18, 24);
      display.setTextSize(3);
      if( contadorHoras<10 ) { display.print(F("0")); }
      display.print(contadorHoras);
  
      display.print(F(":"));
  
      if( contadorMinutos<10 ) { display.print(F("0")); }
      display.print(contadorMinutos); 
  
      if(contadorMedioSegundo%2==0){ // 
        uint8_t x;
        if(FLAG_prog_hour){ x=18; }
        else{ x=72; }
        for(uint8_t i=52;i<56;i++){
          display.drawLine(x,i,x+32,i, WHITE);
        }
      }
      display.display();
    }
  }
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//      CARGAR CONTADORES DEL TEMPORIZADOR
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void cargarTemporizador()
{
 
  contadorHoras = tiemposTimer[0];
  contadorMinutos = tiemposTimer[1];
  contadorSegundos = tiemposTimer[2];

  // DEBUG
  PRINTLN(F("Temporizador Cargado: "));
  PRINT(F("Horas: "));PRINTLN(contadorHoras);
  PRINT(F("Minutos: "));PRINTLN(contadorMinutos);
  PRINT(F("Segundos: "));PRINTLN(contadorSegundos);

}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    EJECUAR PROCESO DE TEMPORIZACION
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
 
void runTimer()
{
  float voltaje;
  float temperatura;
  static bool FLAG_punto_segundero;

  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
  //mmmmmmmmmmmmmmmmmmmmmmmm  MODO RELOJ mmmmmmmmmmmmmmmmmmmmmmmm
  
  if( operation_mode!=OP_MODE_TIMER ){

    uint8_t pulsador_menos;
    uint8_t pulsador_mas;
    pulsador_menos = pulsador_MENOS.Read();
    pulsador_mas = pulsador_MAS.Read();
        
    if( pulsador_menos==PULSAC_CORTA || pulsador_mas==PULSAC_CORTA ){
      FLAG_run_clock = !FLAG_run_clock; 
      set_output(FLAG_run_clock);
    }
    else if( FLAG_run_clock == false && (pulsador_menos==PULSAC_LARGA || pulsador_mas==PULSAC_LARGA) ){
      estado_actual=MQ_STOP;
    }	
  }

  //mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
  //mmmmmmmmmmmmmmmmmmmmmmmm  MODO TIMER mmmmmmmmmmmmmmmmmmmmmmmm
  
  if( FLAG_run_clock == false && operation_mode==OP_MODE_TIMER ){ estado_actual=MQ_STOP; }
  
  if( contadorMedioSegundo!=momentoAnterior){
    momentoAnterior = contadorMedioSegundo;

    //====================  SEGUNDERO BASADO en la ISR de TIMER 1 ====================
    if(contadorMedioSegundo%2==0){
      if(XTAL_SEGUNDOS%10==0){
        voltaje = comprobar_estado_bateria(0);
        temperatura = read_NTC(PIN_sonda_NTC);
        
        PRINT_DEBUG(F("voltaje: ")); PRINTLN_DEBUG(voltaje);
        PRINT_DEBUG(F("temperatura: ")); PRINTLN_DEBUG(temperatura);
        
        if( isnan(temperatura)==0 && FLAG_ntc_control && FLAG_run_clock){  //isnan(x) porque a veces tenemos una mala lectura del sensor 
          if(temperatura < LOW_TEMP){ set_output(true); }          // <40  FLAG_ESTADO_TRIAC=true;
          else if(temperatura > HIGH_TEMP){ set_output(false); }   // >=46  FLAG_ESTADO_TRIAC=false;
          PRINT_DEBUG(F("FLAG_ESTADO_TRIAC: ")); PRINTLN_DEBUG(FLAG_ESTADO_TRIAC);
        }
        
      }
      if(FLAG_ntc_control){
        if(FLAG_ESTADO_TRIAC){contadorON++;}
        else{contadorOFF++;}
        PRINT_DEBUG(F("contadorON: ")); PRINTLN_DEBUG(contadorON);
        PRINT_DEBUG(F("contadorOFF: ")); PRINTLN_DEBUG(contadorOFF);
      }
      XTAL_SEGUNDOS++;  //para tener un segundero 
      TIME_OUT++;       //control de timeout para menus y similar (sin uso por ahora)
    }
    
    FLAG_punto_segundero = !FLAG_punto_segundero;
    display.clearDisplay();
    display.setTextColor(WHITE);

    if(!FLAG_run_clock && FLAG_punto_segundero){
      display.setCursor(12, 4);
      display.setTextSize(1);
      display.print(F("PAUSE"));
    }
	//====================  FIN SEGUNDERO basado en la ISR de timer 1 ====================
	
	
	//==================== MOSTRAR TEMPERATURA Y VOLTAGE ==================== 
	if( FLAG_run_clock && FLAG_ntc_control){    
		display.setCursor(0, 0);
		display.setTextSize(2);
    
		display.print(temperatura,1);
		display.print((char)247);
		display.print(F("C"));
    }
    
    display.setCursor(90, 0);
    display.setTextSize(1);
    
    display.print(voltaje/1000);
    display.print(F("v"));
	//==================== FIN mostrar temperatura y voltage ==================== 
	
	
    //==================== MOSTRAR RELOJES EXTRA ON/OFF ==================== 
    if(FLAG_ntc_control){ // mostrar relojesextra y desplazar el reloj principal un poco hacia abajo
      display.setCursor(0, 18);
      display.setTextSize(1);
      print_reloj(contadorON);
      
      display.setCursor(63, 18);
      print_reloj(contadorOFF);
	  
      display.setCursor(8, 30);    
    }
    else{ display.setCursor(8, 24); }// sin mostrar tiempos on y off, Reloj en su posicion habitual    
    //==================== FIN mostar relojes extra ON/OFF ==================== 
	
	
	//==================== MOSTRAR EL RELOJ PRINCIPAL ====================
    display.setTextSize(3);
    if( contadorHoras<10 ) { display.print(F("0")); }
    display.print(contadorHoras);

    if(FLAG_punto_segundero && FLAG_run_clock){  //&& FLAG_run_clock para fijar los puntos en modo pausa
      display.setTextColor(BLACK);
    }
    display.print(F(":"));

    display.setTextColor(WHITE);
    if( contadorMinutos<10 ) { display.print(F("0")); }
    display.print(contadorMinutos); 

    display.setTextSize(2);
    if( contadorSegundos<10 ) { display.print(F("0")); }
    display.print(contadorSegundos);  
	//==================== FIN mostrar el reloj principal ====================
	
    //display.setCursor(0, 56); //0,56 > ultima linea disponible para setTextSize(1)
    //display.setTextSize(1); //caben 21 caracteres
    //display.setTextSize(2); //caben 10 caracteres
    //display.setTextSize(3); //caben 7 caracteres
    //display.setTextSize(4); //caben 5 caracteres 
    //display.setTextSize(5); //caben 4 caracteres
    //display.setTextSize(6); //caben 3 caracteres	


	//==================== BARRA DEPROGRESO DEL MODO TIMER ====================
    if( operation_mode==OP_MODE_TIMER ){ // mostrar barra de progreso
      float barraGrafica = segmento*(3600*float(contadorHoras)+60*float(contadorMinutos)+float(contadorSegundos));
      barraGrafica = int(barraGrafica);
      
      display.drawLine(0,54,127,54, WHITE);
      display.drawLine(0,63,127,63, WHITE);
      display.drawLine(0,54,0,63, WHITE);
      display.drawLine(127,54,127,63, WHITE);
      for(uint8_t i=56;i<62;i++){
        display.drawLine(0,i,barraGrafica,i, WHITE);
      }
    }
	//==================== FIN bara de progreso del modo timer ====================
	
    display.display();      
  }
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    ESPERA PARA NUEVO REINICIO
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void waitReset()
{
  static int8_t contadorReset=-1;
  if( contadorReset ==-1 || pulsador_MENOS.Read()==PULSAC_CORTA || pulsador_MAS.Read()==PULSAC_CORTA ){
    contadorReset++;
    //PRINT(F("contadorReset: "));PRINTLN(contadorReset);
    
    display.clearDisplay();
    display.setTextColor(WHITE);

    // --> Mostar tiempos de encendido y reposo
    if(FLAG_ntc_control){
      display.setCursor(0, 0);
      display.setTextSize(1);
      print_reloj(contadorON+contadorPREcalentar);
      
      display.setCursor(63, 0);
      print_reloj(contadorOFF);
    }
    // <--
  
    display.setCursor(12, 16);
    display.setTextSize(2);
    display.print(F("TIMER OFF"));
    display.setCursor(0, 40); 
    display.setTextSize(1);
    //              "0123456789ABCDEF-+/*"
    display.print(F("  Presiona ("));
    display.print(4-contadorReset);
    display.print(F(") veces "));
    display.setCursor(0, 52); 
    //              "0123456789ABCDEF-+/*"
    display.print(F("    para reiniciar   "));
    display.display();  
  }
  
  if(contadorReset>3){                  // Provocamos un 'reset'
    contadorReset=-1;
    //estado_actual = MQ_RESTART;       // reinicar como timer
    mostar_pantalla_espera();
    estado_actual = MQ_SELECT_SCREEN;   // reiniciar como menu seleccion timer/clock
  }
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//       INTERRUPCIONES  SOFTWARE  TIMER 1
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
// INICIAR TIMER 1
//========================================================

void start_Timer1()
{
  /* Setup Timer1  para que junto a la funcion de atencion a su interrupcion
   * poder obtener una 'señal util' cada 500 ms */
  //TCCR1A = 0x00;
  TCCR1B = 0x00;        // deshabilita Timer1 mientras lo estamos ajustando
  TCNT1  = 0xE174; ;    //(57716) establecer el contador iniciandolo con este valor para contar 7820
  TIFR1  = 0x00;        // Timer1 INT Flag Reg: borrar la bandera de desbordamiento
  TIMSK1 = 0x01;        // Timer1 INT Reg: habilita la interrupcion de desbordamiento de Timer2
  TCCR1A = 0x00;        // Timer1 Control Reg A: Wave Gen Mode normal
  TCCR1B |= (1 << CS10)|(1 << CS12);    // 1024 prescaler de timer 1
  //TCCR1B = 0x05;        // Timer1 Control Reg B: Timer Prescaler set to 1024
}


//========================================================
// PARAR TIMER 1
//========================================================

void stop_Timer1()
{
 TCCR1B = 0x00;         // deshabilita Timer1 mientras lo estamos ajustando
 TIMSK1=0 ;             // Timer1 INT Reg: deshabilita la interrupcion de desbordamiento de Timer1
}


//========================================================
// RUTINA DE INTERRUPCION PARA DESBORDAMIENTO DE TIMER 2
// El vector de desbordamineto es -->  TIMER1_OVF_vect
//========================================================

ISR(TIMER1_OVF_vect) 
{
  /* 
     Con el preescaler a 1024, contando desde 57717 en TCNT1 para contabilizar 7819 ciclos 
     obtenemos una señal que permitetemporizar 500ms con bastante exactitud
     Este Reloj por software adelanta xx segundos cada xx hora
     contando 7819 --> adelanta 0.1 segundo cada 1 hora (vcc 5v, sin prueba con bateria)
	//iniciando en 0xE175 +1 segundos en 10 horas Arduino UNO 5v
	//iniciando en 0xE175 +6 segundos en 10 horas Arduino NANO 4.1v	
	//iniciando en 0xE174 +-??? segundos en 10 horas Arduino NANO 4.1v	
  */
  
  contadorMedioSegundo ++;
  if( operation_mode == OP_MODE_TIMER){ CuentaAtras_ISR(); }
  else{ Reloj_ISR(); }   

  TCNT1 = 0xE174;                 //0xE174 reset del contador iniciandolo con el valor 57716 para contar 7820
  TIFR1 = 0x00;                   // Timer1 INT Flag Reg: borrar la bandera de desbordamiento
  //0xE175 +1 segundos en 10 horas Arduino UNO 5v
  //0xE175 +6 segundos en 10 horas Arduino NANO 4.1v
  //0xE174 en pruebas en arduino nano 4.1v

}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//       INTERRUPCIONES  SOFTWARE  TIMER 2
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
// INICIAR TIMER 2
//========================================================

void start_Timer2()
{
  /* Setup Timer2  para que junto a la funcion de atencion a su interrupcion
   * poder obtener una 'señal util' cada 500 ms */
  TCCR2B = 0x00;        // deshabilita Timer2 mientras lo estamos ajustando
  TCNT2  = 0x04 ;        //(4) establecer el contador iniciandolo con el valor 4 (para contar 252)
                        // OJO: cada unidad mas supone aproximadmente adelantar unos 17s/Hora
  TIFR2  = 0x00;        // Timer2 INT Flag Reg: borrar la bandera de desbordamiento
  TIMSK2 = 0x01;        // Timer2 INT Reg: habilita la interrupcion de desbordamiento de Timer2
  TCCR2A = 0x00;        // Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x07;        // Timer2 Control Reg B: Timer Prescaler set to 1024
  //TCCR2B |= (1 << CS10)|(1 << CS11)|(1 << CS12);    // otra forma de poner a 1024 el prescaler de Timer2
}


//========================================================
// PARAR TIMER 2 
//========================================================

void stop_Timer2()
{
 TCCR2B = 0x00;         // deshabilita Timer2 mientras lo estamos ajustando
 TIMSK2=0 ;             // Timer2 INT Reg: deshabilita la interrupcion de desbordamiento de Timer2
}


//========================================================
// RUTINA DE INTERRUPCION PARA DESBORDAMIENTO DE TIMER 2
// El vector de desbordamineto es -->  TIMER2_OVF_vect
//========================================================

volatile unsigned int timer2_ovf_count = 0;   // contador de numero de interrupciones (desbordamientos de Timer2)
                                              // necesario porque es un timer de 8 bits y hemos de contar ~8070
ISR(TIMER2_OVF_vect) 
{
  /* 
     Con el preescaler a 1024, contando desde 4 en TCNT2 (es decir 252) y timer2_ovf_count>=31,(32 ciclos) 
     con esta funcion de atencion a su interrupcion se obtiene una 'señal util' cada 500 ms 
     (realmente hemos de hacer 31 conteos de 252 y uno de 256 para cada ciclo de 500ms)
     Este Reloj por software adelanta 1.4 segundos cada 1 hora
     31 conteos de 252 y 1 de 256 --> adelanta 1.4 segundos cada 1 hora (vcc 5v, sin prueba con bateria)
     30 conteos de 252 y 2 de 255 --> adelanta 0.2 segundos cada 1 hora (vcc 5v, sin prueba con bateria)
  */
  
  timer2_ovf_count++;             // Incremento del contador de numero de interrupciones
  if(timer2_ovf_count >= 31){     // >= 31 cuenta 32 (0-31)
    timer2_ovf_count = 0;         // reset del contador de numero de interrupciones
    contadorMedioSegundo ++;
    if( operation_mode == OP_MODE_TIMER){ CuentaAtras_ISR(); }
    else{ Reloj_ISR(); }   
  }
  if(timer2_ovf_count<2){
    TCNT2 = 0x01;                 // reset del contador iniciandolo con el valor 1 para contar 255
  }
  else{
    TCNT2 = 0x04;                 // reset del contador iniciandolo con el valor 4 para contar 252
  }
  TIFR2 = 0x00;                   // Timer2 INT Flag Reg: borrar la bandera de desbordamiento
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//       TEMPORIZADORES SOFTWARE BASADOS EN INTERRUPCIONES TIMER1/TIMER2...
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
// RELOJ CON INTERRUPCION SOFTWARE
//========================================================

void Reloj_ISR()
{
  /*
  * Reloj relativamente preciso mediante software
  * y el uso de interrupciones intermas del timer02 
  * Podemos ponerlo en hora actuando sobre las variables globales 
  * 'horas', 'minutos' y 'segundos'  definidas al inicio del codigo
  * 
  * Usado para mostrar tiempo en pantalla unicamente como un contador creciente
  */

  if( FLAG_run_clock == false){ return; }  //permite hacer pausas en el reloj

  if(contadorMedioSegundo%2==0){
    contadorSegundos ++;
    if(contadorSegundos == 60){
      contadorSegundos = 0;
      contadorMinutos ++;
      if(contadorMinutos == 60){
        contadorMinutos = 0;
        contadorHoras ++;
        if(contadorHoras == 24){
          contadorHoras = 0;
        }
      }
    }  
  }
}


//========================================================
// CUENTA ATRAS CON INTERRUPCION SOFTWARE 
//========================================================

void CuentaAtras_ISR()
{
  /*
  * Contador descendente mediante el uso de interrupciones intermas del timer02
  */

  if( FLAG_run_clock == false){ return; }
  
  if(contadorHoras==0 AND contadorMinutos==0 AND contadorSegundos==0){
    /* por si se queda activo el servicio de ISR, salimos de esta rutina si el tiempo ha llegado a cero
     evitando asi que se produzcan valores negativos que generen erroes de representacion el el LCD */                                
    FLAG_run_clock = false;
    digitalWrite(PIN_TRIAC, LOW);  //por segurudad lo paro desde aqui, aunque lo haga tambien en el loop()
    digitalWrite(PIN_LED_TRIAC, LOW);
	return;
  }
  
  if(contadorMedioSegundo%2 == 0){                        // cada dos medios segundos, 
    contadorSegundos --;                                  // descontamos 1 segundo
    if(contadorSegundos == -1){                           // Despues del segundo CERO,...
      contadorSegundos = 59;                              // viene el 59
      contadorMinutos --;                                 // y con ello descontar 1 minuto
      if(contadorMinutos == -1){                          
        contadorMinutos = 59;                             // El paso de CERO minutos lleva al 59
        contadorHoras --;                                 // y con ello descontar 1 hora
      }
    }
  } 
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    TERMOMETRO CON SENSOR NTC 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

float read_NTC(uint8_t _pinNTC) 
{
  //const int Vcc = 5;
  const int Rc = 10000;       //valor de la resistencia en serie con la NTC
  float K = 2.5;              //factor de disipacion en mW/C
  float A = 1.11492089e-3;
  float B = 2.372075385e-4;
  float C = 6.954079529e-8;
  
  //------------------------------//
  
  float raw = analogRead(_pinNTC);
  delay(5);
  raw = analogRead(_pinNTC);
//    for(int x=0; x<4; x++){ 
//      raw = analogRead(PIN_sonda_NTC);
//    }

  float Vcc = comprobar_estado_bateria(0)/1000.0;
  //if( Vcc!=5.0 ){ Vcc=5.0; }
  

  float Vntc =  (raw * Vcc) / float(LIMITE_ADC);
  float Rntc = ( Vntc * Rc ) / ( Vcc - Vntc );

  PRINT(F("Vcc=")); PRINTLN(Vcc); 
  PRINT(F("raw=")); PRINTLN(raw); 
  PRINT(F("Vntc=")); PRINTLN(Vntc);
  PRINT(F("Rntc=")); PRINTLN(Rntc);
  
  float logR  = log(Rntc);
  float R_th = 1.0 / (A + B * logR + C * logR * logR * logR );
 
  float kelvin = R_th - 1000*(Vntc*Vntc)/(K * Rntc);
  float celsius = kelvin - 273.15;
  celsius=celsius*1.02;    // *1.03 --> ajuste para esta NTC (calculo empirico por calibracion )
  return celsius;
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//     CONTROL DEL ESTADO DE LA BATERIA / ALIMENTACION
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
//   FUNCION PARA MONITORIZAR EL ESTADO DE VCC
//========================================================

int comprobar_estado_bateria(byte modo) 
{
  /* ----  0 devuelve milivoltios, >0 devuelve porcentaje de carga  ---- */
  /* leer la referecia interna de  1.1V  para calcular Vcc */
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0) ;
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2);                         // pausa para que Vref se estabilice
  ADCSRA |= _BV(ADSC);              // iniciar medicion
  while (bit_is_set(ADCSRA,ADSC));  // proceso de medicion propiamente dicho
 
  uint8_t low  = ADCL; // leer ADCL
  uint8_t high = ADCH; // leer ADCH
 
  long lecturaACD = (high<<8) | low;
 
  long milivoltios = 1125300L / lecturaACD;           // Calcular Vcc en mV (1125300 = 1.1*1023*1000)
  float voltaje = milivoltios/1000.0;                 // estado de la bateria en mV 
  float porcentage_carga = (100-(4.2-voltaje)*100);   // una LiPo al 100% tiene 4'2V, al 0% --> 3'2V
  
  if(porcentage_carga<0){ porcentage_carga = 0;}      // ojito la bateria estaria por debajo de 3'2 voltios
  if(porcentage_carga>100){ porcentage_carga = 100;}  // ojito la bateria estaria por encima de 4'2 voltios

  if(modo == 0){ return int(milivoltios); }           // Voltaje de la bateria en milivoltios
  if(modo > 0){ return int(porcentage_carga); }       // carga restante en porcentaje  
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    CONTROL SALIDA TIRISTOR 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void set_output(bool _state)
{
  if(_state){
    digitalWrite(PIN_TRIAC, HIGH);
    digitalWrite(PIN_LED_TRIAC, HIGH);
    FLAG_ESTADO_TRIAC = true;
  }
  else{
    digitalWrite(PIN_TRIAC, LOW);
    digitalWrite(PIN_LED_TRIAC, LOW);
    FLAG_ESTADO_TRIAC = false;
  }
}



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    MOSTRAR TIEMPO EN FORMATO RELOJ
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void print_reloj(uint32_t segundos)
{
  uint32_t horas = segundos/3600;
  segundos = segundos%3600;
  uint32_t minutos = segundos/60;
  segundos = segundos%60;

  if(horas<10){ display.print(F("0")); }
  display.print(horas); display.print(F(":")); 
  if(minutos<10){ display.print(F("0")); }
  display.print(minutos); display.print(F(":")); 
  if(segundos<10){ display.print(F("0")); }
  display.print(segundos);
}


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    MOSTRAR TIEMPO EN FORMATO RELOJ
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void esperarTemperaturaMinima()
{
  set_output(true);
  float temperatura;
  temperatura = read_NTC(PIN_sonda_NTC);

  //display.clearDisplay();
  display.setTextColor(WHITE);
  contadorPREcalentar = 0;
  
  while(temperatura<INIT_TEMP){ 
    //while(true){
  	if (contadorMedioSegundo != momentoAnterior){
      momentoAnterior = contadorMedioSegundo;
      if(contadorMedioSegundo%2==0){
        contadorPREcalentar++;
        if(contadorPREcalentar%2==0){ temperatura = read_NTC(PIN_sonda_NTC); }
        display.clearDisplay();
        display.setTextSize(1);
  		  display.setCursor(40, 16);  //revisar las coordenadas de posicion del reloj!!
  		  print_reloj(contadorPREcalentar);
  			if(isnan(temperatura)==0){
  			  display.setCursor(0, 0);
  			  display.print(F("PRECALENTANDO A "));
  			  display.print(INIT_TEMP);
  			  display.print((char)247);
  			  display.print(F("C"));
  				  
  			  display.setCursor(10, 42);  // sin reloj en (10, 24)
  			  display.setTextSize(3);
  			  display.print(temperatura,1);
  			  display.print((char)247);
  			  display.print(F("C"));
  			  PRINTLN_DEBUG(temperatura);
  			  //if( temperatura >= INIT_TEMP ){ break; }  
  			} 
        display.display(); 
  	  }
  	  if( contadorPREcalentar > LIMITE_PRECALENTAR ){ break; }    
  	}
  }
}


//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************
