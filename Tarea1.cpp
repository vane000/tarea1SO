#include <unistd.h>
#include <signal.h>
#include <stdlib.h>   
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_TAM_IMPUT 100
#define CANT_MAX_PARAMETROS 10
#define CANT_MAX_COMANDOS 3
#define MAX_FAVS 100
#define MAX_CMD_LEN 100

char recordatorio[100];

struct comandos {
	char **argumentos; //array de strings
};

/* Para la creacion de comandos de monitoreo  */
struct comando_monitor {
	char nombre_comando[30];
	char tipo_comando[8];
	char x[11];
	char z[11];
};

// Estructura para los favoritos
struct Favorito {
    int numero;
    char comando[MAX_CMD_LEN];
};

struct Favorito lista_favoritos[MAX_CMD_LEN];
int total_favoritos = 0;
char archivo_favoritos[MAX_CMD_LEN]; // Ruta del archivo para almacenar favoritos


/* Parsea input para identidicar el comando y sus argumentos */
struct comandos *parsear_entrada(char * input) { //usuario escribe algo y transf. las entras a llamada sist.
	static struct comandos cmds_parseados[CANT_MAX_COMANDOS]; // (Global)
	char * aux_c = NULL;
	
    // Reserva memoria para los comandos
	for(int i = 0; i < CANT_MAX_COMANDOS ; ++i){
		cmds_parseados[i].argumentos = (char**)malloc(CANT_MAX_PARAMETROS * sizeof(char*));
		for(int j = 0; j < CANT_MAX_PARAMETROS; ++j){
			cmds_parseados[i].argumentos[j] = (char*)malloc(sizeof(char));//reserva memoria
			cmds_parseados[i].argumentos[j] = NULL;//?sobre escribiendo puntero?arriba  no se recupera
		}	
	}

	// Realiza el parseo
    for(int i = 0; i < CANT_MAX_COMANDOS; i++) {
    	for(int j = 0; j < CANT_MAX_PARAMETROS; ++j){

	        aux_c = strsep(&input, " "); // Separando comandos y parametros
	        if (aux_c == NULL){
	        	cmds_parseados[i].argumentos[j] = aux_c; // NULL indica donde termina el comando
	        	return cmds_parseados;
	        }
	        else if (strcmp(aux_c,"|") == 0 ) break; // Se ignoran los "|" (no se guardan)

	        cmds_parseados[i].argumentos[j] = aux_c;
		}
    }
	return cmds_parseados;
}


/* Ejecuta comandos leyendo desde un descriptor de archivo "descIN" y escribiendo
   en un descriptor "descOUT" */
int usar_pipe(int descIN, int descOUT, struct comandos cmds){
	pid_t pid;
	if((pid = fork()) == 0){
		if(descIN != 0){
			dup2(descIN,0);
			close(descIN);
		}
		if(descOUT != 1){
			dup2(descOUT,1);
			close(descOUT);
		}
		return execvp(cmds.argumentos[0],cmds.argumentos);
	}
	return pid;
}


// Función para agregar un comando a favoritos
void agregar_favorito(char *comando) {

	
    // Verifica si el comando ya está en favoritos
    for (int i = 0; i < total_favoritos; i++) {
        if (strcmp(lista_favoritos[i].comando, comando) == 0) {
            //printf("El comando '%s' ya está en favoritos.\n", comando);
            return;
        }
    }

	// buscar indice mas pequeño disponible
	int nuevo_indice = 0;
	for(int i = 0; i < total_favoritos; i++){
		if(lista_favoritos[i].numero == nuevo_indice){
			nuevo_indice++;
			i = 0;
		}	
	}

    // Si no está en favoritos, lo agrega
    if (total_favoritos < MAX_FAVS) {
        lista_favoritos[total_favoritos].numero = nuevo_indice;
        strcpy(lista_favoritos[total_favoritos].comando, comando);
        total_favoritos++;
        //printf("Comando '%s' agregado a favoritos.\n", comando);
    } else {
        //printf("Error: No se pueden agregar más favoritos.\n");
    }
}

void eliminar_favoritos(char * numeros){
	char *cursor;
    cursor = strtok (numeros,",");
    while (cursor != NULL) {
        int num = atoi(cursor);
		
		for(int i = 0; i < total_favoritos; i++){
			if(lista_favoritos[i].numero == num){
				lista_favoritos[i] = lista_favoritos[--total_favoritos];
			}	
		}
		//array[removeIndex] = array[--logicalSize];

        cursor = strtok (NULL, ",");
    }
}

// Función para mostrar los favoritos
void mostrar_favoritos() {
    if (total_favoritos == 0) {
        printf("No hay comandos favoritos.\n");
        return;
    }

    printf("Lista de comandos favoritos:\n");
    for (int i = 0; i < total_favoritos; i++) {
        printf("%d: %s\n", lista_favoritos[i].numero, lista_favoritos[i].comando);
    }
}

void buscar_favoritos(char* palabra) {

	bool alguno = false;
	for (int i = 0; i < total_favoritos; i++) {
		if (strstr(lista_favoritos[i].comando, palabra) == NULL) continue;

		alguno = true;
        printf("%d: %s\n", lista_favoritos[i].numero, lista_favoritos[i].comando);
    }

	if(!alguno) printf("No se encontro ningun comando.\n");
}

// Función para guardar los favoritos en un archivo
void guardar_favoritos() {
    FILE *archivo = fopen(archivo_favoritos, "w"); // Abrir en modo append para agregar comandos nuevos
    if (archivo == NULL) {
        printf("Error al abrir el archivo para guardar los favoritos.\n");
        return;
    }

    for (int i = 0; i < total_favoritos; i++) {
        fprintf(archivo, "%s\n", lista_favoritos[i].comando);
    }

    fclose(archivo);
    printf("Favoritos guardados en %s.\n", archivo_favoritos);
}

// Función para cargar favoritos desde un archivo
void cargar_favoritos() {
    FILE *archivo = fopen(archivo_favoritos, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo para cargar los favoritos.\n");
        return;
    }

    char comando[MAX_CMD_LEN];
    total_favoritos = 0; // Reinicia la lista en memoria antes de cargar
    while (fgets(comando, sizeof(comando), archivo) != NULL) {
        comando[strcspn(comando, "\n")] = '\0'; // Remueve el salto de línea
        agregar_favorito(comando);
    }

    fclose(archivo);
    printf("Favoritos cargados desde %s.\n", archivo_favoritos);
}

// Función para eliminar todos los favoritos
void borrar_favoritos() {
    total_favoritos = 0;
    printf("Todos los favoritos han sido eliminados.\n");
}

// Función para manejar el comando "favs"
void manejar_comando_favs(char **argumentos) {
    if (strcmp(argumentos[1], "crear") == 0) {
        strcpy(archivo_favoritos, argumentos[2]); // Establecer el nombre del archivo
        // Crear el archivo vacío (o sobrescribir si ya existe)
        FILE *archivo = fopen(archivo_favoritos, "w");
        if (archivo == NULL) {
            printf("Error al crear el archivo de favoritos: %s\n", archivo_favoritos);
        } else {
            printf("Archivo de favoritos creado: %s\n", archivo_favoritos);
            fclose(archivo);
        }
    } else if (strcmp(argumentos[1], "agregar") == 0) {
        char comando[MAX_CMD_LEN] = "";
        for (int i = 2; argumentos[i] != NULL; i++) {
            strcat(comando, argumentos[i]);
            strcat(comando, " ");
        }
        agregar_favorito(comando);
    } else if (strcmp(argumentos[1], "mostrar") == 0) {
        mostrar_favoritos();
    } else if (strcmp(argumentos[1], "guardar") == 0) {
        guardar_favoritos();
    } else if (strcmp(argumentos[1], "cargar") == 0) {
		strcpy(archivo_favoritos, argumentos[2]); 
        cargar_favoritos();
    } else if (strcmp(argumentos[1], "borrar") == 0) {
        borrar_favoritos();
	} else if (strcmp(argumentos[1], "eliminar") == 0) {
        eliminar_favoritos(argumentos[2]);
    } else if (strcmp(argumentos[1], "buscar") == 0) {
        buscar_favoritos(argumentos[2]);
    } else if (strcmp(argumentos[2], "ejecutar") == 0) {
        int num = atoi(argumentos[1]);
		
		if(total_favoritos < 1){
        	printf("No hay favoritos cargados.\n");
		}

		bool encontrado = false;

		for(int i = 0; i < total_favoritos; i++){
			if(lista_favoritos[i].numero == num){
				char input[MAX_TAM_IMPUT+1];
				comandos * cmds;
				encontrado = true;
				
				strcpy(input, lista_favoritos[i].comando);
				cmds = parsear_entrada(input);

				pid_t pid = fork();

				if (pid < 0){ // Error
					printf("No se pudo iniciar proceso\n");
					exit(1); 

				} else if (pid == 0){
					execvp(cmds[0].argumentos[0], cmds[0].argumentos);

					// Caso que el comando no se reconozca por execvp 
					printf("Comando Desconocido\n");
					_exit(127); //Este exit es necesario para saber si fallo el execvp, Exit de comandos not found

				}else{
					// Padre espera por el hijo
					int status; 
					waitpid(pid, &status, 0); 
				}
			}
		}

		if(!encontrado) printf("No se encontro un comando favorito con el numero indicado\n");

    } else {
        printf("Comando favs desconocido: %s\n", argumentos[1]);
	}
}

// Función para establecer recordatorio
void set_recordatorio(int segundos, char *mensaje) {
    printf("Recordatorio establecido para %d segundos: %s\n", segundos, mensaje);
    strcpy(recordatorio, mensaje);  // Copia el mensaje en la variable global
    alarm(segundos);                // Establece la alarma
}

void alarm_handler(int sig){
    printf("Recordatorio: %s\n", recordatorio);  // Muestra el recordatorio
}

int main(){
	struct comando_monitor cm;
	bool agregar_comando;
    // Verifica que se ha creado al menos una vez un comando con el comando cmdmonset
	bool existe_nombre_comando = false; 
    // Captura nombre del usuario para mostrarlo en el prompt
    size_t len = 30;
    char name[len];
    size_t ret = getlogin_r(name, len);
    // Establece la maxima cantidad de caracteres que tolera el input
    char input[MAX_TAM_IMPUT+1];
	char copia_input[MAX_TAM_IMPUT+1];
    struct comandos *cmds;
    // Condicion de salida de la shell
    bool salir_shell = false;

	// Captura la señal de la alarma y la maneja con la funcion "alarm_handler"
	signal(SIGALRM, alarm_handler);
	
    while(!salir_shell){
        // Prompt con nombre del usuario y color
		printf("\033[0;36m%s$: ",name);
        printf("\033[0m");

        // Limpia el buffer de entrada (stdin) antes de recibir instruccion (input)
		memset(input,'\0',strlen(input));

        // Espera la instruccion por entrada estandar, lo guarda en "input"
		if(fgets(input, sizeof(input), stdin) == NULL) break; 

        // Al presionar "enter" sin un previo comando, imprime nuevamente el prompt 
        if(input[0] == '\n') continue; 

        // Remueve el caracter de salto de linea al final del input
        if(input[strlen(input)-1] == '\n') { 
            input[strlen(input)-1] = '\0';
        }

        // Parsea el input 
		strcpy(copia_input, input);
        cmds = parsear_entrada(input); // Divide el input en un arreglo de comandos

        // Caso comando "exit" 
		if (strcmp(cmds[0].argumentos[0], "exit") == 0){
			salir_shell = true;
        	break;
		} else if (strcmp(cmds[0].argumentos[0], "favs") == 0){
			manejar_comando_favs(cmds[0].argumentos);
            continue;
		} else if (strcmp(cmds[0].argumentos[0], "set") == 0 && strcmp(cmds[0].argumentos[1], "recordatorio") == 0) {
            int segundos = atoi(cmds[0].argumentos[2]);
    
        // Construir el mensaje completo a partir de todos los argumentos restantes
            char mensaje[100] = "";
            for (int i = 3; cmds[0].argumentos[i] != NULL; i++) {
                strcat(mensaje, cmds[0].argumentos[i]);
				strcat(mensaje, " ");
			}
			set_recordatorio(segundos, mensaje);
			continue;
			}else {
            // Caso lectura comando de monitoreo creado 
			if((strcmp(cmds[0].argumentos[0], cm.nombre_comando) == 0)&&(existe_nombre_comando == true)){
				char monit[60];
				memset(monit,'\0',strlen(monit));
				strcpy(monit,cm.tipo_comando);
				strcat(monit, " ");
				strcat(monit, cm.x);
				strcat(monit, " ");
				strcat(monit, cm.z);
				strcat(monit, " ");
				strcat(monit, " > log.txt");
				int a = system("touch log.txt");
				int r = system(monit);
				printf("\033[0;34m");
				printf("\n");
				printf("Archivo log creado satisfactoriamente\n");
				continue;
			}
		}

        // Cuenta los comandos para saber si usar pipes o no
        int cantidadcmds = 0;
        for(int i = 0; i < CANT_MAX_COMANDOS; ++i){
            if(cmds[i].argumentos[0] != NULL) cantidadcmds++;
        }

		// Caso creacion comando de monitoreo 
		agregar_comando = false;
		if(cantidadcmds == 1){
			if(strcmp(cmds[0].argumentos[0], "cmdmonset") == 0){
				if(cmds[0].argumentos[1] != NULL){
					if((strcmp(cmds[0].argumentos[2], "vmstat") == 0)||(strcmp(cmds[0].argumentos[2], "netstat") == 0)){
						if(cmds[0].argumentos[3] != NULL){
							if(cmds[0].argumentos[4] != NULL){
								int x = atoi(cmds[0].argumentos[3]);
								int z = atoi(cmds[0].argumentos[4]);
								if((x >= 0)&&(z >= 0)){
									memset(cm.nombre_comando,'\0',strlen(cm.nombre_comando));
									strcpy(cm.nombre_comando,cmds[0].argumentos[1]);
									strcpy(cm.tipo_comando,cmds[0].argumentos[2]);
									strcpy(cm.x,cmds[0].argumentos[3]);
									strcpy(cm.z,cmds[0].argumentos[4]);
									agregar_comando = true;
									existe_nombre_comando = true;
								}
							}
						}
					}
				}
			}
		}

        // Ejecuta comando (sin pipes, solo 1) 
        if((cantidadcmds == 1)&&(agregar_comando != true)){

			agregar_favorito(copia_input);

            pid_t pid = fork();

            if (pid < 0){ // Error
                printf("No se pudo iniciar proceso\n");
                exit(1); 

            } else if (pid == 0){
                execvp(cmds[0].argumentos[0], cmds[0].argumentos);

                // Caso que el comando no se reconozca por execvp 
                printf("Comando Desconocido\n");
                _exit(127); //Este exit es necesario para saber si fallo el execvp, Exit de comandos not found

            }else{
				// Padre espera por el hijo
                int status; 
	        	waitpid(pid, &status, 0); 
            }
        }

        // Ejecuta comandos (mas de 1, con pipes) 
	    if(cantidadcmds > 1){
	    	pid_t pid;

	    	if((pid = fork()) == 0){ // Hijo	    	
		    	int in, descriptor[2];
		    	in = 0;
				
				// Comunicacion con pipes
		    	for(int i = 0; i < cantidadcmds - 1; ++i){ //ultimo comando para despues 
		    		pipe(descriptor); // Crea los descriptores de archivo
		    		usar_pipe(in, descriptor[1], cmds[i]); 
		    		close(descriptor[1]);
		    		in = descriptor[0]; //Almacenamos el in de esta pipe, para pasarlo al siguiente comando a ejecutar
		    	}

		    	// Ultimo comando
		    	if(in != 0){
		    		dup2(in,0);
		    		close(in);
		    	}

		    	execvp(cmds[cantidadcmds - 1].argumentos[0], cmds[cantidadcmds - 1].argumentos);

		    	// Caso que el comando no se reconozca por execvp 
		        printf("Comando Desconocido\n");
		        _exit(127); // Caso en que execvp produce un fallo

	    	}else{
				// Padre espera por el hijo
                int status; 
	        	waitpid(pid, &status, 0); 
            }
	    }
    }
}