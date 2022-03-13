

/*mmmmmmmmmmmmmmmmmmmmmm */
/*    MAPA DE PINES      */
/*mmmmmmmmmmmmmmmmmmmmmm */

 /* Arduino NANO pinout:
				 _______________
				|     |USB|     |
                |     |___|		|		
				|               |				
				|13           12|
				|3V3          11| CONTROL TRIAC
				|AREF         10| LED_TRIAC
	  SONDA NTC |A0            9|
				|A1    xxxx    8|
				|A2   xxxxxx   7|
				|A3  x 328P x  6| TECLA (+)ENTER
   OLED DISPLAY |A4   xxxxxx   5| TECLA (-)PROG
   OLED DISPLAY |A5    xxxx    4| 
				|              3| 
				|              2| 
				|5V          GND|
				|RST         RST|
				|GND   1/INT2/RX|
				|VIN   0/INT3/TX|
				|_______________|
		
*/

//#define DIGITAL_EXT         2   // DIGITAL EXTERNA

#define PIN_TECLA_PROG      5   // pulsador  (-)
#define PIN_TECLA_ENTER     6   // pulsador  (+)
#define PIN_AUTO_VREF       7   // salida para alimentar V_ref (sin uso)
#define PIN_LED_TRIAC      10   // luz que indica que el triac esta activo
#define PIN_TRIAC          11   // salida par el cotrol del TRIAC
#define LED_OnBoard        13   // (Libre) led OnBoard de la placa Arduino UNO
          
#define PIN_sonda_NTC      A0   // Sonda de temperatura para la yogurtera
