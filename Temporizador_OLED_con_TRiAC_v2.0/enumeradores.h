
/* ENUM TIPOS DE PULSACION */
enum tipoPulsacion 
{
  PULSAC_NULA       =  0,
  PULSAC_CORTA      =  1,   // 
  PULSAC_DOBLE      =  2,   // 
  PULSAC_LARGA      =  3,   // 
  PULSAC_MANTENIDA  =  9,   // necesitamos un tiempo mayor a 5*PULSAC_LARGA para que se detecte como mantenida
};



/* ENUM OPCIONES PROGRAMABLES */
enum  opcion_programacion
{
  PROG_TIMER    	=  0,   //
  PROG_CLOCK     	=  1,   //
  PROG_NTC      	=  2,   //
  PROG_PREHOT     =  3,   //	
};



/* ENUM OPERATION MODE */
enum  modo_operacion
{
  OP_MODE_TEST       =  0,   //
  OP_MODE_MANUAL     =  1,   // encendido apagado manual mediante pulsador
  OP_MODE_CLOCK      =  2,   // Encendido y contando tiempo (¿mezclar con modo manual?)
  OP_MODE_TIMER      =  3,   // modo principal y mas util, temporizador para desconexion
  OP_MODE_SELECT     =  4,
};



/* ENUM ESTADOS MAQUINA */
enum estados_maquina
{
  MQ_REPOSO           =   0,   //  inicio tras recibir corriente
  MQ_RESTART          =   1,   //  estado tras reinicio/restablecimiento de corriente
  MQ_PAUSE            =   2,   //  muestra mensaje de pausa/espera
  MQ_MODIFY_PROG      =   3,   //  entra en modo modificar programcion
  MQ_SELECT_SCREEN    =   4,   //  entra en modo modificar programcion

  MQ_MODIFY_HOUR      =  10,  
  MQ_MODIFY_MINUTE    =  11,
  
  MQ_START_TIMER       =  20,   //  inicion funcionamiento (carga de tiempos en los contadores)
  MQ_RUNNIG           =  21,   //  estado de ejecucion de tarea
  MQ_STOP             =  22,   //  fin de ciclo
  MQ_WAIT_RESET       =  23,   //  espera para reinicion/nueva ejecucion

  MQ_CLOCK            =  30,   //  modo Manual
  MQ_CLOCK_PAUSE      =  31,   //  modo Manual
  MQ_TEST             =  32,   //  modo test generico (multiproposito)
  
      
};
