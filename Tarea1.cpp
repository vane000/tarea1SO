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

bool presione_ctrl_c;

struct comandos {
	char **argumentos;
};

/* Para la creacion de comandos de monitoreo (item b.2) */
struct comando_monitor {
	char nombre_comando[30];
	char tipo_comando[8];
	char x[11];
	char z[11];
};


/* Realiza la pregunta de si se desea salir de la chell y el bool
   'presione_ctrl_c' permite manejar la respuesta desde el fgets del 'while(!salir_shell)' */
void control_c(){
	printf("\n");
	printf("¿Desea salir de la Shell?(Y/N)\n");
	presione_ctrl_c = true;
}


/* Parsea input para identidicar el comando y sus argumentos */
struct comandos * parsear_entrada(char * input) { 
	static struct comandos cmds_parseados[CANT_MAX_COMANDOS]; // (Global)
	char * aux_c = NULL;
	
    // Reserva memoria para los comandos
	for(int i = 0; i < CANT_MAX_COMANDOS ; ++i){
		cmds_parseados[i].argumentos = (char**)malloc(CANT_MAX_PARAMETROS * sizeof(char*));
		for(int j = 0; j < CANT_MAX_PARAMETROS; ++j){
			cmds_parseados[i].argumentos[j] = (char*)malloc(sizeof(char));
			cmds_parseados[i].argumentos[j] = NULL;
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


int main(){
	struct comando_monitor cm;
	bool agregar_comando;
    // Verifica que se ha creado al menos una vez un comando con el comando cmdmonset
	bool existe_nombre_comando = false; 
    // Inicializa variable global que reconoce si se ha presionado o no ctrl + c
	presione_ctrl_c = false;
    // Captura nombre del usuario para mostrarlo en el prompt
    size_t len = 30;
    char name[len];
    size_t ret = getlogin_r(name, len);
    // Establece la maxima cantidad de caracteres que tolera el input
    char input[MAX_TAM_IMPUT+1];
    struct comandos *cmds;
    // Condicion de salida de la shell
    bool salir_shell = false;

	// Captura la senal "CTRL+C" y la maneja con la funcion "control_c" (item b.1)
	signal(SIGINT, control_c);
	
    while(!salir_shell){
        // Prompt con nombre del usuario y color (item a.1)
		printf("\033[0;36m%s$: ",name);
        printf("\033[0m");

        // Limpia el buffer de entrada (stdin) antes de recibir instruccion (input)
		memset(input,'\0',strlen(input));

        // Espera la instruccion por entrada estandar, lo guarda en "input"
		if(fgets(input, sizeof(input), stdin) == NULL) break;

        // Manejo del "retorno" de la senal "CTRL+C", para volver a mostrar el prompt o finalizar la shell
		if(presione_ctrl_c == true){
			bool entrada_correcta = false;
			bool salir_ctrl_c = false;
			while(!entrada_correcta){
				if(strlen(input) == 2){
					if((input[0] == 'y')||(input[0] == 'Y')){
						salir_ctrl_c = true;
						entrada_correcta = true;	
					}else if((input[0] == 'n')||(input[0] == 'N')){
						salir_ctrl_c = false;
						entrada_correcta = true;
					}
				}
				if(entrada_correcta != true){
					printf("Ingrese una opción válida (Y/N)\n");
					memset(input,'\0',strlen(input));
					fgets(input, sizeof(input), stdin);
				}
			}
			if(salir_ctrl_c == true){
				break;
			}else{
				memset(input,'\0',strlen(input));
				input[0] = '\n';
				presione_ctrl_c = false;
			}
		} 

        // Al presionar "enter" sin un previo comando, imprime nuevamente el prompt (item a.4)
        if(input[0] == '\n') continue; 

        // Remueve el caracter de salto de linea al final del input
        if(input[strlen(input)-1] == '\n') { 
            input[strlen(input)-1] = '\0';
        }

        // Parsea el input (item 1.b)
        cmds = parsear_entrada(input); // Divide el input en un arreglo de comandos

        // Caso comando "exit" (item a.6)
		if (strcmp(cmds[0].argumentos[0], "exit") == 0){
			salir_shell = true;
        	break;
		}else{
            // Caso lectura comando de monitoreo creado (item b.2)
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

		// Caso creacion comando de monitoreo (item b.2)
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

        // Ejecuta comando (sin pipes, solo 1) (item 1.3)
        if((cantidadcmds == 1)&&(agregar_comando != true)){

            pid_t pid = fork();

            if (pid < 0){ // Error
                printf("No se pudo iniciar proceso\n");
                exit(1); 

            } else if (pid == 0){
                execvp(cmds[0].argumentos[0], cmds[0].argumentos);

                // Caso que el comando no se reconozca por execvp (item a.7)
                printf("Comando Desconocido\n");
                _exit(127); //Este exit es necesario para saber si fallo el execvp, Exit de comandos not found

            }else{
				// Padre espera por el hijo
                int status; 
	        	waitpid(pid, &status, 0); 
            }
        }

        // Ejecuta comandos (mas de 1, con pipes) (item a.5)
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

		    	// Caso que el comando no se reconozca por execvp (item a.7)
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