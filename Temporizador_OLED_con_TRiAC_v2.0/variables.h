
#include "enumeradores.h"

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/


volatile uint32_t contadorMedioSegundo = 0;   // Control de medios segundos. Por una parte controla el parpadeo del segundero
                                              // y por otra, cada dos ciclos modifica los segundos de nuestro reloj 


//int ESTADO_ANTERIOR = -1;             // Sin uso
int estado_actual = MQ_REPOSO;          // Contador para el control del 'momento' del programa en que nos encontramos
bool FLAG_program  = true;              // Indica si es el momento de refrescar el lcd para mostrar la hor
volatile bool FLAG_run_clock = false;   // Si estamos en modo reloj indicará si esta funcionando o en pausa, 
                                        // usada tambien para activar y desactivar el TRIAC si estamos funcionando o en pausa

uint8_t operation_mode = OP_MODE_SELECT;  // Por defecto el modo inicial es seleccionar el modo de funcionamiento



uint32_t XTAL_SEGUNDOS = 0;
uint32_t TIME_OUT = 0;

//uint32_t momentoActual = 0;
uint32_t momentoAnterior = 0;   // Para operaciones de comparacion de tiempos
//uint32_t contador_ticks = 0;

int tiemposTimer[3]={1, 0, 0}; // horas, minutos segundos, (por defecto 6 horas para hacer yogurt)


int8_t contadorHoras = 0;
int8_t contadorMinutos = 0;
int8_t contadorSegundos = 0;
float segmento;                 // Controla la porcion de barra de progreso que se va actualizando

bool FLAG_ntc_error = false;    // control de sonda (false = conectada)
bool FLAG_ntc_control = false;  // Determina si se usa la NTC apra el control de temperatura (y encendidos y apagados auto)
bool FLAG_precalentar = false;  // Determina si se debe alcanzar una temperatura umbral para empezar a temposizar el proceso
const uint16_t LIMITE_PRECALENTAR = 3600;  // 3600, En segundos, maximo una hora precalentando

uint32_t contadorPREcalentar=0; // Para contolar el tiempo del precalentamiento
uint32_t contadorON = 0;        // A nivel estadistico, para saber el tiempo que esta activo durante los controles auto
uint32_t contadorOFF = 0;       // A nivel estadistico, para saber el tiempo que esta OFF durante los controles auto

bool FLAG_ESTADO_TRIAC = false;
const uint8_t INIT_TEMP = 35;   // 39,Temperatura necesaria para iniciar la temporizacion del proceso de fermentacion
const uint8_t LOW_TEMP  = 41;   // 41,Temperatura minima admisible durante las operaciones control Auto con sonda
const uint8_t HIGH_TEMP = 46;   // 46,Temperatura maxima admisible durante las operaciones control Auto con sonda

//uint8_t AC_status = 0;        // Control de perdida de alimentacion en version con bateria


const char *textosMenu[] = {  
    "TIMER",
    "MANUAL",
	  "SONDA",
    "PREHOT"
};


static const unsigned char PROGMEM icono_timer[] = {  //16x24
0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x60, 0x06, 0x3F, 0xFC, 0x3F, 0xFC,
0x1F, 0xF8, 0x0F, 0xF0, 0x07, 0xE0, 0x03, 0xC0, 0x03, 0xC0, 0x06, 0x60, 0x0C, 0x30, 0x19, 0x18,
0x32, 0x0C, 0x60, 0x26, 0x63, 0x06, 0x47, 0xC2, 0x4F, 0xF2, 0x5F, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF
};

static const unsigned char PROGMEM icono_clock[] = {  //24x24
0x00, 0x7E, 0x00, 0x03, 0xFF, 0xC0, 0x07, 0xDB, 0xE0, 0x0E, 0x18, 0x70, 0x1C, 0x01, 0x38, 0x39,
0x10, 0x1C, 0x70, 0x10, 0x0E, 0x60, 0x10, 0x26, 0x68, 0x10, 0x06, 0xE0, 0x10, 0x07, 0xC0, 0x10,
0x03, 0xF0, 0x3F, 0x0F, 0xF0, 0x3F, 0x8F, 0xC0, 0x10, 0x03, 0xE0, 0x00, 0x07, 0x60, 0x00, 0x16,
0x64, 0x00, 0x06, 0x70, 0x00, 0x0E, 0x38, 0x00, 0x9C, 0x1C, 0x80, 0x38, 0x0E, 0x18, 0x70, 0x07,
0xDB, 0xE0, 0x03, 0xFF, 0xC0, 0x00, 0x7E, 0x00, 
};

static const unsigned char PROGMEM icono_ntc[] = {  //16x24
0x00, 0x00, 0x07, 0x80, 0x08, 0x40, 0x08, 0x58, 0x08, 0x40, 0x08, 0x5C, 0x08, 0x40, 0x08, 0x58,
0x08, 0x40, 0x08, 0x5C, 0x0B, 0x40, 0x0B, 0x58, 0x0B, 0x40, 0x0B, 0x5C, 0x0B, 0x40, 0x0B, 0x40,
0x13, 0x20, 0x27, 0x90, 0x2F, 0xD0, 0x2F, 0xD0, 0x2F, 0xD0, 0x37, 0xB0, 0x10, 0x20, 0x0F, 0xC0
};

static const unsigned char PROGMEM icono_prehot[] = {  //16x24
0x00, 0x00, 0x08, 0x88, 0x08, 0x88, 0x19, 0x98, 0x11, 0x10, 0x11, 0x10, 0x11, 0x10, 0x11, 0x10,
0x19, 0x98, 0x08, 0x88, 0x08, 0x88, 0x0C, 0xCC, 0x04, 0x44, 0x04, 0x44, 0x04, 0x44, 0x0C, 0xCC,
0x08, 0x88, 0x08, 0x88, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x1F, 0xFC, 0x00, 0x00, 0x00, 0x00
};
