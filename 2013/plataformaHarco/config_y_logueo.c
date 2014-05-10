
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

#include "planificador.h"
#include "orquestador.h"
#include "colas.h"
#include <pthread.h>

void cargarConfiguracionOrquestador(t_orquestador* orquestador,char* configPath){
	/*MODELO DE ARCHIVO DE CONFIG PARA QUE ANDE:
	 * 		retardo=500000		aclaracion: es en microsegundo, por eso es tan grande el numero
	 * 		quantum=3
	 * 		puertoBase=5000		aclaracion: trata de usar ese siempre asi no hay problemas despues por las dudas
	 * 		logPath=/home/utnso.....
	 * 		miPuerto=5111
	 * */
	t_config* configuracion=config_create(configPath);
	orquestador->retardo=config_get_int_value(configuracion,"retardo");
	orquestador->quantum=malloc(sizeof(int));
	pthread_rwlock_init(&orquestador->lockDeQuantum,NULL);
	*orquestador->quantum=config_get_int_value(configuracion,"quantum");
	orquestador->puertoBase=config_get_int_value(configuracion,"puertoBase");

	orquestador->logger=log_create("orquestador.log","orquestador",true,LOG_LEVEL_DEBUG);
	orquestador->miPuerto=config_get_int_value(configuracion,"miPuerto");
	//-------Creo listas por ahora pero despues se cambiara a colas o algo con mutex para la sincronizacion CAPAZ...
	orquestador->personajes=list_create();
	orquestador->planificadores=list_create();
	orquestador->niveles=list_create();

	//Ejemplo de logueo, ASI DE SIMPLE, el segundo parametro es un char*
	log_debug(orquestador->logger,"//////////////////////////////////////COMIENZO DEL ORQUESTADOR//////////////////////////////////////");
	config_destroy(configuracion);
}


/*
 * @DESC: Crea un string dada una cola y el nombre de la misma. Formato: "Listos: Mario <- Sonic <- Koopa"
 */
char* stringColaPersonajes(t_cola* cola,char* nombreCola){

	//Ver la sincronizacion, faltan los mutex!!!!!!
	char* string = string_new();
	t_list* listaAux= list_create();
	string_append(&string,nombreCola);
	string_append(&string,": ");
	list_add_all(listaAux,cola->queue->elements);
	if(listaAux->elements_count==0)
		string_append(&string,"-");
	else{
		t_link_element* elemento=listaAux->head;
		t_personaje* unPersonaje;
		while(elemento!=NULL){
			unPersonaje=(t_personaje*)elemento->data;
			if(!string_ends_with(string,": "))
				string_append(&string," <- ");
			string_append(&string,unPersonaje->nombre);
			elemento=elemento->next;
		}
	}
	list_destroy(listaAux);
	return string;
}


void loguearEstado_Planificador(t_planificador* planificador){
	// Ver la sincronizacion de las colas, se pueden modificar al mismo tiempo, agregar algun mutex o los mismos de las colas
	char* mensaje=(char*)malloc(strlen("Planificador")+1);
	strcpy(mensaje,"Planificador");
	string_append(&mensaje,planificador->nombreNivelAsociado);
	string_append(&mensaje," = ");
	char* listos = stringColaPersonajes(planificador->colaListos,"Listos");
	char* bloqueados = stringColaPersonajes(planificador->colaBloqueados,"Bloqueados");
	string_append(&mensaje,listos);
	string_append(&mensaje," ;");
	string_append(&mensaje,bloqueados);
	string_append(&mensaje," ;");
	string_append(&mensaje,"Ejecutando: ");
	if(planificador->personajeActual==NULL)
		string_append(&mensaje,"-");
	else
		string_append(&mensaje,planificador->personajeActual->nombre);
	log_debug(planificador->logger,mensaje);
	free(mensaje);
	free(listos);
	free(bloqueados);
}

void cargarConfiguracionPlanificador(t_planificador* planificador,t_paramHiloPlanif* parametros){

	planificador->nombreNivelAsociado=parametros->nombreNivelAsociado;
	planificador->puerto= parametros->puertoPorDondeEscucha;
	planificador->quantum=parametros->quantum;
	planificador->lockDeQuantum=parametros->lockDeQuantum;
	planificador->retardo=parametros->retardo;
	char* nombre= string_new();


	string_append_with_format(&nombre,"plan_%s.log",parametros->nombreNivelAsociado);
	planificador->logger=log_create(nombre,parametros->nombreNivelAsociado,true,LOG_LEVEL_DEBUG);
	planificador->colaListos=parametros->colaListos;
	planificador->colaBloqueados=parametros->colaBloqueados;
	planificador->personajes=parametros->personajes;
	planificador->personajeActual= NULL;
	planificador->fdPipe=parametros->fdPipe;
	free(nombre);

	//free(parametros);
}
/*
char* loguearInterbloqueo(t_nivel* nivel,t_list* personajes){
	char* mensaje=(char*)malloc(strlen("Culpables de Interbloqueo: ")+1);
	char* culpables=string_new();
	strcpy(mensaje,"Culpables de Interbloqueo: ");
	t_link_element* elemento=personajes->head;
	t_personajeDeNivel* unPersonaje;
	while(elemento!=NULL){
		unPersonaje=(t_personajeDeNivel*)elemento->data;
		if(!string_ends_with(mensaje,": ")){
			string_append(&culpables,", ");
			string_append(&mensaje,", ");
		}
		string_append(&culpables,unPersonaje->nombre);
		string_append(&mensaje,unPersonaje->nombre);
		elemento=elemento->next;
	}
	log_debug(nivel->logger,mensaje);
	free(mensaje);
	return culpables;

}
*/
