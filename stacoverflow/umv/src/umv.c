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
	char* peticion = malloc(sizeof(char)*200);
	char** arrayPeticion;
	char* lectura;
	int param;
	bool error = false;

	memoria = malloc(sizeof(char)*1000);
	segmentos=list_create();
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->libre = false;
	segmento->offset=0;
	list_add(segmentos,segmento);


	printf("Inicio de la UMV\n");
	printf("Ingrese los comandos para operar sobre la memoria:\n");
	scanf("%s", peticion);
	arrayPeticion = string_split(peticion," ");

	while(!string_equals_ignore_case(arrayPeticion[0],"quit")){

		if(string_equals_ignore_case(arrayPeticion[0],"grabar")){
			for (param = 1; param < 5; param++)
				if(!isdigit(arrayPeticion[param])){
					printf("Error en parametro numero %d.\nIntente nuevamente:\n",param);
					error = true;
				}
			if(!error)
				grabar(atoi(arrayPeticion[1]),atoi(arrayPeticion[2]),atoi(arrayPeticion[3]),
						atoi(arrayPeticion[4]),arrayPeticion[5]);

		}else
			if(string_equals_ignore_case(arrayPeticion[0],"leer")){
				for (param = 1; param < 5; param++)
					if(!isdigit(arrayPeticion[param])){
						printf("Error en parametro numero %d.\nIntente nuevamente:\n",param);
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
		scanf("%s", peticion);
		arrayPeticion = string_split(peticion," ");
	}



	return EXIT_SUCCESS;
}
