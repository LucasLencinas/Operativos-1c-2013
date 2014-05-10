#include <pthread.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>


#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/string.h>

#include "socket_y_serializadores/basicosSockets.h"
#include "socket_y_serializadores/nipc.h"

#include "planificador.h"
#include "orquestador.h"


void actualizarQuantum(t_planificador* planificador, int* quantumRestante){
	pthread_rwlock_rdlock(&planificador->lockDeQuantum);
	*quantumRestante=*planificador->quantum;
	pthread_rwlock_unlock(&planificador->lockDeQuantum);

}

void intentarMoverAlSiguiente(t_planificador* planificador,int* quantumRestante){
	if(planificador->personajeActual==NULL){
		pthread_mutex_lock(planificador->colaListos->mutexCola);
		if(sem_trywait(&planificador->colaListos->contador)!=-1){
			planificador->personajeActual=queue_pop(planificador->colaListos->queue);

			actualizarQuantum(planificador,quantumRestante);
			log_debug(planificador->logger,"Turno de %s, %d movimientos",
					planificador->personajeActual->nombre,*quantumRestante);
			--*quantumRestante;		//printf("quantum restante:%d\n",--quantumRestante);

			loguearEstado_Planificador(planificador);

			usleep(planificador->retardo );
			movetePibe(planificador->personajeActual->socket);
			//LOGUEAR tambien por el caso que no haya persoajes, porque tengo que marcar que se murio uno
		}
		else
			loguearEstado_Planificador(planificador);
		pthread_mutex_unlock(planificador->colaListos->mutexCola);
	}
}





 void movetePibe(int socketPersonaje){
	 package *paquete=crear_paquete(movimientoPermitido,"movete",strlen("movete")+1);
	 enviar_paquete(paquete,socketPersonaje);
	 destruir_paquete(paquete);

 }



