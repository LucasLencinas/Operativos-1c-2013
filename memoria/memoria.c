#include <stdlib.h>
#include <stdio.h>
#include "memoria.h"
#include "funcionesAuxiliares.h"
#include <string.h>
#include <commons/log.h>


t_list* lista_particiones;
int tamanioMemoria;

t_memoria crear_memoria(int tamanio) {
	t_log* logger = log_create("koopa.log","memoria",true,LOG_LEVEL_DEBUG);
	log_debug(logger,"CREAR MEMORIA");

	t_memoria segmento=(t_memoria)malloc(tamanio);
	tamanioMemoria=tamanio;
	t_particion *particion = (t_particion*)malloc(sizeof(t_particion));
	particion->inicio=0;
	particion->libre=true;
	particion->tamanio=tamanio;
	particion->dato=segmento;
	log_debug(logger,"la memoria debe ir de %p   a %p",particion->dato,particion->dato + tamanio);

	lista_particiones = list_create();
	list_add(lista_particiones,particion);

	log_debug(logger,"CREAR MEMORIA---FIN");
	log_destroy(logger);

	return segmento;
}

int almacenar_particion(t_memoria segmento, char id, int tamanio, char* contenido) {
	t_log* logger = log_create("koopa.log","memoria",true,LOG_LEVEL_DEBUG);
		log_debug(logger,"ALMACENAR");
	if(tamanioMemoria<tamanio){
		log_debug(logger,"ALMACENAR---FIN---Mas grande que memoria");
			log_destroy(logger);
		return -1;
	}
	t_link_element * particion_lista =lista_particiones->head;
	//t_particion* contenidoquesigue;
	//t_link_element * particion_sig_sig;
	int index=0;
	int bestIndex=-1;
	bool finMemoriaOcupada=false;

	t_particion* bestParticion = NULL;

	while(particion_lista!= NULL && !finMemoriaOcupada){
		if(tieneMismoID(id,particion_lista->data)){
			log_debug(logger,"ALMACENAR---FIN---Mismo id");
			log_destroy(logger);
			return -1;
		}
		if(particionVacia(particion_lista->data)){
			if(puedeEntrar(particion_lista->data,tamanio)){
				if(bestParticion==NULL){
					bestIndex=index;
					bestParticion=(t_particion*)particion_lista->data;
				}
				else
					if(actualMejorQueBest((t_particion*)particion_lista->data,bestParticion)){
						bestIndex=index;
						bestParticion=(t_particion*)particion_lista->data;
					}
				//particionarMemoria(segmento,particion_lista->data,index,id,
				//		tamanio, contenido);
			}
		}//en un momento me dice que el next es igual a una portcion
		//de memoria invalida, por eso pasa el seg fault
		finMemoriaOcupada= esLaUltimaParticionOcupada(particion_lista->data);
		particion_lista=particion_lista->next;

		//if(particion_lista!=NULL){
		//	contenidoquesigue=(t_particion*)particion_lista->data;
		//	particion_sig_sig = particion_lista->next;
		//}
		index++;
		}
	if(bestIndex!=-1){
		particionarMemoria(segmento,bestParticion,bestIndex,id,tamanio,contenido);
		log_debug(logger,"ALMACENAR---FIN---OK");// aca suele aparecer un Segmentation Fault
		log_destroy(logger);
		return 1;
	}
	else{
		log_debug(logger,"ALMACENAR---FIN---No hay lugar");
		log_destroy(logger);
		return 0;
	}
}

int eliminar_particion(t_memoria segmento, char id) {
	t_log* logger = log_create("koopa.log","memoria",true,LOG_LEVEL_DEBUG);
		log_debug(logger,"ELIMINAR");
	t_link_element *element = lista_particiones->head;
	while (element != NULL && !tieneMismoID(id,element->data)) {
		element = element->next;
	}
	if(element==NULL){
		log_debug(logger,"ELIMINAR---FIN---No esta ese ID");
		log_destroy(logger);
		return 0;
	}
	else{
	borrarParticion(element->data);
	log_debug(logger,"ELIMINAR---FIN---OK");
	log_destroy(logger);
	return 0;
	}
}

void liberar_memoria(t_memoria segmento) {
	free(segmento);
	list_destroy(lista_particiones);

}

t_list* particiones(t_memoria segmento) {
	t_log* logger = log_create("koopa.log","memoria",true,LOG_LEVEL_DEBUG);
		log_debug(logger,"PARTICIONES");

	t_list* lista_a_mostrar=list_create();
	t_link_element* elemento=lista_particiones->head;
	t_particion* particion;
	void* data;
	bool finMemoria=false;
	while(elemento!=NULL && !finMemoria){
		data=malloc(sizeof(t_particion));
		memcpy(data,elemento->data,sizeof(t_particion));
		particion=(t_particion*)data;
		log_debug(logger,"puntero: %p",particion->dato);
		list_add(lista_a_mostrar,data);
		if(esElFinDelSegmento(elemento->data))
			finMemoria=true;
		elemento=elemento->next;
	}
	log_debug(logger,"PARTICIONES---FIN");
	log_destroy(logger);
	return lista_a_mostrar;
}
