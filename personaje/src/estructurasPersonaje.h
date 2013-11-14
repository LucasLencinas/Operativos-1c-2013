/*
 * estructuras.h
 *
 *  Created on: 28/04/2013
 *      Author: utnso
 */

#ifndef ESTRUCTURASPERSONAJE_H_
#define ESTRUCTURASPERSONAJE_H_

#include <commons/collections/list.h>
#include <commons/log.h>

typedef struct {
	char* nombre;
	char simbolo;
	int vidas;
	char* dir_orquestador;
	t_list* niveles;
	t_log* logger;


}t_personaje;

typedef struct{
	char* nombre;
	int cantObjetivos;
	char **objetivos;		//Es un array de objetivos=['H','M','H','M']
}t_nivel;


typedef struct {
	int recx;
	int recy;
}t_posRecurso;




#endif /* ESTRUCTURASPERSONAJE_H_ */
