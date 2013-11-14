/*
 * estructuras.h
 *
 *  Created on: 29/04/2013
 *      Author: utnso
 */

#ifndef ESTRUCTURASNIVEL_H_
#define ESTRUCTURASNIVEL_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdint.h>

typedef struct {
	char* nombre;
	char simbolo;
	int instancias;
	int posX;
	int posY;
}t_caja;

typedef struct{
	char* nombre;
	int recovery;
	int t_chequeoDL;
	char* dir_orquestador;
	t_list* cajas;
	t_log* logger;
	t_list* personajes;
	char* miDireccion;
}t_nivel;

typedef struct unpersonaje {
	int socket_cliente;
	char identificador;
	int pos_x; int pos_y;
	char* nombre;
	t_list* objetivosCumplidods;
	char solicitud;
	char pendiente;
} unPersonaje;

typedef struct {
	uint8_t recx;
	uint8_t recy;
	char identificador;
	uint8_t instancias;
}__attribute__((__packed__)) t_recurso;

#endif /* ESTRUCTURASNIVEL_H_ */
