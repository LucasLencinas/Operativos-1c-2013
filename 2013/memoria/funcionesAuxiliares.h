/*
 * funcionesAuxiliares.h
 *
 *  Created on: 15/04/2013
 *      Author: utnso
 */

#ifndef FUNCIONESAUXILIARES_H_
#define FUNCIONESAUXILIARES_H_

#include <stdbool.h>
#include "memoria.h"

bool puedeEntrar(void* particionDeLista,int tamanio);
bool particionVacia(void* particionDeLista);
void particionarMemoria(t_memoria segmento,void* particionLista,int index,char id,int tamanio, char* contenido);
bool tieneMismoID(char id, void* particion);
void borrarParticion(void* particion);
bool esLaUltimaParticionOcupada(void*particionLista);
bool esElFinDelSegmento(void*particionLista);
bool actualMejorQueBest(t_particion* particion_lista,t_particion* bestParticion);

#endif /* FUNCIONESAUXILIARES_H_ */
