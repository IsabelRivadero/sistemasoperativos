# Laboratorio 1 ~ Sistemas Operativos

_Integrantes_

* Joaquín Origlia
* Isabel Rivadero
* Ignacio Todeschini

---
## Introducción

Comenzamos el proyecto descargando y estudiando la estructura del esqueleto
otorgado por la cátedra, encontrándonos con algunas dificultades que fueron
resueltos por los docentes y otras buscando un poco de información adicional en internet.


## TAD's Scommand y Pipeline

TAD Scommand:

Comenzamos inspeccionando en primer lugar lo que se especificaba en en el pdf.

Para el `TAD Scommand` utilizamos la estructura conformada por dos punteros de tipo char de los cuales serán para las redirecciones de input y output. Y para la lista de comandos utilizamos la implementación `GList` que pertenece a la librería `GLib` que estaba entre las recomendadas dentro del pdf.

Decidimos utilizar esa ya que para la implementación del TAD a nuestro parecer fue la más amigable y comprensible tanto a nivel de las funciones predeterminada que trae consigo como la estructura del mismo.

En principio una vez, decidida las herramientas a utilizar, comenzamos a inspeccionar el archivo `command.h` ya que es el archivo que contiene las definición de las funciones requeridas.

TAD Pipeline:

Para el `TAD Pipeline` utilizamos el mismo método que con `Scommand` a diferencia
que en vez de utilizar dos punteros, este necesitaba contar con una variable
`Booleana` que representara el wait del comando.

Y continuamos con el mismo procedimiento, nos guiamos con las funciones requeridas
que se encontraban en `command.c`.

## Execute

En el módulo de `Execute` en primera instancia leímos el archivo de pdf y el archivo `execute.h` para saber sobre la función que debíamos implementar.

Nos encontramos con dificultades para comprender acerca del concepto de `file descriptor` y como funcionan. Para el cual recurrimos tanto a internet.

En cuanto a las funciones **fork()**, **wait()**, **execvp()**. Pudimos comprender el uso más fácilmente de las funciones **fork()** y **wait()** mientras que para la función **execvp** aparte de leer el `man execvp` tuvimos que buscar más cantidad de información y ejemplos en internet.
Luego, con las funciones **open()** y **close()** fueron más sencillas de comprender.

En cuanto a la estructura del código en principio, se pensó que era mejor primero configurar los `Input` y en el ultimo caso el `Output` ya que en caso que existiese una redirección de cualquiera de ellos, se espere por canal y no por el standard.
Continuando se realiza un **fork()** el cual según lo que retorne se llevará a cabo una secuencia de pasos, ya sea en caso de ser negativo `valor -1` se salga con un **exit(1)** haciendo un print sobre el error, en el caso de que sea `valor 0` establecemos los argumentos y transformamos en la estructura pedida por la función **execvp**, terminando con la ejecución haremos un **free()** de la memoria pedida en ese trozo de código.
Y para otro caso, establecemos un **wait(NULL)**
Terminando por retirar el `primer comando` de la lista del pipelist.
Una vez ejecutado todo, finalizamos el execute restaurando los canales de entrada y salida a sus standard.


## Builtin

Este módulo a diferencia del `Execute` fue un poco más sencillo de tratar.
Pues leímos como en todos los casos anteriores el archivo `builtin.h` y llevamos a cabo la implementación de acuerdo a algunos datos que nos dieron los profesores en el pdf, además de la diferenciación entre un `Comando Interno` y los otros comandos.

Principalmente en `builtin_is_internal` utilizamos la función **strcmp()** que nos la provee la librería `<string.h>`.
Y en `builtin_exec` al igual que en `builtin_is_internal` colocamos un **assert()** para cumplir con lo que se requería en el archivo `builtin.h` y de allí colocamos dos condiciones el primer caso para el comando **cd** y el segundo para el comando **exit** en el cual se ejecutarán pasos distintos de acuerdo a cada caso.
