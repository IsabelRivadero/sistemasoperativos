# Laboratorio 1 ~ Sistemas Operativos

_Integrantes_

* Joaquín Origlia
* Isabel Rivadero   isarivadero@gmail.com
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

Para el `TAD Pipeline` utilizamos el mismo método que con `Scommand` a diferencia
que en vez de utilizar dos punteros, este necesitaba contar con una variable
`Booleana` que representara el wait del comando.

Y continuamos con el mismo procedimiento, nos guiamos con las funciones requeridas
que se encontraban en `command.c`.


##Execute
