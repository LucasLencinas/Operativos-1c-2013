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


#include <sys/types.h>
#include <sys/inotify.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/string.h>

#include "socket_y_serializadores/basicosSockets.h"
#include "socket_y_serializadores/nipc.h"

#include "planificador.h"
#include "orquestador.h"

#define BUFF_SIZE 1024
#define MAXEVENTS 64
#define BUFF_SIZE 1024

void siEstaSacarloDeLasListas(char* nombrePersonaje,t_planificador* planificador);

void funcionPlanificador(void* parametro){
	t_paramHiloPlanif* parametroHilo=(t_paramHiloPlanif*)parametro;

	t_planificador* planificador=(t_planificador*)malloc(sizeof(t_planificador));
	t_personaje* personaje;
	cargarConfiguracionPlanificador(planificador,parametroHilo);
	//Tendria que hacer el free del parametroHilo pero tiene el LockQuantum, tendria que ser un puntero eso? porque si hago el free se me pierde creo
	//int optval = 1;

	/////////////////////
	int socketEscucha, socketNuevaConexion, miEpoll, s;
	int bytesRecibidos;
	int yes=1;
	struct epoll_event event;
	struct epoll_event *events;
	//char buffer[BUFF_SIZE];
	int quantumRestante;
	socketEscucha = crear_socket();
	if (setsockopt(socketEscucha, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	bindear_socket(socketEscucha,planificador->puerto);
	escuchar_llamadas(socketEscucha);

	//Creo el epoll, y le agrego el socket que escucha
	miEpoll = epoll_create1(0);
	if (miEpoll == -1) {
		perror("epoll_create");
		abort();
	}

	event.data.fd = socketEscucha;
	event.events = EPOLLIN ;
	s = epoll_ctl(miEpoll, EPOLL_CTL_ADD, socketEscucha, &event);
	if (s == -1) {
		perror("epoll_ctl");
		abort();
	}

	event.data.fd = planificador->fdPipe;
	event.events = EPOLLIN ;
	s = epoll_ctl(miEpoll, EPOLL_CTL_ADD, planificador->fdPipe, &event);
	if (s == -1) {
		perror("epoll_ctl");
		abort();
	}


	events = calloc(MAXEVENTS, sizeof event);
	puts("Escuchando conexiones\n");

	while (1) {
		int n, done, i;

		n = epoll_wait(miEpoll, events, MAXEVENTS, -1);

		for (i = 0; i < n; i++) {

			if (planificador->fdPipe == events[i].data.fd) {
				char buffer[1024];
				int length = read(planificador->fdPipe, buffer, 1024); //Se desbloqueo un personaje
				usleep(100);
				if (length < 0)
					perror("read");

				intentarMoverAlSiguiente(planificador,&quantumRestante);
				continue;
			}


			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
				//El personaje termino el nivel o lo mataron por sucio
				
				log_debug(planificador->logger, "EPOLL ERROR PLANIFICADOR!!!!!!!!!!!!");
				sleep(1);
				imprimirPersonajes(planificador->logger,planificador->personajes);
				borrarPersonajeDePlanificador(planificador,events[i].data.fd);
				log_debug(planificador->logger,"despues de borrar persoanje");
				//sacarPersonajeSegunSocket(planificador->personajes, events[i].data.fd);
				log_debug(planificador->logger,"despues de borrar personaje de la lista general");

				intentarMoverAlSiguiente(planificador,&quantumRestante);
				log_debug(planificador->logger,"despues de intentarMoverAlsiguiente");
				continue;
			}

			//Este es el if que representa que tenemos una nueva conexion, la cual aceptamos, y realizamos el primer contacto
			else if (socketEscucha == events[i].data.fd) {
				socketNuevaConexion = aceptar_clientes(socketEscucha);
				//if que representa que hubo un error en la nueva conexion
				if (socketNuevaConexion == -1) {
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
						break;
					else {
						perror("accept");
						break;
					}
				}//FIN - if socketNuevaConexion == -1



				package *paquete = recibir_paquete(socketNuevaConexion);	      // En ningun lado veo que se haga un free de paquete, preguntar a nacho
				bytesRecibidos= paquete->payloadLength;
				if(bytesRecibidos > 0){


					switch(paquete->type){
					case presentacionPersonaje:
						
						imprimirPersonajes(planificador->logger,planificador->personajes);
						log_debug(planificador->logger,"Nueva conexion de Personaje Nombre: %s", paquete->payload);
						

//SACAR SI ESTA DE LAS LISTAS todavia no se si hace falta FIXME

						personaje=(t_personaje*)malloc(sizeof(t_personaje));
						personaje->nombre=malloc(strlen(paquete->payload)+1);
						strcpy(personaje->nombre,paquete->payload);
						personaje->socket=socketNuevaConexion;
						destruir_paquete(paquete);
						list_add(planificador->personajes,personaje);
						//FIXME, ese lista add, tiene que verificar que ya no este ese personaje.
						//si está => no hacer nada, sino, meterlo
						//O capaz que habria que hacerlo en la desconexion por fin de nivel

						cola_push(planificador->colaListos,personaje);
						intentarMoverAlSiguiente(planificador,&quantumRestante);

						break;

					default:
						printf("Handhsake valido para planificador.\n");
						break;
					}

					event.data.fd = socketNuevaConexion;
					event.events = EPOLLIN | EPOLLET;
					s = epoll_ctl(miEpoll, EPOLL_CTL_ADD, socketNuevaConexion, &event);
					if (s == -1) {
						perror("epoll_ctl para planificador");
						abort();
					}
				}
				if(bytesRecibidos == 0 )
					printf("BytesRecibidos == 0 Planificador primera conexion\n ");
				if(bytesRecibidos == -1 )
					printf("BytesRecibidos == -1 Planificador\n ");

				continue;
			}

			else {

				done = 0;

				package* paquete = recibir_paquete(events[i].data.fd);

				bytesRecibidos= paquete->payloadLength;
				if(bytesRecibidos>0){
					switch(paquete->type){

					case handshake:   //pongo handshake por paja,
						//es el case de msj cuando se le tira
						//sigterm al personaje y esta bloqueado
						borrarPersonajeDePlanificador(planificador,events[i].data.fd);
						sacarPersonajeSegunSocket(planificador->personajes, events[i].data.fd);

						close(events[i].data.fd);
						destruir_paquete(paquete);

						//intentarMoverAlSiguiente(planificador,&quantumRestante);


						break;


					case movimiento_simple:
						destruir_paquete(paquete);

						if(quantumRestante > 0){		//Sigo ejecutando el mismo
							--quantumRestante;
							usleep(planificador->retardo );
							movetePibe(planificador->personajeActual->socket);
						}

						else{
							cola_push(planificador->colaListos,planificador->personajeActual);
							planificador->personajeActual=NULL;
							intentarMoverAlSiguiente(planificador,&quantumRestante);

						}

						break;
					case movimiento_bloqueadoPorRecurso:

						planificador->personajeActual->solicitud=*paquete->payload;
						log_debug(planificador->logger,"%s se bloqueo por 1%c",planificador->personajeActual->nombre,
								planificador->personajeActual->solicitud);
						destruir_paquete(paquete);
						cola_push(planificador->colaBloqueados,planificador->personajeActual);
						planificador->personajeActual=NULL;
						intentarMoverAlSiguiente(planificador,&quantumRestante);

						break;
					case movimiento_objetivoEncontrado:
						if(!string_equals_ignore_case("finNivel",paquete->payload)){ // si no es su ultimo objetivo lo meto en la cola de listos
							cola_push(planificador->colaListos,planificador->personajeActual);
						}else{
							destruir_paquete(paquete);
							paquete = crear_paquete(movimientoPermitido, "Listo", strlen("Listo")+1);
							enviar_paquete(paquete, events[i].data.fd);
							log_debug(planificador->logger,"Desconexion de personaje %s", planificador->personajeActual->nombre);  //mentirosillo

						}
						destruir_paquete(paquete);

						planificador->personajeActual=NULL;
						intentarMoverAlSiguiente(planificador,&quantumRestante);

						break;
					default:
						close(events[i].data.fd);
						printf("Me mando fruta. Porque!!!!!!\n");
						break;
					}
				}

				if (bytesRecibidos==-1) {
					printf("BytesRecibidos == -1 Planificador\n ");
					if (errno != EAGAIN) {
						perror("read");
						done = 1;
					}
					continue;

				} else if (bytesRecibidos==0) {	//El personaje termino el nivel o lo mataron por sucio
					imprimirPersonajes(planificador->logger,planificador->personajes);
					printf("BytesRecibidos == 0 Planificador\n ");
					borrarPersonajeDePlanificador(planificador,events[i].data.fd);
					sacarPersonajeSegunSocket(planificador->personajes, events[i].data.fd);

					intentarMoverAlSiguiente(planificador,&quantumRestante);
					
					done = 1;
					continue;
				}
			}
			if (done){
				printf ("Closed connection on descriptor %d\n",events[i].data.fd);
				// Closing the descriptor will make epoll remove it from the set of descriptors which are monitored.
				close (events[i].data.fd);
			}
		}
	}
	free (events);
	close (socketEscucha);
}



void siEstaSacarloDeLasListas(char* nombrePersonaje,t_planificador* planificaodr){



}
