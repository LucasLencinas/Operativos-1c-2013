/*
 * funcionesAuxiliares.c
 *
 *  Created on: 15/04/2013
 *      Author: utnso
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "memoria.h"

extern t_list* lista_particiones;
extern int tamanioMemoria;

bool puedeEntrar(void* particionDeLista,int tamanio){
	t_particion* particion=(t_particion*)particionDeLista;
	if(particion->tamanio>=tamanio)
		return true;
	else
		return false;
}

bool particionVacia(void* particionDeLista){
	t_particion* particion=(t_particion*)particionDeLista;
	return particion->libre;

}

void particionarMemoria(t_memoria segmento,void* particionLista,int index,char id,
		int tamanio, char* contenido){
	t_particion* particionDeLista=(t_particion*)particionLista;
	if(particionDeLista->tamanio>tamanio){

		t_particion *particionNueva=(t_particion*)malloc(sizeof(t_particion));
		particionNueva->id=id;
		particionNueva->tamanio=tamanio;
		particionNueva->inicio=particionDeLista->inicio;
		particionNueva->libre=false;
		memcpy(segmento+particionDeLista->inicio,contenido,particionNueva->tamanio);
		particionNueva->dato= segmento+particionDeLista->inicio;

		list_add_in_index(lista_particiones,index,particionNueva);

		particionDeLista->inicio=particionNueva->inicio+particionNueva->tamanio;
		particionDeLista->tamanio-=particionNueva->tamanio;
		particionDeLista->dato = particionNueva->dato + tamanio;
	}
	else{//el tamanio de las particiones es el mismo
		memcpy(particionDeLista->dato,contenido,tamanio);
		particionDeLista->id=id;
		particionDeLista->libre=false;
	}
}

bool tieneMismoID(char id, void* particion){
	t_particion* particionAux =(t_particion*)particion;
	if(id==particionAux->id)
		return true;
	else
		return false;
}


void borrarParticion(void* particion){
	t_particion* unaParticion=(t_particion*)particion;
	unaParticion->id='\0';
	unaParticion->libre= true;
}

bool esElFinDelSegmento(void*particionLista){
	t_particion* particionDeLista=(t_particion*)particionLista;
	if(tamanioMemoria == particionDeLista->inicio+particionDeLista->tamanio)
		return true;
	else
		return false;
}
bool esLaUltimaParticionOcupada(void*particionLista){
	t_particion* particionDeLista=(t_particion*)particionLista;
	if(esElFinDelSegmento(particionLista) && !particionDeLista->libre)
		return true;
	else
		return false;
}

bool actualMejorQueBest(t_particion* particion_lista,t_particion* bestParticion){
	if(particion_lista->tamanio<bestParticion->tamanio)
		return true;
	else
		return false;
}



