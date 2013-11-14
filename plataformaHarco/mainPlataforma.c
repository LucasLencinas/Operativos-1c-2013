#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include "orquestador.h"


int main(int argc, char **argv){

	pthread_t orquestador;
	pthread_create(&orquestador, NULL, funcionOrquestador, argv[1]);
	pthread_join(orquestador,NULL);
	sleep(1);
	printf("Vas a morir Koopa, Uiiiiiii\n");
	sleep(1);
	//t_config* config= config_create(argv[1]);
	char* pathPedidos;//=config_get_string_value(config,"pedidosKoopa");
	if(argv[2]!=NULL){
		pathPedidos=argv[2];
		char* argumentos[3]={"koopa",pathPedidos,NULL};
		printf("3\n");
		sleep(1);
		printf("2\n");
		sleep(1);
		printf("1\n");
		sleep(1);

		printf("A punto de ejecutar al estupido y sensual Koopa\n");
		sleep(3);

		int returnExec=execv("koopa",argumentos);
		if (returnExec==-1)
			perror("Error al ejecutar execv");
	}
	else{
		printf("llamando a koopa\n");
		sleep(1);
		printf("aaaaa te la creiste sucio. Acordate de poner un parametros mas: pathPedidos\n");
	}

	return EXIT_SUCCESS;
}
