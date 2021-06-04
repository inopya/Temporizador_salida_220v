# libreria ClickButton para el control de pulsadores

	version:  1.0 Fecha:    14/10/2018 
	version:  1.1 Fecha:    26/03/2020 
	  - Modificada deteccion del doble click 
      - Añadido metodo para poder controlar el tiempo entre clicks (en las dobles pulsaciones)
	  - Añadido parametro para poder usar configuracion pull-up (antigua version solo pull-down)
		
		
 # NOTAS DE LA VERSION
  
  Esta libreria permite el control de pulsadores en los que podremos detectar tres tipos de eventos 
  mediante el metodo Read() de los objetos ClickButton.
  Disponemos de varios parametros de configuracion:
  - uint8_t pin: el numero de pin en el que esta conectado nuestro pulsador
  - uint8_t antirrebotes: tiempo (ms) minimo para que una pulsacion no sea despreciada como un ruido/rebote del pulsador
  - uint16_t corta: tiempo (ms) para que una pulsacion sea considerada pulsacion corta 
  - uint16_t larga: tiempo (ms) para que una pulsacion sea considerada pulsacion larga 
  
  *Este valor es accesible explicitamente mediante su metodo DobleClick(uint16_t doble)*
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
	    Ejemplo de uso para la libreria ClickButton
  *******************************************************
   >> solo con el numero de pin:
   
      ClickButton mi_pulsador_1(uint8_t pin); el resto de parametros por defecto: son (30,150,380)
   
   
   >> o bien definiendo todos los parametros disponibles:
   
      ClickButton mi_pulsador_2(uint8_t pin, uint8_t antirrebotes, uint16_t corta, uint16_t larga);


   >> Modificar el tiempo entre clicks para deteccion de DobleClick (por defecto 110 ms):
   
      mi_pulsador_1.DobleClick(uint16_t doble)
      mi_pulsador_2.DobleClick(uint16_t doble)

   >> para recoger el estado de un pulsador hemos de llamar a su metodo Read():
   
      estado_pulsador = mi_pulsador_1.Read();
      0 --> sin pulsar o en el proceso de identificar una pulsacion
      1 --> pulsacion corta
      2 --> pulsacion doble
      3 --> pulsacion larga
	  
	  
