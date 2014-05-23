/*
 * umv.h
 *
 *  Created on: 10/05/2014
 *      Author: utnso
 */

#ifndef UMV_H_
#define UMV_H_

#include <stdbool.h>

typedef struct{
	int pid;
	int sid;
	int offset;
	bool libre;
}t_segmento;

typedef struct{
	int size_memoria;
}t_configuracion;


void cargarConfiguracion(t_configuracion* config,char* dir_config);
char * leer(int pid, int base,int offset,int tamanio);
int grabar(int pid, int base,int offset,int tamanio,char* contenido);





#endif /* UMV_H_ */
