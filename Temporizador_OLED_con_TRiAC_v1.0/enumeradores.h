
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
  OP_MODE_CLOCK,      // Encendido y contando tiempo 
  OP_MODE_TIMER,      // modo principal y mas util, temporizador para desconexion
  OP_MODE_SELECT,     // sin modo, esperando seleccion por parte del usuario
};



/* ENUM ESTADOS MAQUINA */
enum estados_maquina
{
  MQ_RESTART,         //  estado tras reinicio/restablecimiento de corriente
  MQ_PAUSE,           //  muestra mensaje de pausa/espera
  MQ_MODIFY_PROG,     //  entra en modo modificar programacion
  MQ_SELECT_SCREEN,   //  entra en pantalla apra elegir modo RELOJ/TEMPORIZADOR
  MQ_WAIT_RESET,      //  espera para reinicio/nueva ejecucion

  MQ_MODIFY_HOUR,     //  modificando horas
  MQ_MODIFY_MINUTE,   //  modificando minutos
   
  MQ_START_TIMER,     //  inicio de funcionamiento (carga de tiempos en los contadores)
  MQ_RUNNIG,          //  estado de ejecucion de tarea (reloj/timer)
  MQ_STOP,            //  fin de tarea
  MQ_CLOCK,           //  modo reloj (modo manual)
  
  MQ_TEST,            //  modo test (sin uso)  
};
