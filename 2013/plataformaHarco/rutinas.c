#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "planificador.h"
#include "colas.h"
#include "socket_y_serializadores/nipc.h"
#include "orquestador.h"

#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

void levantarPlanificador(t_orquestador* orquestador,char* nombreNivel){

	int fdPipe[2];
	if(pipe (fdPipe))
		perror("Error en la creacion del pipe");
	t_paramHiloPlanif* parametros=(t_paramHiloPlanif*)malloc(sizeof(t_paramHiloPlanif));
	t_planificadorParaLista* unPlanificador= (t_planificadorParaLista*)malloc(sizeof(t_planificadorParaLista));
	pthread_t thr_planificador;

	t_cola* colaBloqueados=cola_create();
	t_cola* colaListos= cola_create();
	t_list* personajes= list_create();
	parametros->colaBloqueados=colaBloqueados;
	parametros->colaListos=colaListos;
	parametros->personajes=personajes;
	parametros->puertoPorDondeEscucha=orquestador->puertoBase;
	orquestador->puertoBase++;
	parametros->quantum=orquestador->quantum;
	parametros->retardo=orquestador->retardo;
	parametros->lockDeQuantum=orquestador->lockDeQuantum;
	parametros->nombreNivelAsociado= malloc(strlen(nombreNivel)+1);
	parametros->fdPipe= fdPipe[0];
	strcpy(parametros->nombreNivelAsociado,nombreNivel);

	unPlanificador->colaBloqueados=colaBloqueados;
	unPlanificador->colaListos= colaListos;
	unPlanificador->personajes=personajes;
	unPlanificador->nombre=nombreNivel;	//FIXME tengo que hacer un strcpy!!!!!
	unPlanificador->fdPipe= fdPipe[1];
	pthread_create(&thr_planificador, NULL, (void*)&funcionPlanificador, (void*)parametros);

	unPlanificador->puertoPlanif=string_new();
	unPlanificador->puertoPlanif=string_from_format("%d",parametros->puertoPorDondeEscucha);



	list_add(orquestador->planificadores,unPlanificador);
}


void borrarPersonajeDePlanificador(t_planificador* planificador, int socket){
	/* Voy buscando por todos lados al perosnaje con ese socket y
	 * lo borro: pongo NULL o lo saco de las listas
	 */
	if(planificador->personajeActual!=NULL)
		if(planificador->personajeActual->socket == socket)
			planificador->personajeActual = NULL;


		int index=0;
		pthread_mutex_lock(planificador->colaListos->mutexCola);
		t_link_element* elementoLista = planificador->colaListos->queue->elements->head;	//Primer elemento de la lista
		t_personaje* personaje=NULL;

		while(elementoLista!=NULL){
			personaje=(t_personaje*)elementoLista->data;
			if(personaje->socket == socket){
				list_remove(planificador->colaListos->queue->elements,index);
				sem_wait(&planificador->colaListos->contador);
			}

			index++;
			elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
		}
		pthread_mutex_unlock(planificador->colaListos->mutexCola);

		index=0;
		pthread_mutex_lock(planificador->colaBloqueados->mutexCola);
		elementoLista = planificador->colaBloqueados->queue->elements->head;	//Primer elemento de la lista
		personaje=NULL;

		while(elementoLista!=NULL){
			personaje=(t_personaje*)elementoLista->data;
			if(personaje->socket == socket){
				list_remove(planificador->colaBloqueados->queue->elements,index);
				sem_wait(&planificador->colaBloqueados->contador);
			}

			index++;
			elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
		}
		pthread_mutex_unlock(planificador->colaBloqueados->mutexCola);
	}

void sacarPersonajeSegunSocket(t_list* personajes, int socket){ // lo saca de la lista general de personajes
	t_link_element* elementoLista = personajes->head;	//Primer elemento de la lista
	t_personaje* personaje=NULL;
	int index=0;

	while(elementoLista!=NULL){
		personaje=(t_personaje*)elementoLista->data;
		if(personaje->socket == socket)
			break;		//termina el while
		index++;
		elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
	}
	list_remove(personajes,index);
}

t_personaje* sacarPrimerPersonajeDelosQueSeBloquearon(t_planificadorParaLista* planificador){
	/*Busco el primer personaje de la lista de personajes generales que tambien este
	 * en la lista de personajes bloqueados
	 * */
	t_link_element* elementoLista = planificador->personajes->head;	//Primer elemento de la lista
	t_personaje* personaje=NULL;
	t_personaje* bloqueado=NULL;

	int index=0;

	while(elementoLista!=NULL){
		personaje=(t_personaje*)elementoLista->data;
		if((bloqueado = encontrarPersonajeBloqueado(planificador->colaBloqueados,personaje->nombre))!= NULL)
			break;		//termina el while
		index++;
		elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
	}
	if(bloqueado == NULL)
		return NULL;
	else{
		return list_remove(planificador->personajes,index);
	}
}


t_personaje* encontrarPersonajeBloqueado(t_cola* bloqueados, char* nombre){

	pthread_mutex_lock(bloqueados->mutexCola);
	t_link_element* elementoLista = bloqueados->queue->elements->head;	//Primer elemento de la lista
	t_personaje* personaje=NULL;
	int index=0;

	while(elementoLista!=NULL){ //mientras que sea el fin de la lista
		personaje=(t_personaje*)elementoLista->data;
		if((strcmp(personaje->nombre,nombre))==0)
			break;		//termina el while
		index++;
		elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
	}
	if(elementoLista!=NULL){
		personaje= list_remove(bloqueados->queue->elements,index);
		sem_wait(&bloqueados->contador);
		pthread_mutex_unlock(bloqueados->mutexCola);

		return personaje;
	}
	else{
		pthread_mutex_unlock(bloqueados->mutexCola);
		return NULL;
	}
}

