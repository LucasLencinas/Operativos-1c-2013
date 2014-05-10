/*
 * planificador.h
 *
 *  Created on: 06/05/2013
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <commons/log.h>
#include <commons/collections/queue.h>
#include "colas.h"
#include "orquestador.h"



    typedef struct {
    	char* nombreNivelAsociado;
    	t_log* logger;
    	int* quantum;
    	pthread_rwlock_t lockDeQuantum;
    	int retardo;
    	int puerto;
    	t_cola* colaBloqueados;
    	t_cola* colaListos;
    	t_list* personajes;
    	t_personaje* personajeActual;
    	int fdPipe;
    } t_planificador;




    void cargarConfiguracionPlanificador(t_planificador* planificador, t_paramHiloPlanif* parametros);
    void funcionPlanificador(void* parametrosPlanificador);
    void planificador_posta(void* algo);
    void loguearEstado_Planificador(t_planificador* planificador);
    char* stringColaPersonajes(t_cola* cola,char* nombreCola);

    void borrarPersonajeDePlanificador(t_planificador* planificador, int socket);
    void sacarPersonajeSegunSocket(t_list* personajes, int socket);

    void actualizarQuantum(t_planificador* planificador, int* quantumRestante);
    void intentarMoverAlSiguiente();
    void movetePibe();







#endif /* PLANIFICADOR_H_ */
