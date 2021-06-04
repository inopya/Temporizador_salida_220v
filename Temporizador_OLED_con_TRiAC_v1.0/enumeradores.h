
/* ENUM TIPOS DE PULSACION */
//debemos usar la libreria inopya para pulsadores
enum tipoPulsacion 
{
  PULSAC_NULA       =  0,   // estos valores son los codigos que devuelve la libreria, no cambiar
  PULSAC_CORTA      =  1,   // 
  PULSAC_DOBLE      =  2,   // 
  PULSAC_LARGA      =  3,   // 
  PULSAC_MANTENIDA  =  9,   // necesitamos un tiempo mayor a 5*PULSAC_LARGA para que se detecte como mantenida
};


/* ENUM OPERATION MODE */
enum  modo_operacion
{
  OP_MODE_CLOCK      =  1,   // Encendido y contando tiempo 
  OP_MODE_TIMER      =  2,   // modo principal y mas util, temporizador para desconexion
  OP_MODE_SELECT     =  3,   // sin modo, esperando seleccion por parte del usuario
};



/* ENUM ESTADOS MAQUINA */
enum estados_maquina
{
  MQ_RESTART          =   1,   //  estado tras reinicio/restablecimiento de corriente
  MQ_PAUSE            =   2,   //  muestra mensaje de pausa/espera
  MQ_MODIFY_PROG      =   3,   //  entra en modo modificar programacion
  MQ_SELECT_SCREEN    =   4,   //  entra en pantalla apra elegir modo RELOJ/TEMPORIZADOR
  MQ_WAIT_RESET       =   5,   //  espera para reinicio/nueva ejecucion

  MQ_MODIFY_HOUR      =  10,   //  modificando horas
  MQ_MODIFY_MINUTE    =  11,   //  modificando minutos
   
  MQ_START_TIMER      =  20,   //  inicio de funcionamiento (carga de tiempos en los contadores)
  MQ_RUNNIG           =  21,   //  estado de ejecucion de tarea (reloj/timer)
  MQ_STOP             =  22,   //  fin de tarea
  MQ_CLOCK            =  23,   //  modo reloj (modo manual)
  
  MQ_TEST             =  30,   //  modo test (sin uso)
  
      
};
