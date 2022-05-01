# naiveOS

## Configuracion inicial
1. Instalar los paquetes ```nasm qemu gcc make```.

2. Armar el Toolchain ejecutando los comandos ```cd Toolchain``` y  ```make all``` desde el directorio raíz


3. Armar el Kernel

    Desde el directorio raiz correr ```make all```

4. Correr el kernel

    Desde el directorio raíz correr ```./run.sh```

## Uso del SO
Al bootear el sistema (basado en Barebonesx64 de RowDaBoat), se encontrará con una pantalla dividida horizontalmente en dos partes iguales. En ambas se podrá operar un shell simultáneamente, pudiéndose intercambiar entre ellas por medio de las flechas &#8593; y &#8595;. Se ingresa un comando presionando la tecla *enter*.
### Shell

La shell admite los siguientes comandos:
- **help:** muestra una descripción de todos los comandos disponibles en el sistema.
- **timedate:** despliega por pantalla la fecha y hora
- **inforeg:** despliega por pantalla los contenidos de los registros
- **printmem:** recibe un único argumento y despliega el contenido de 32 bytes de memoria comenzando por la dirección del argumento
- **multiple:** despliega el *modo de ventanas múltiples*. Se desarrolla a continuación.
-  **exception0:** ejecuta una rutina que genera la excepción de división por cero.
-  **exception6:** ejecuta una rutina que genera la excepción de código de operación inválido.

### Modo de ventanas múltiples
Se divide la pantalla en 4 partes iguales con diferentes aplicaciones. En las ventanas que permiten una entrada por teclado, se puede navegar entre ellas a través de las teclas &#8592;, &#8593;, &#8594; y &#8595;. Si se presiona la tecla *ESC* estando en este modo, entonces se puede volver al modo de Shell.
#### Sudoku (1)
Permite jugar una partida de Sudoku a través del teclado. Las entradas consisten de la letra de la fila, el número de columna, y el número a ingresar, todos separados por espacio (por ej: una entrada podría ser *'a 2 1'*). Las casillas preinicializadas no permiten modificación, y en caso de querer llenar una casilla con un número que no respeta las reglas del Sudoku, se muestra un mensaje de error. Los comandos que permite la entrada son:
-  **solve:** resuelve el sudoku por el usuario.
-  **reset:** reinicia el juego.
#### Reloj (2)
Simplemente es un reloj que despliega la hora en formato *hh:mm:ss* de forma continua.
#### Ahorcado (3)
Permite jugar un ahorcado a través del teclado. Las entradas pueden ser o bien letras mayúsculas o minúsculas o alguno de los siguientes comandos:
-  **solve:** resuelve el ahorcado por el usuario.
-  **reset:** reinicia el juego
#### Cronómetro (4)
Permite controlar un cronómetro a través del teclado. Se puede pausar y resumir a gusto del usuario. Los comandos que admite son:
-  **start:** inicia el cronómetro.
-  **stop:** detiene el cronómetro. Se puede reanudar con el comando *start*.
-  **reset:** reinicia el cronómetro.