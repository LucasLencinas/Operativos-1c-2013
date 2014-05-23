/*
 ============================================================================
 Name        : umv.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <ctype.h>
#include "umv.h"

char* memoria;
t_list* segmentos;


int main(void) {
	char peticion[1024];
	char** arrayPeticion;
	char* lectura;
	int param;
	bool error = false;
	t_configuracion* config=   malloc(sizeof(t_configuracion));

	//cargarConfiguracion(config,getenv("ANSISOP_CONFIG"));
	cargarConfiguracion(config,"/home/utnso/Operativos-1c-2013/stacoverflow/umv/configuracion.txt");

	memoria = malloc(sizeof(char)*config->size_memoria);
	segmentos=list_create();
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->libre = false;
	segmento->offset=0;
	list_add(segmentos,segmento);


	printf("Inicio de la UMV\n");
	printf("Ingrese los comandos para operar sobre la memoria:\n");



	fgets(peticion,1024,stdin);
	arrayPeticion = string_split(peticion," ");

	while(!string_equals_ignore_case(arrayPeticion[0],"quit")){
		int numero;
		if(string_equals_ignore_case(arrayPeticion[0],"grabar")){

			for (param = 1; param < 5; param++){
				numero = atoi(arrayPeticion[param]);
				if(numero == 0 && arrayPeticion[param][0] != '0'){
					printf("Error en parametro numero %d.\n",param);
					error = true;
				}
			}
			if(!error){
				numero = grabar(atoi(arrayPeticion[1]),atoi(arrayPeticion[2]),atoi(arrayPeticion[3]),
						atoi(arrayPeticion[4]),arrayPeticion[5]);
				if(numero ==1)
					printf("Se grabo correctamente\n");
			}

		}else
			if(string_equals_ignore_case(arrayPeticion[0],"leer")){
				for (param = 1; param < 5; param++)
					numero = atoi(arrayPeticion[param]);
						if(numero == 0 && arrayPeticion[param][0] != '0'){
							printf("Error en parametro numero %d.\n",param);
							error = true;
						}
				if(!error){
					lectura = leer(atoi(arrayPeticion[1]),atoi(arrayPeticion[2]),
						atoi(arrayPeticion[3]),atoi(arrayPeticion[4]));
					printf("%s\n",lectura);
				}
			}else
				if(string_equals_ignore_case(arrayPeticion[0],"crearSegmento")){

				}else
					if(string_equals_ignore_case(arrayPeticion[0],"destruirSegmento")){

					}else
						if(string_equals_ignore_case(arrayPeticion[0],"retardo")){

						}else
							if(string_equals_ignore_case(arrayPeticion[0],"algoritmo")){

							}else
								if(string_equals_ignore_case(arrayPeticion[0],"compactacion")){

								}else
									if(string_equals_ignore_case(arrayPeticion[0],"dump")){

									}
		fgets(peticion,1024,stdin);
		arrayPeticion = string_split(peticion," ");
	}



	return EXIT_SUCCESS;
}


void cargarConfiguracion(t_configuracion* umvConfig,char* dir_config){

	t_config* config=config_create(dir_config);

	printf("%s\n",dir_config);
	printf("%d\n",config_keys_amount(config));

	umvConfig->size_memoria=config_get_int_value(config,"size_memoria");

}



