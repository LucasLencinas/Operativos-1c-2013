/*
 * orquestador.c
 *
 *  Created on: 17/05/2013
 *      Author: utnso
 */

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <stdint.h>

#include <commons/config.h>
#include <commons/string.h>

#include "socket_y_serializadores/nipc.h"
#include "socket_y_serializadores/basicosSockets.h"
#include "socket_y_serializadores/serializar.h"
#include "orquestador.h"

#define BUFF_SIZE 1024
#define MAXEVENTS 64
#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )

// El tamaño del buffer es igual a la cantidad maxima de eventos simultaneos
// que quiero manejar por el tamaño de cada uno de los eventos. En este caso
// Puedo manejar hasta 1024 eventos simultaneos.
#define BUF_LEN     ( 10 * EVENT_SIZE )

t_list* armarListaRecursos(char* recursos);
bool loPudeDesbloquear(t_personaje* personaje,t_list* recursos);
void imprimirPersonajes(t_log* logger,t_list* personajes);

void *funcionOrquestador(char* configPath) {


	int numeroPersonajes;
	printf ("Indique cuantos personajes ingresaran en el juego: ");
	scanf("%d", &numeroPersonajes);

	t_orquestador* orquestador = (t_orquestador*)malloc(sizeof(t_orquestador));
	cargarConfiguracionOrquestador(orquestador,configPath);

	printf("Datos de la Plataforma:\n");
	printf("Retardo= %d\n",orquestador->retardo);
	printf("Quantum=%d\n",*orquestador->quantum);
	printf("PuertoBase=%d\n",orquestador->puertoBase);

	int socketEscucha, socketNuevaConexion, miEpoll, s;
	int bytesRecibidos;
	int yes=1;
	struct epoll_event event;
	struct epoll_event *events;
	//char buffer[BUFF_SIZE];
	socketEscucha = crear_socket();
	if (setsockopt(socketEscucha, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	bindear_socket(socketEscucha,orquestador->miPuerto);
	escuchar_llamadas(socketEscucha);


	//Creo el epoll, y le agrego el socket que escucha
	miEpoll = epoll_create1(0);
	if (miEpoll == -1) {
		perror("epoll_create");
		abort();
	}

	event.data.fd = socketEscucha;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(miEpoll, EPOLL_CTL_ADD, socketEscucha, &event);
	if (s == -1) {
		perror("epoll_ctl");
		abort();
	}

	events = calloc(MAXEVENTS, sizeof event);
	puts("Escuchando conexiones\n");

	//----Comienzo cosas inotify-----

	int inotify = inotify_init();
	if (inotify < 0) {
		perror("inotify_init");
	}
	inotify_add_watch(inotify, configPath, IN_MODIFY);

	event.data.fd = inotify;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(miEpoll, EPOLL_CTL_ADD, inotify, &event);
	if (s == -1) {
		perror("epoll_ctl");
		abort();
	}
	//----Fin cosas inotify

	while (numeroPersonajes != 0) {
		int n, done, i;

		n = epoll_wait(miEpoll, events, MAXEVENTS, -1);

		for (i = 0; i < n; i++) {
			if (inotify == events[i].data.fd) { // si el evento que ocurre es de inotify lo atiendo
				char buffer[1024];
				int length = read(inotify, buffer, BUF_LEN);
				usleep(3000);// hago un retardo porque sino hay segmentation fault porque el read no es bloqueante CREO
				if (length < 0) {
					perror("read");
				}
				t_config* config_txt=config_create(configPath);
				int quantumNuevo = config_get_int_value(config_txt, "quantum");
				printf("Se obtiene el quantumNuevo\n");
				pthread_rwlock_wrlock(&orquestador->lockDeQuantum);
				*orquestador->quantum=quantumNuevo;
				log_debug(orquestador->logger,"valorActual de quantum:%d",quantumNuevo);
				pthread_rwlock_unlock(&orquestador->lockDeQuantum);
				continue;
			}
			//Este es el if que representa que hubo un error
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
					fprintf(stderr, "epoll error orquestador\n");
					log_debug(orquestador->logger,"Error en el socket: %d",socketNuevaConexion);
					close(events[i].data.fd);
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
			package *paquete_respuesta = NULL;
			t_presentacionPersonaje * personajePres;// = malloc(sizeof(t_presentacionPersonaje));  //QUEDA FEO ACA PERO NOSE PORQUE SINO NO ANDA
			t_presentacionNivel * presNivel;// = malloc(sizeof(t_presentacionNivel));




			////////////para logueo conexion personaje VER MAÑANA///////////
			struct sockaddr_in suAddr = tomar_datos_conexion (socketNuevaConexion);

			switch(paquete->type){
				case handshakePresentacionPersonaje:

					//logueo conexion
					log_debug(orquestador->logger,"Handshake de PERSONAJE IP:%s PUERTO:%d",  inet_ntoa(suAddr.sin_addr),ntohs(suAddr.sin_port));
					//fin logueo conexion

					personajePres = desserializarPresentPersonaje(paquete->payload);
					list_add(orquestador->personajes, personajePres);
					printf("El personaje %s quiere entrar al: %s\n",personajePres->nomPersonaje, personajePres->nomNivel);
					t_respuestaPresentacion * respPresent = malloc(sizeof(t_respuestaPresentacion));

					t_datosConexionNivel * unNivel = encontrarNivel(orquestador->niveles, personajePres->nomNivel);
					t_planificadorParaLista * unPlanif = encontrarPlanificador(orquestador->planificadores, personajePres->nomNivel);

					if (unNivel != NULL || unPlanif != NULL){

					respPresent->ipPuertoPlanif = unPlanif->puertoPlanif;  //(Es solo puerto)
					respPresent->ipPuertoNivel =  unNivel->ipPuertoNivel;			// Es IP:Puerto

					char *streamResp = serializarRespuestaPresent(respPresent);
					paquete_respuesta = crear_paquete(respuestaHandshakePersonaje, streamResp,
							strlen(respPresent->ipPuertoPlanif) + 1 + strlen(respPresent->ipPuertoNivel)+1);
					enviar_paquete(paquete_respuesta, socketNuevaConexion);
					destruir_paquete(paquete_respuesta);
					free(streamResp);
					free(respPresent);
					}else{
						printf("%s no encontrado.\n",personajePres->nomNivel);
						paquete_respuesta = crear_paquete(respuestaHandshakePersonajeSinNivel,
								"sinNivel", strlen("sinNivel")+1);
						enviar_paquete(paquete_respuesta, socketNuevaConexion);
						destruir_paquete(paquete_respuesta);
						close(socketNuevaConexion);
					}
					log_debug(orquestador->logger,"fin handshake personaje");

					break;

				case handshakeNivel:

					presNivel = desserializarPresNivel(paquete->payload);

					//logueo conexion/////////////////
					char** direccionParaLog = string_split(presNivel->ipPuertoNivel,":");
					log_debug(orquestador->logger,"Handshake NIVEL IP:%s PUERTO:%s",  direccionParaLog[0],direccionParaLog[1]);
					free(direccionParaLog[0]);
					free(direccionParaLog[1]);
					//fin logueo conexion////////////

					list_add(orquestador->niveles, presNivel);
					log_debug(orquestador->logger,"levanto el planificador %s",presNivel->nomNivel);
					levantarPlanificador(orquestador,presNivel->nomNivel);
					paquete_respuesta = crear_paquete(respuestaHandshakeNivel, "holanivel!", strlen("holanivel!") + 1);
					enviar_paquete(paquete_respuesta, socketNuevaConexion);
					destruir_paquete(paquete_respuesta);
					//free(presNivel->ipPuertoNivel);
					//free(presNivel->nomNivel);
					//free(presNivel);
					log_debug(orquestador->logger,"fin Handshake nivel");

					break;

				case fin_personaje:
					log_debug(orquestador->logger,"Quedan %d Personajes",--numeroPersonajes);

					break;

				default:
					close(socketNuevaConexion);
					printf("Handshake invalido para orquestador, se cierra conexion\n");
					break;
				}
			destruir_paquete(paquete);

			//Metemos al recien recibido en el epoll
			event.data.fd = socketNuevaConexion;
			event.events = EPOLLIN | EPOLLET;
			s = epoll_ctl(miEpoll, EPOLL_CTL_ADD, socketNuevaConexion, &event);
			if (s == -1) {
				perror("epoll_ctl para orquestador");
				abort();
			}
			continue;
			}

			//Aca esta como nos comunicamos con los sockets que ya estan
			//dentro del epoll (osea que se fueron conectando, e hicieron handshake valido)
			else {

				done = 0;



					package* paquete = recibir_paquete(events[i].data.fd);
					t_nivelConRecursos* nivelConRecursos;
					t_nivelConCulpables* nivelConCulpables;
					t_planificadorParaLista* planificador;
					bytesRecibidos= paquete->payloadLength;
					if(bytesRecibidos > 0 ){
						switch(paquete->type){
						case recursosLiberados:
							log_debug(orquestador->logger,"comienzo recursos liberados");

							nivelConRecursos = desserializarNivelConRecursos(paquete->payload);
							destruir_paquete(paquete);
							log_debug(orquestador->logger,"Me llega: %s y %s",nivelConRecursos->nomNivel,nivelConRecursos->recursos);
							planificador = encontrarPlanificador(orquestador->planificadores,nivelConRecursos->nomNivel);
							pthread_mutex_lock(planificador->colaBloqueados->mutexCola);
							t_list* lista= planificador->colaBloqueados->queue->elements;
							t_link_element* elementoLista = lista->head;	//Primer elemento de la lista
							t_personaje* personaje;
							int index=0;

							t_list* recursos= armarListaRecursos(nivelConRecursos->recursos);
							//mutex lock creo de la lista
							log_debug(orquestador->logger,"perosnajes en la coal de bloqueados: %d",lista->elements_count);

							while(elementoLista!=NULL){

								personaje= (t_personaje*)elementoLista->data;
								if(loPudeDesbloquear(personaje,recursos)){//si lo desbloquea, le asigno el recurso y lo resto en la lista
									//Saca el personaje de la bloqueados y los pasa a listos

									sem_wait(&planificador->colaBloqueados->contador);
									cola_push(planificador->colaListos,personaje);
									//Avisarle al planificador
									elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
									list_remove(lista,index); // lo hago aca porque me puede traer problemas con el putnero elementoLIsta
									//index++;

									if(write (planificador->fdPipe, "personaje desbloqueado",strlen("personaje desbloqueado")+1) == -1){
										perror ("write");
										exit (2);
									}
								}
								else{
									elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
									index++;
								}
							}
							pthread_mutex_unlock(planificador->colaBloqueados->mutexCola);

							char* recRestantes=string_new();	//No uso mi otra funcion porque la otra no libera los recursos
							while(recursos->elements_count != 0){
								t_recurso * unRecurso = list_remove(recursos,0);
								string_append_with_format(&recRestantes,"%d%c",unRecurso->instancias,unRecurso->identificador);
								free(unRecurso);
								if(recursos->elements_count != 0)
									string_append(&recRestantes,"|");
								//free (unRecurso);
							}

							paquete = crear_paquete(recursosRestantes,  recRestantes, strlen(recRestantes)+1);
							enviar_paquete(paquete, events[i].data.fd);
							destruir_paquete(paquete);

							free(nivelConRecursos->nomNivel);
							free(nivelConRecursos->recursos);
							free(nivelConRecursos);
							free(recRestantes);
							list_destroy_and_destroy_elements(recursos,free);
							log_debug(orquestador->logger,"fin recursos liberados");

							break;

						case notificacion_interbloqueo: // ME llega, culpables deadlock: alguien, alguien, alguien
							log_debug(orquestador->logger,"alguien me avisa por interbloqueo");

							nivelConCulpables = desserializarNivelConCulpables(paquete->payload);
							destruir_paquete(paquete);
							log_debug(orquestador->logger,"Mensaje de deadlock: %s y %s",nivelConCulpables->nomNivel,nivelConCulpables->personajes);
							planificador = encontrarPlanificador(orquestador->planificadores,nivelConCulpables->nomNivel);

							log_debug(orquestador->logger,"personajes del planificador");
							imprimirPersonajes(orquestador->logger,planificador->personajes);
							//log_debug(orquestador->logger,"personajes del orquestador");
							//imprimirPersonajes(orquestador->logger,orquestador->personajes);
							personaje= sacarPrimerPersonajeDelosQueSeBloquearon(planificador);

							paquete = crear_paquete(personajeEliminado,  personaje->nombre, strlen(personaje->nombre)+1);
							enviar_paquete(paquete, events[i].data.fd);
							destruir_paquete(paquete);
							//sleep(1);
							close(personaje->socket);
							free(nivelConCulpables->nomNivel);
							free(nivelConCulpables->personajes);
							free(nivelConCulpables);
							/*
							 *
							 *El personaje se desconecta del nivel, y recien ahi
							 *nivel me dice cuantos recursos se liberaron
							 *le contesto con los sobrantes
							 *y listo
							 */
							log_debug(orquestador->logger,"fin aviso de interbloqueo");


							break;

						default:
							close(events[i].data.fd);
							printf("Handshake invalido, se cierra conexion\n");
							break;
						}
					}

					if (bytesRecibidos==-1) {
						printf("BytesRecibidos == -1 Orquestador\n ");
						if (errno != EAGAIN) {
							perror("read");
							done = 1;
						}
						continue;

					} else if (bytesRecibidos==0) {
						printf("BytesRecibidos == 0 Orquestador\n ");

						/* End of file. The remote has closed the connection. */
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

	free(orquestador->quantum);

	void nivel_destroy(t_datosConexionNivel *self){
		free(self->ipPuertoNivel);
		free(self->nomNivel);	// FIXME error en el free
		free(self);
	}
	void planificador_destroy(t_planificadorParaLista *self){
		free(self->nombre);	// FIXME error en el free
		free(self->puertoPlanif);


		free(self);
	}
	void personaje_destroy(t_presentacionPersonaje *self){
		free(self->nomNivel);
		free(self->nomPersonaje);
		free(self);
	}


	log_destroy(orquestador->logger);
	//list_destroy_and_destroy_elements(orquestador->personajes,(void*)personaje_destroy);
	//list_destroy_and_destroy_elements(orquestador->niveles,(void*)nivel_destroy);
	//list_destroy_and_destroy_elements(orquestador->planificadores,(void*)planificador_destroy);

	free(orquestador);

	//free (events);
    close (socketEscucha);
    return NULL;
	}

/////////////////////FIN ORQUESTADOR/////////////////


t_datosConexionNivel* encontrarNivel(t_list* lista, char* nombre){

t_link_element* elementoLista = lista->head;	//Primer elemento de la lista
t_datosConexionNivel* nivel=NULL;

while(elementoLista!=NULL){ //mientras que sea el fin de la lista
	nivel=(t_datosConexionNivel*)elementoLista->data; //data contiene el nivel propiamente dicho
	if((strcmp(nivel->nomNivel,nombre))==0){ //se fija si el nombre del nivel es el mismo que elnombre pasado por parametro
		return nivel;		//termina el while
	}
	elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
}
return NULL;

}

t_planificadorParaLista* encontrarPlanificador(t_list* lista, char* nombre){

t_link_element* elementoLista = lista->head;	//Primer elemento de la lista
t_planificadorParaLista* planificador=NULL;

while(elementoLista!=NULL){
	planificador=(t_planificadorParaLista*)elementoLista->data; //data contiene el planificador propiamente dicho
	if((strcmp(planificador->nombre,nombre))==0){ //se fija si el nombre del planificador es el mismo que elnombre pasado por parametro
		return planificador;		//termina el while
	}
	elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
}
return NULL;

}

t_recurso* encontrarRecurso(t_list* lista, char solicitud,int* index){

t_link_element* elementoLista = lista->head;	//Primer elemento de la lista
t_recurso* recurso=NULL;
*index=0;

while(elementoLista!=NULL){
	recurso=(t_recurso*)elementoLista->data;
	if(recurso->identificador == solicitud){

		return recurso;		//termina el while
	}
	*index= *index + 1;
	elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
}
return NULL;
}

bool loPudeDesbloquear(t_personaje* personaje,t_list* recursos){
	int index;
	t_recurso* recurso= encontrarRecurso(recursos,personaje->solicitud,&index);
	if(recurso==NULL)
		return false;

	recurso->instancias--;
	if(recurso->instancias==0){
		recurso=list_remove(recursos,index);
		free(recurso);
	}
	return true;
}


t_list* armarListaRecursos(char* recursos){

	t_list* listaRecursos=list_create();
	t_recurso* recurso;

	int i;
	for(i=0;recursos[i]!='\0';i++){
		recurso=malloc(sizeof(t_recurso));
		recurso->instancias=atoi(&recursos[i]);
		i++;
		recurso->identificador=recursos[i];
		i++;
		if(recursos[i]=='\0')
			i--;
		list_add(listaRecursos,recurso);
	}
	return listaRecursos;

}

void imprimirPersonajes(t_log* logger,t_list* personajes){
	char* chabones =string_new();
	string_append(&chabones,"personajes: ");
	if(personajes->elements_count==0)
		string_append(&chabones," ---");
	else{
		void _imprimirUnPersonaje(t_personaje *personaje) {
			string_append_with_format(&chabones,"%s ",personaje->nombre);
		}
		list_iterate(personajes, (void*) _imprimirUnPersonaje);
	}
	log_debug(logger,chabones);
	free(chabones);
}




