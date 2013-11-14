/*
 * orquestador.h
 *
 *  Created on: 17/05/2013
 *      Author: utnso
 */

#ifndef ORQUESTADOR_H_
#define ORQUESTADOR_H_

#include <commons/log.h>
#include "colas.h"	//estructuras mias(Lucas) para tener colas con sincronizacion
#include <commons/collections/list.h>


typedef struct{
	char* nomNivel;
	char* ipPuertoNivel;

}t_datosConexionNivel;

typedef struct{
	char* nombre;
	int socket;
	bool listo;
	char solicitud;
}t_personaje;

typedef struct{
	char* nombreNivelAsociado;
	t_cola* colaBloqueados;
	t_cola* colaListos;
	t_list* personajes;
	int *quantum;
	int retardo;
	int puertoPorDondeEscucha;
	pthread_rwlock_t lockDeQuantum;
	int fdPipe;
}t_paramHiloPlanif;

typedef struct{
	int *quantum;
	pthread_rwlock_t lockDeQuantum;
	int retardo;
	int puertoBase;
	int miPuerto;
	t_list* personajes;
	t_list* planificadores;
	t_list* niveles;
	t_log* logger;
}t_orquestador;

typedef struct{
	char* nombre;
	t_cola* colaBloqueados;
	t_list* personajes;
	char* puertoPlanif;
	t_cola* colaListos;
	int fdPipe;
}t_planificadorParaLista;

typedef struct{
	char* configPath;
	int* quantum;
	pthread_rwlock_t lockDeQuantum;
	t_log* logger;
}t_parametroInotify;

typedef struct {
	int recx;
	int recy;
	char identificador;
	int instancias;
}t_recurso;


t_datosConexionNivel * encontrarNivel(t_list* lista, char* nombre);
t_planificadorParaLista* encontrarPlanificador(t_list* lista, char* nombre);

void *funcionOrquestador();
void cargarConfiguracionOrquestador(t_orquestador* orquestador,char* configPath);
void levantarPlanificador(t_orquestador* orquestador,char* nombreNivel);
void funcionalidad_inotify(void* algoMagico);
t_personaje* encontrarPersonajeBloqueado(t_cola* bloqueados, char* nombre);
t_personaje* sacarPrimerPersonajeDelosQueSeBloquearon(t_planificadorParaLista* planificador);





#endif /* ORQUESTADOR_H_ */
