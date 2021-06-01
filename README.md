# Temporizador con salia de 220v
Temporizador con TRIAC y salida 220v

Como siempre, cosas que surgen de la necesidad.
El antecedente, un temporizador electromecanico para enchufe que brillaba por su falta de exactitud.
Atraso de unos 25 minutos por hora programada.

La temporizacion la realizaba un pequeño reloj alimentado a 30v mediante un divisor de tension (y de una calidad pesima a tenor de su imprecision),
y la salida estaba controlada por un interruptor que era accionado mecanicamente durante el giro del reloj.

Ya disponia de una version de temporizador controlada con Arduino y salida a rele, sin RTC, pero de una precision aceptable, aproximadamente un adelando de 1,5 segundos por hora. Dicha temporizacion estaba basada en Timer2 de arduino y debido a que es un Timer de 8 bits montada reaprovechando la caja de un viejo router.

Así que disponer de este temporizador inutil a todas luces, era una oportunidad de oro para canibalizarlo y crear de un temporizador mendianamente preciso y con una aspecto fisico compacto y nada aparatoso. (mejorable, por supuesto).

Debido al limitado espacio disponible, se sustituye el arduino UNO de la version original por un NANO.
La salida a rele pasa a ser sustituida con TRIAC y optoacoplador. El LCD 16x2 se sustituye por una pequeña pantalla OLED de 128x64 pixeles, que la verdad sea dicha, nos permite una mayor cantidad de informacion. Eso sí, tambien es comunicacion I2C y hay que tener en cuenta que su escritura y refresco necesita algo ams de 40 ms (una autentica enormidad)

Mi version inicial de temporizador era un montaje multiproposito y diponia de algunas salidas para servos, por lo que se optó por Timer2 en lugar de Timer1.
En este caso, y debido a que se va a destinar especificamente a ser un temporizador para cargas de 220v se usa Timer1 que al ser de 16 bits permite sobradamente realizar el conteo necesario de una sola vez y evita tener que disponer de un contador secundario como el caso de Timer2 (Ademas el uso de Timer2 facilita mucho los ajustes de temporizacion).


