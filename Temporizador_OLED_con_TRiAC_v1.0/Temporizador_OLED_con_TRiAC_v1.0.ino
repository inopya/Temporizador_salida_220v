
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
  # *       Versión v1.0      Fecha: 02/05/2021          * #
  # ****************************************************** #
  ##########################################################
  
*/

/*
 * DESCRIPCION DEL PROYECTO:
    Control temporizado, partiendo de un temporizador electromecanico canibalizado debido a su grandisma imprecision
    El temporizador original desfasaba aproximadamente unos 25 minutos (de retraso) cada hora.
    Se aprovecha la carcasa por el tema del enchufe pasante,
    (unque queda demasiado pequeña apra la electronica y no cierra bien).
    Se cambia el cerebro a un arduino NANO sin RTC que consigue un error de 0.14 segundos de adelando por hora
    El control de alimentacion 220v se hace con Triac BT137 controlado por Arduino mediante un MOC3041
    que dispone de deteccion de paso por CERO, pudiendo asi realizar un "arranque suave" de las cargas.
    Sería valida tambien una version sin cruce por cero como MOC3021
    Dispone de pantalla OLED 128x64 como interfaz para visualizacion de tiempos y la programacion
    y de dos pulsadores para programar tiempos y otros controles de uso.

    Presenta un menu al iniciar en el que podemos elegir entre dos modos:
    1) TIMER con tiempo a desconexion que muestra la cuenta atras hasta terminar la tarea 
       mas una barra de progreso que tambien va disminuyendo su longitud conforme se agota el tiempo.
    2) RELOJ, modo en que se matiene activa la salida y muestra el tiempo que lleva en marcha.
       posibilidad de pausar o parar la actividad (solo en modo reloj)

    NOTA:  
    La programacion modifica el valor de horas de 1 en 1.
    El valor de minutos los hace por defecto de 5 en 5, pero si mantenemos pulsada la tecla contratia a lo que deseamos hacer
    es decir tecla menos durante el incremento, o tecla mas durante un decremento de minutos, 
    veremos que el cursor parpadea rapidamente, y en ese momento los incrementos y decrementos seran de 1 en 1   ;-)



 * ESTADO DEL ARTE: 
 
   ¿Buscar una carcasa fisica mas aparente...?

 

 * DETALLES DE COMPILACION CON ARDUINO IDE 1.8.10
 
   Compilado para ARDUINO NANO R3 Old Bootloader  (con chip 328p)

   - MODO OPERATIVO - 
   El Sketch usa 15838 bytes del espacio de almacenamiento de programa. (Serial desactivado)
   Las variables Globales usan 364 bytes de RAM.
   
 
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



#include "definiciones.h"
#include "pinout.h"
#include "variables.h"



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE CREACION DE OBJETOS
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#define SCREEN_WIDTH 128 // ancho de la pantalla  (en pixeles) 
#define SCREEN_HEIGHT 64 // alto de la pantalla  (en pixeles) 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

ClickButton pulsador_MENOS(PIN_TECLA_MENOS, true); 
ClickButton pulsador_MAS(PIN_TECLA_MAS, true);



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        PROTOTIPADO DE FUNCIONES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

void start_Timer1( void );    
void Reloj_ISR( void );   
void CuentaAtras_ISR( void); 
  
void mostar_pantalla_espera( void ); 
void pantalla_select_mode( void ); 
void programar_tiempo( void );  
void cargarTemporizador( void ); 
void runTimer( void );   
void waitReset( void );    
int comprobar_estado_bateria( byte modo );  




//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup()  
{
  estado_actual=MQ_RESTART;  // sin uso
  
  pinMode(PIN_TRIAC, OUTPUT);
  pinMode(PIN_LED_TRIAC, OUTPUT);
  digitalWrite(PIN_TRIAC, LOW);
  digitalWrite(PIN_LED_TRIAC, LOW);
  
  pinMode(LED_OnBoard, OUTPUT);
  digitalWrite(LED_OnBoard, LOW);
    
  SERIAL_BEGIN(115200);    
  PRINTLN(F(_VERSION_));

  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    PRINTLN(F("Error OLED"));
    for(;;);
  }

  
  /* iniciar seleccionando el modo */
  FLAG_run_clock = false;
  operation_mode = OP_MODE_SELECT;
  estado_actual = MQ_SELECT_SCREEN;
  
  start_Timer1();   // programar y usar interrupciones de timer 1
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
      //segmento = unidad basica de la barra de progreso
      segmento=126.0/( float(tiemposTimer[0])*3600 + float(tiemposTimer[1])*60 + float(tiemposTimer[2]) ); 
      
      contadorMedioSegundo=0;         // reset del "reloj" software
      cargarTemporizador();
      estado_actual=MQ_RUNNIG;
      FLAG_run_clock = true;
      operation_mode = OP_MODE_TIMER;
      digitalWrite(PIN_TRIAC, HIGH);
      digitalWrite(PIN_LED_TRIAC, HIGH);
      break; 
  case MQ_RUNNIG:
      runTimer();
      break;
  case MQ_STOP:
      digitalWrite(PIN_TRIAC, LOW);
      digitalWrite(PIN_LED_TRIAC, LOW);
      FLAG_run_clock = false;
      estado_actual = MQ_WAIT_RESET;
      break; 
  case MQ_WAIT_RESET:
      waitReset();
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
      digitalWrite(PIN_TRIAC, HIGH);
      digitalWrite(PIN_LED_TRIAC, HIGH);
      estado_actual = MQ_RUNNIG;
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
  display.print(F("Pulsacion larga"));
  display.setCursor(30, 52); 
  display.setTextSize(1);
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

  estado_actual=MQ_SELECT_SCREEN; //redundante
  contadorMedioSegundo = 0; //para forzar el refresco del menu

  
  while(estado_actual == MQ_SELECT_SCREEN){
    pulsador_menos = pulsador_MENOS.Read();
    pulsador_mas = pulsador_MAS.Read();
    
    if( pulsador_menos==PULSAC_CORTA || pulsador_mas==PULSAC_CORTA ){
      FLAG_select_timer=!FLAG_select_timer;
      contadorMedioSegundo=0; //para forzar el refresco del menu
    }
    else if( pulsador_menos==PULSAC_LARGA || pulsador_mas==PULSAC_LARGA ){
      if(FLAG_select_timer){
        operation_mode = OP_MODE_TIMER;
        estado_actual = MQ_MODIFY_PROG;
      }
      else{
        operation_mode = OP_MODE_CLOCK;
        estado_actual = MQ_CLOCK;    
      }  
    }
    
    if( contadorMedioSegundo%4==0 ){  
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      if( contadorMedioSegundo%8==0 ){
        display.setCursor(0, 0);
        display.print(F("PULSA PARA SELECCION"));
      }
      else{
        display.setCursor(20, 0);
        display.print(F("Pulsacion larga"));
        display.setCursor(30, 8);
        display.print(F("Para iniciar"));  
      } 
  
      display.setTextSize(2);
      if( FLAG_select_timer ){
        display.setCursor(0, 22); 
        display.print(F("  >TIMER<")); 
        display.setCursor(0, 40); 
        display.print(F("   CLOCK"));
      }
      else{
        display.setCursor(0, 22); 
        display.print(F("   TIMER")); 
        display.setCursor(0, 40); 
        display.print(F("  >CLOCK<")); 
      }
      display.display();
    }
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
}


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    EJECUAR PROCESO DE TEMPORIZACION
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
 
void runTimer()
{
  float voltaje;
  static bool FLAG_punto_segundero;

  if( operation_mode!=OP_MODE_TIMER ){

    uint8_t pulsador_menos;
    uint8_t pulsador_mas;
    pulsador_menos = pulsador_MENOS.Read();
    pulsador_mas = pulsador_MAS.Read();
        
    if( pulsador_menos==PULSAC_CORTA || pulsador_mas==PULSAC_CORTA ){
      FLAG_run_clock = !FLAG_run_clock; 
      digitalWrite(PIN_TRIAC, FLAG_run_clock);
      digitalWrite(PIN_LED_TRIAC, FLAG_run_clock);
    }
    else if( FLAG_run_clock == false && (pulsador_menos==PULSAC_LARGA || pulsador_mas==PULSAC_LARGA) ){
      FLAG_run_clock = false;
      estado_actual=MQ_STOP;
    }
  }

  
  if( FLAG_run_clock == false && operation_mode==OP_MODE_TIMER ){ estado_actual=MQ_STOP; }
  
  if( contadorMedioSegundo!=momentoAnterior){
    momentoAnterior = contadorMedioSegundo;

    /* segundero basado en la ISR de TIMER 2 */
    if(  contadorMedioSegundo%2==0){  
      if(XTAL_SEGUNDOS%10==0){
        voltaje = comprobar_estado_bateria(0);
        PRINT(F("voltaje: ")); PRINTLN(voltaje);
      }   
      XTAL_SEGUNDOS++;  //para tener un segundero 
    }
    
    FLAG_punto_segundero = !FLAG_punto_segundero;
    display.clearDisplay();
    display.setTextColor(WHITE);

    if(!FLAG_run_clock && FLAG_punto_segundero){
      display.setCursor(12, 4);
      display.setTextSize(1);
      display.print(F("PAUSE"));
    }
    
    display.setCursor(60, 0);
    display.setTextSize(2);
    
    display.print(voltaje/1000);
    display.print(F("v"));

    display.setCursor(8, 24);
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

    if( operation_mode==OP_MODE_TIMER ){
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
    
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(12, 16);
    display.setTextSize(2);
    display.print(F("TIMER OFF"));
    display.setCursor(0, 40); 
    display.setTextSize(1);
    display.print(F("  Presiona ("));
    display.print(4-contadorReset);
    display.print(F(") veces "));
    display.setCursor(0, 52); 
    display.print(F("    para reiniciar   "));
    display.display();  
  }
  
  if(contadorReset>3){                  // Provocamos un 'reset'
    contadorReset=-1;
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
  /* Setup Timer2  para que junto a la funcion de atencion a su interrupcion
   * poder obtener una 'señal util' cada 500 ms */
  //TCCR1A = 0x00;
  TCCR1B = 0x00;        // deshabilita Timer2 mientras lo estamos ajustando
  TCNT1  = 0xE174; ;    //(57716) establecer el contador iniciandolo con este valor para contar 7820
  TIFR1  = 0x00;        // Timer2 INT Flag Reg: borrar la bandera de desbordamiento
  TIMSK1 = 0x01;        // Timer2 INT Reg: habilita la interrupcion de desbordamiento de Timer2
  TCCR1A = 0x00;        // Timer2 Control Reg A: Wave Gen Mode normal
  TCCR1B |= (1 << CS10)|(1 << CS12);    // 1024 prescaler de timer 1

}



//========================================================
// RUTINA DE INTERRUPCION PARA DESBORDAMIENTO DE TIMER 2
// El vector de desbordamineto es -->  TIMER1_OVF_vect
//========================================================

ISR(TIMER1_OVF_vect) 
{
  /* 
     Con el preescaler a 1024, contando desde 57716 en TCNT1 para contabilizar 7820 ciclos 
     obtenemos una señal que permitetemporizar 500ms con bastante exactitud
     Este Reloj por software adelanta xx segundos cada xx hora
     contando 7820 --> adelanta 0.14 segundo cada 1 hora
  */
  
  contadorMedioSegundo ++;
  if( operation_mode == OP_MODE_TIMER){ CuentaAtras_ISR(); }
  else{ Reloj_ISR(); }   

  TCNT1 = 0xE174;                 //0xE174 reset del contador iniciandolo con el valor 57716 para contar 7820
  TIFR1 = 0x00;                   // Timer1 INT Flag Reg: borrar la bandera de desbordamiento

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
  * Reloj relativametne preciso mediante software
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
  
  if(contadorHoras==0 && contadorMinutos==0 && contadorSegundos==0){
    /* por si se queda activo el servicio de ISR, salimos de esta rutina si el tiempo ha llegado a cero
     evitando asi que se produzcan valores negativos que generen erroes de representacion el el LCD */                                
    FLAG_run_clock = false;
    digitalWrite(PIN_TRIAC, LOW);  //por segurudad lo paro desde aqui, aunque lo haga tambien en el loop()
    digitalWrite(PIN_LED_TRIAC, LOW);
	return;
  }
  
  if(contadorMedioSegundo%2 == 0){                          // cada dos medios segundos, 
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





//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************
