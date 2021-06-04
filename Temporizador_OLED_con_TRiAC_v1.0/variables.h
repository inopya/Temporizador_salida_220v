
#include "enumeradores.h"

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/


volatile uint32_t contadorMedioSegundo = 0;   // control de medios segundos. 
                                              // Por una parte controla el parpadeo  de los puntos del segundero
                                              //  y por otra, cada dos ciclos modifica los segundos de nuestro reloj 


  
int estado_actual = MQ_RESTART;    

bool FLAG_program  = true;  
volatile bool FLAG_run_clock = false;

uint8_t operation_mode = OP_MODE_SELECT;



uint32_t XTAL_SEGUNDOS = 0;

uint32_t momentoAnterior=0;


//por defecto temporizador programado a 10 segundos para pruebas rapidas de DEBUG
int tiemposTimer[3]={0, 0, 10};  // horas, minutos segundos


int8_t contadorHoras=0;
int8_t contadorMinutos=0;
int8_t contadorSegundos=0;
float segmento;   // controla la porcion de barra de progreso que se va actualizando
