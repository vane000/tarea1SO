# tarea1SO

- Integrantes: Andres Lartiga
               Lina Cao
               Vanessa Suazo

Link de informe en latex: https://www.overleaf.com/read/gjdptkxtffjp#e00dec

----------------------------------------------------------------------------------------
##### COMPILACIÓN DEL ARCHIVO shell.c en linux: #####

	gcc Tarea1.cpp 


----------------------------------------------------------------------------------------
##### PARA CORRER LA SHELL: #####

	./a.out


----------------------------------------------------------------------------------------
##### PARA UTILIZAR LA SHELL: #####
En la misma terminal se imprimirá un prompt de color cian con su nombre de usuario,
esperando que ingrese por teclado los comandos que desee probar. Debe presionar ENTER
para comenzar a ejecutar la instrución.


----------------------------------------------------------------------------------------
##### COMANDOS FAVS #####
Ahora se describira el uso y funcionalidad de los comandos relacionados con favoritos en la shell personalizada implementada en C++. Los comandos "favs" permiten gestionar una lista de comandos favoritos, almacenarlos, cargarlos desde archivos y realizar operaciones como agregar, eliminar, buscar o ejecutar favoritos.

Comandos disponibles
1. favs crear <nombre_archivo>
Este comando crea un archivo para almacenar la lista de comandos favoritos. El archivo se utilizará para guardar y cargar los comandos favoritos.

	Uso: favs crear mis_favoritos.txt

Descripción: Crea el archivo especificado (mis_favoritos.txt) para almacenar comandos favoritos.

2. favs agregar <comando>
Agrega un nuevo comando a la lista de favoritos.


	Uso: favs agregar ls -la

Descripción: El comando ls -la se añade a la lista de comandos favoritos. Si el comando ya existe en la lista, no se añade nuevamente.

3. favs mostrar
Muestra la lista de comandos favoritos guardados en memoria.

	Uso: favs mostrar

Descripción: Imprime todos los comandos guardados en la lista de favoritos, cada uno con un número único que lo identifica.

4. favs guardar
Guarda la lista actual de comandos favoritos en el archivo previamente creado.

	Uso: favs guardar

Descripción: Guarda todos los comandos favoritos en el archivo asociado a la shell. Si ya se ha creado un archivo con favs crear, los comandos se almacenan allí.

5. favs cargar <nombre_archivo>
Carga la lista de comandos favoritos desde un archivo.

	Uso: favs cargar mis_favoritos.txt

Descripción: Carga todos los comandos guardados en el archivo mis_favoritos.txt a la memoria, reemplazando cualquier lista de favoritos previamente cargada.

6. favs borrar
Elimina todos los comandos favoritos en la lista actual.

	Uso: favs borrar

Descripción: Elimina todos los favoritos almacenados en memoria. Nota: Esto no afecta el archivo de favoritos a menos que se ejecute favs guardar.

7. favs eliminar <números>
Elimina uno o más comandos de la lista de favoritos, según el número que se les asignó.


	Uso: favs eliminar 1,3

Descripción: Elimina los comandos favoritos identificados con los números 1 y 3 de la lista de favoritos. Puedes eliminar varios favoritos a la vez separando los números con comas.

8. favs buscar <palabra_clave>
Busca comandos en la lista de favoritos que contengan una palabra clave específica.

	Uso: favs buscar ls

Descripción: Busca todos los comandos en la lista de favoritos que contienen la palabra clave ls y los muestra.

9. favs <número> ejecutar
Ejecuta un comando favorito según el número que tiene en la lista de favoritos.


	Uso: favs 0 ejecutar

Descripción: Ejecuta el comando favorito que tiene el número 2 en la lista.

##### Ejemplo de Uso Completo #####
Crear archivo de favoritos:

	favs crear mis_favoritos.txt

Agregar un comando a favoritos:

	favs agregar ls -la
    favs agregar ps aux

Mostrar lista de favoritos:

	favs mostrar

Buscar un comando en favoritos:

	favs buscar ls

Eliminar un favorito:

	favs eliminar 1

Guardar favoritos en archivo:

	favs guardar

Cargar favoritos desde un archivo:

	favs cargar mis_favoritos.txt

Ejecutar un favorito:

	favs 0 ejecutar 


##### Consideraciones #####
* Archivo de favoritos: Los comandos favoritos se guardan en un archivo especificado por el usuario. Es importante utilizar favs crear antes de intentar guardar o cargar favoritos.
* Gestión de favoritos: Puedes tener hasta 100 comandos favoritos en memoria y almacenarlos o cargarlos desde un archivo.
* Compatibilidad: Cualquier comando que se pueda ejecutar en la shell puede ser agregado a favoritos.

Este sistema de favoritos es una herramienta útil para guardar y reutilizar comandos frecuentes en la shell.

------------------------------------------------------------------------------------------
##### RECORDATORIOS #####

La función set_recordatorio se invoca cuando el usuario escribe el comando set recordatorio <tiempo> <mensaje>. El programa interpreta este comando, obtiene el tiempo y el mensaje de los argumentos ingresados por el usuario y establece el recordatorio utilizando la función set_recordatorio.

Ejemplo de Uso
El usuario puede configurar un recordatorio de la siguiente manera:

	set recordatorio 10 "Toma un descanso"

Este comando hará lo siguiente:

* Configurará un recordatorio para que se active en 10 segundos.
* Después de 10 segundos, el programa mostrará el mensaje en la consola:

	Recordatorio: Toma un descanso

##### Limitaciones #####
Sólo puede haber un recordatorio activo a la vez. Si se configura otro recordatorio antes de que el actual haya terminado, el nuevo sobrescribirá al anterior.
El tiempo del recordatorio se especifica en segundos, por lo que no es posible establecer recordatorios de menos de un segundo ni usar unidades de tiempo más precisas (como milisegundos).


------------------------------------------------------------------------------------------
##### PARA CERRAR LA SHELL: #####

Puede utilizar el comando: exit


------------------------------------------------------------------------------------------