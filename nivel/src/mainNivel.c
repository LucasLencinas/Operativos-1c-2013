#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "tad_items.h"
#include "estructurasNivel.h"
#include "lectura_config.h"
#include "funcionesNivel.h"
#include "serializar.h"
#include "nipc.h"
//#include "commons/collections/list.h"//LIBNIVEL


#define DIRECCION INADDR_ANY   //INADDR_ANY representa la direccion de cualquier
							   //interfaz conectada con la computadora

#define BUFF_SIZE 1024
#define MAXEVENTS 64

int realizarHandshakeOrquestador(t_nivel* nivel, int socketOrquestador,char* direccionNivel);
char* streamRecursosAgarrados(t_list* recursos);
char* loguearInterbloqueo(t_nivel* nivel,t_list* personajes);
void* deadlock(void* nivelFeo);



int socketOrquestador;
t_log* loggerGlobal;
pthread_mutex_t* mutexDeadlock;
bool seTerminoLaActualizacionDeRecursos;
int actualizados;

int main(int argc, char **argv){
	int32_t socketEscucha, socketNuevaConexion;
	int efd, s;
	package *paquete = NULL;
	int bytesRecibidos;
	struct epoll_event event;
	struct epoll_event *events;

	char* configPath=argv[1];
//	int socketOrquestador;

	mutexDeadlock=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutexDeadlock,NULL);


	int indice;
    t_link_element *elementoPersonaje;
    unPersonaje* personaje;
    t_nivel *nivel=(t_nivel*)malloc(sizeof(t_nivel));
    t_presPersonaje* presentacion;
    //t_movimientoPermitido * mov=(t_movimientoPermitido*)malloc(sizeof(t_movimientoPermitido));
    t_asignacionRecurso * rec=(t_asignacionRecurso*)malloc(sizeof(t_asignacionRecurso));
    //char caracter;



	cargarConfiguracionNivel(nivel,configPath);
	printf("nombre=%s\n",nivel->nombre);
	printf("recovery=%d\n",nivel->recovery);
	printf("TiempoChequeoDeadlock=%d\n",nivel->t_chequeoDL);
	printf("orquestador=%s\n",nivel->dir_orquestador);
	printf("miDireccion:%s\n",nivel->miDireccion);
	imprimirCajas(nivel->logger,nivel->cajas);
	loggerGlobal= nivel->logger;



	printf("Conectandome al orquestador\n");
	char** direccionOrquestador= string_split(nivel->dir_orquestador,":");
	socketOrquestador = sockets_abrir_cliente(direccionOrquestador[0],atoi(direccionOrquestador[1]));

	if(realizarHandshakeOrquestador(nivel,socketOrquestador,nivel->miDireccion)==0){
		printf("Error en Handshake\n");
		return EXIT_FAILURE;
	}
	free(direccionOrquestador[0]);
	free(direccionOrquestador[1]);

	//t_list* lista_personajes = list_create();
	char** direccion=string_split(nivel->miDireccion,":");
	socketEscucha = sockets_abrir_servidor(DIRECCION, atoi(direccion[1]), 10);
	free(direccion[0]);
	free(direccion[1]);

	//Dibuja el nivel en pantalla
	ITEM_NIVEL* ListaItems = NULL;
	int rows, cols;
	//int posx, posy;
	nivel_gui_inicializar();	//SALTAR ESTA LINEA
	nivel_gui_get_area_nivel(&rows, &cols);

	cargarCajas(nivel->cajas, &ListaItems,cols,rows);
	nivel_gui_dibujar(ListaItems);
	//Fin dibujado del nivel en pantalla

    pthread_t th_deadlock;
    pthread_create(&th_deadlock, NULL, deadlock, (void*)nivel);

	efd = epoll_create1(0);
	if (efd == -1) {
		perror("epoll_create");
		abort();
	}

	event.data.fd = socketEscucha;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(efd, EPOLL_CTL_ADD, socketEscucha, &event);
	if (s == -1) {
		perror("epoll_ctl");
		abort();
	}
	events = calloc(MAXEVENTS, sizeof event);

	while (1) {
		int n, done, i;
		n = epoll_wait(efd, events, MAXEVENTS, -1);

                         pthread_mutex_lock(mutexDeadlock); //Lo agregue  Mariano
		for (i = 0; i < n; i++) {
			//pthread_mutex_lock(mutexDeadlock);
			    log_debug(nivel->logger,"mutex Lock desde epoll");//--------------------------
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
				// error en ese file descriptor o el no esta listo para lectura
				log_debug(loggerGlobal,"epoll error");

				fprintf(stderr, "epoll error\n");
				close(events[i].data.fd);
				continue;
			}

			else if (socketEscucha == events[i].data.fd) {
				if ((socketNuevaConexion = accept(socketEscucha, NULL, 0)) < 0) {
					perror("Error al aceptar conexion entrante");
					return EXIT_FAILURE;
				}

				if (socketNuevaConexion == -1) {
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						/* We have processed all incoming connections. */
						break;
					} else {
						perror("accept");
						break;
					}
				}
// aca es cuando se conecta un personaje por primera vez

				// algo

// fin de presentacion personaje
				event.data.fd = socketNuevaConexion;
				event.events = EPOLLIN | EPOLLET;
				s = epoll_ctl(efd, EPOLL_CTL_ADD, socketNuevaConexion, &event);
				if (s == -1) {
					perror("epoll_ctl");
					abort();
				}
				continue;
			}
			else {

				done = 0;

				paquete = recibir_paquete(events[i].data.fd);
				bytesRecibidos = paquete->payloadLength;
				if (bytesRecibidos > 0) {
					char recurso;
					unPersonaje* personajin;
					switch(paquete->type){

					case presentacionPersonaje:
						log_debug(nivel->logger,"alguien se presenta");
						presentacion= desserializarPresPersonaje(paquete->payload);
						personaje = nuevoPersonaje(events[i].data.fd, presentacion->simbolo,presentacion->nombre , 1 , 1);
						log_debug(nivel->logger,"Se presenta %s con %c",presentacion->nombre,presentacion->simbolo);
						list_add(nivel->personajes, personaje);
						CrearPersonaje(&ListaItems,personaje->identificador, personaje->pos_x, personaje->pos_y);
						free(presentacion->nombre);
						free(presentacion);
						log_debug(nivel->logger,"a punto de dibujar en PresentacionPersonaje");
						nivel_gui_dibujar(ListaItems);
						log_debug(nivel->logger,"Se dibujo");


					    paquete = crear_paquete(handshake, "VIVO", strlen("VIVO")+1);
					    enviar_paquete(paquete, events[i].data.fd);
					    destruir_paquete(paquete);
					    log_debug(nivel->logger,"Mande vivo a Personaje");

						break;

					case movimientoPersonaje:
						log_debug(nivel->logger,"alguien de mueve");
						personaje = buscaPersonaje(nivel->personajes,events[i].data.fd);
						if(personaje->solicitud !='\0'){
							//If(el movimiento anterior fue agarrar una instancia de un recurso)....
							//Entonces....Hago lo que sigue

							t_recurso *recurso_objetivos;
							t_recurso * unRecurso = buscarRecurso(ListaItems,personaje->pendiente);
							recurso_objetivos = buscaRecurso(personaje->objetivosCumplidods, personaje->pendiente);

							if (recurso_objetivos!= NULL){
								recurso_objetivos->instancias = recurso_objetivos->instancias + 1;
							 }
							else{
								unRecurso->instancias=1;
								list_add(personaje->objetivosCumplidods, unRecurso);
							}
							personaje->pendiente='\0';
							personaje->solicitud='\0';
							//No pide ni pidio nada.

						}
						if (strcmp(paquete->payload, "AR") == 0){
							if (personaje->pos_y > 1) {
								personaje->pos_y = personaje->pos_y - 1;
							}
						}
						else if (strcmp(paquete->payload ,"AB") == 0){
							if (personaje->pos_y< rows) {
								personaje->pos_y = personaje->pos_y + 1;
							}
						}
						else if (strcmp(paquete->payload ,"DE") == 0){
							if (personaje->pos_x < cols) {
								personaje->pos_x = personaje->pos_x + 1;
							}
						}
						else if (strcmp(paquete->payload ,"IZ") == 0){
							if (personaje->pos_x > 1) {
								personaje->pos_x = personaje->pos_x - 1;
							}
						}
						else if (strcmp(paquete->payload ,"FIN") == 0){
							nivel_gui_terminar();
						}

						//  personaje->pos_x = posx;
						//	personaje->pos_y = posy;
						MoverPersonaje(ListaItems,personaje->identificador ,personaje->pos_x, personaje->pos_y);
						destruir_paquete(paquete);

						paquete = crear_paquete(handshake, "VIVO", strlen("VIVO")+1);
						enviar_paquete(paquete, events[i].data.fd);
						destruir_paquete(paquete);
						log_debug(nivel->logger,"Envio confirmacion de desplazamiento");


						log_debug(nivel->logger,"a punto de dibujar en MovimientoPersonaje");
						nivel_gui_dibujar(ListaItems);
						log_debug(nivel->logger,"Se dibujo");
						break;

					case posicionRecurso:
						log_debug(nivel->logger,"alguien pide PosicionRecurso");
						personajin = (unPersonaje*) buscaPersonaje(nivel->personajes,events[i].data.fd);
						log_debug(nivel->logger, "PETICION DE POSICION DE RECURSO");
						log_debug(nivel->logger,"HECHA POR %s", personajin->nombre);
						recurso = (char )*paquete->payload;
						destruir_paquete(paquete);
						t_recurso * posiciones = buscarRecurso(ListaItems,recurso);
						t_stream* stream = serializar_posiciones(posiciones);

						log_debug(nivel->logger,"Le envio %c=[ %d , %d ] a %s",recurso,posiciones->recx,
								posiciones->recy,personajin->nombre);
						paquete = crear_paquete(handshake, stream->payload, stream->length);
						enviar_paquete(paquete, events[i].data.fd);
						destruir_paquete(paquete);
						free(posiciones);
						free(stream->payload);
						free (stream);
						log_debug(nivel->logger,"Se envio bien");

						break;

					case instanciaRecurso:
						log_debug(nivel->logger,"alguien pide InstanciaRecurso");
						personaje = buscaPersonaje(nivel->personajes,events[i].data.fd);
						recurso = (char )*paquete->payload;
						destruir_paquete(paquete);

						t_recurso * unRecurso = buscarRecurso(ListaItems,recurso);
						if ((personaje->pos_y == unRecurso->recy) && (personaje->pos_x == unRecurso->recx)) {
							char *b = malloc(2);
							sprintf(b,"%c",recurso);
							rec->IDunRecurso = b;
							t_recurso *recurso_objetivos;

							if(unRecurso->instancias > 0){

								restarRecurso(ListaItems,nivel->cajas, unRecurso->identificador);
								rec->resultadoOperacion = 1;
								recurso_objetivos = buscaRecurso(personaje->objetivosCumplidods, recurso);

								if (recurso_objetivos!= NULL){//Si el recurso esta en sus cumplidos => incremento instancias
									free(unRecurso);//FIXME, es muy dudoso
									recurso_objetivos->instancias = recurso_objetivos->instancias + 1;
								}
								else{//Agrego un recurso nuevo a sus cumplidos
									unRecurso->instancias=1;
									list_add(personaje->objetivosCumplidods, unRecurso);
								     }
							}
							else {
								//ESta bloqueado
								//Cuando vuelvue a hacer un movimiento o me lo pide explicitamente("Actualizame este recurso")
								//Se lo doy
								personaje->solicitud=recurso;
								rec->resultadoOperacion = 0;
								personaje->pendiente=recurso;
							}

							char *streamRec = serializarAsignacionRecurso(rec);
							paquete = crear_paquete(handshake,  streamRec, strlen(streamRec)+1);
							enviar_paquete(paquete, events[i].data.fd);
							destruir_paquete(paquete);
							free(streamRec);
							free(b);
							//CREO!!!!! que
							//TENDRIA QUE HABER UN free(rec y sus cosas) y sacar ese free(b)
						}

						log_debug(nivel->logger,"a punto de dibujar en InstanciaRecurso");
						nivel_gui_dibujar(ListaItems);
						log_debug(nivel->logger,"Se dibujo");

						break;

					case handshake:	//SIRVE PARA ACTUALIZAR RECURSOS nomas
						log_debug(nivel->logger,"Handshake de alguien");
						elementoPersonaje = buscaPersona(nivel->personajes,&indice,events[i].data.fd);
						destruir_paquete(paquete);

						personaje = elementoPersonaje->data;
						log_debug(nivel->logger,"Actualizacion de Recursos de: %s",personaje->nombre);
						t_recurso *recurso_objetivos;
						recurso_objetivos = buscaRecurso(personaje->objetivosCumplidods, personaje->pendiente);
						log_debug(loggerGlobal,"%s tiene los siguientes objetivosCumplidos=",personaje->nombre);
						imprimirObjetivosCumplidos(loggerGlobal,personaje->objetivosCumplidods);
						log_debug(loggerGlobal,"y pendiente 1%c",personaje->pendiente);

						if (recurso_objetivos!= NULL){
							recurso_objetivos->instancias = recurso_objetivos->instancias + 1;
						}
						else{
							unRecurso = buscarRecurso(ListaItems,personaje->pendiente);
							unRecurso->instancias=1;
							list_add(personaje->objetivosCumplidods, unRecurso);

						}
						log_debug(loggerGlobal,"Ahora, %s tiene los siguientes objetivosCumplidos=",personaje->nombre);

						imprimirObjetivosCumplidos(loggerGlobal,personaje->objetivosCumplidods);
						personaje->pendiente='\0';
						personaje->solicitud='\0';

						/////////////FIXME Agregue todo lo que esta aca abajo porque no me toma
						//cuando un personaje agarra un ultimo recurso por el cual estaba bloqueado y termina el nivel
						//ES lo mismo que l oque esta en bytesRecibidos ==0
						log_debug(nivel->logger,"%s Agarró su ultimo recurso",personaje->nombre);
						BorrarItem(&ListaItems,personaje->identificador);


						char* recursosLibres=string_new();	//No uso mi otra funcion porque la otra no libera los recursos
						while(personaje->objetivosCumplidods->elements_count != 0){
							t_recurso * unRecurso = list_remove(personaje->objetivosCumplidods,0);
							//log_debug(nivel->logger,"remueve el recurso %c de la lista",unRecurso->identificador);
							string_append_with_format(&recursosLibres,"%d%c",unRecurso->instancias,unRecurso->identificador);
							if(personaje->objetivosCumplidods->elements_count != 0)
								string_append(&recursosLibres,"|");
							free (unRecurso);
						}
						log_debug(nivel->logger,"Recursos liberados : %s por %s",recursosLibres,personaje->nombre);
						t_nivelConRecursos* recursosDelNivel = malloc(sizeof(t_nivelConRecursos));
						recursosDelNivel->nomNivel=malloc(strlen(nivel->nombre)+1);
						recursosDelNivel->recursos=malloc(strlen(recursosLibres)+1);

						strcpy(recursosDelNivel->nomNivel,nivel->nombre);
						strcpy(recursosDelNivel->recursos,recursosLibres);

						char *streamRecursos = serializarNivelConRecursos(recursosDelNivel);
						paquete = crear_paquete(recursosLiberados,  streamRecursos, strlen(recursosDelNivel->nomNivel)+1 + strlen(recursosDelNivel->recursos)+1);
						//log_debug(nivel->logger,"Envie los datos al orquestador");
						enviar_paquete(paquete, socketOrquestador);
						destruir_paquete(paquete);
						free(streamRecursos);
						free(recursosDelNivel->recursos);
						free(recursosDelNivel->nomNivel);
						free(recursosDelNivel);
						free(recursosLibres);
						//log_debug(nivel->logger,"Esperando los datos del orquestador");
						paquete = recibir_paquete(socketOrquestador);	//Me da los recursos que le sobran
						if(paquete->payloadLength<=0){
							close(socketOrquestador);
							return EXIT_SUCCESS;
							//EL orquestador termino, y esta ejecutando koopa
						}

						log_debug(nivel->logger,"Recursos restantes: %s", paquete->payload);
						int i;
						int instancia;
						char id;
						//Leo toda la cadena y voy incrementando las instancias a los recursos
						for(i=0;paquete->payload[i]!='\0';i++){
							instancia= atoi(&paquete->payload[i++]);
							id = paquete->payload[i++];
							sumarRecurso(ListaItems,nivel->cajas,id,instancia);
							if(paquete->payload[i]=='\0')
								i--;
						}

						destruir_paquete(paquete);

						list_remove(nivel->personajes,indice);
						imprimirPersonajes(nivel->logger, nivel->personajes);
						log_debug(nivel->logger,"a punto de dibujar en Handshake");
						nivel_gui_dibujar(ListaItems);
						log_debug(nivel->logger,"se dibujo");
						done=1;
						//FIN DE LO QUE AGREGUE-------------------------------------------------------


						break;

					default:
						close(socketNuevaConexion);
						break;
					}


				}
				if (bytesRecibidos == -1){
					printf("bytesRecibidos == -1");
					if (errno != EAGAIN){
						perror ("read");
						done = 1;
					}
					break;
				}
				else if (bytesRecibidos == 0){
					log_debug(nivel->logger,"bytes == 0 de alguien");
					/*Busca y borra personaje de la lista de personajes*/
					imprimirPersonajes(nivel->logger, nivel->personajes);
					elementoPersonaje = buscaPersona(nivel->personajes,&indice,events[i].data.fd);
					if(elementoPersonaje==NULL)
						log_debug(nivel->logger,"no se encontro el personaje en la lista");
					unPersonaje* personajeSaliente= elementoPersonaje->data;
					log_debug(nivel->logger,"Se fue %s",personajeSaliente->nombre);
					/*Borrado del personaje que se desconecta en pantalla*/
					BorrarItem(&ListaItems,personajeSaliente->identificador);


					char* recursosLibres=string_new();	//No uso mi otra funcion porque la otra no libera los recursos
					while(personajeSaliente->objetivosCumplidods->elements_count != 0){
						t_recurso * unRecurso = list_remove(personajeSaliente->objetivosCumplidods,0);
						//log_debug(nivel->logger,"remueve el recurso %c de la lista",unRecurso->identificador);
						string_append_with_format(&recursosLibres,"%d%c",unRecurso->instancias,unRecurso->identificador);
						if(personajeSaliente->objetivosCumplidods->elements_count != 0)
							string_append(&recursosLibres,"|");
						free (unRecurso);
					}
					imprimirCajas(nivel->logger,nivel->cajas);

					log_debug(nivel->logger,"Recursos liberados : %s por %s",recursosLibres,personajeSaliente->nombre);
					t_nivelConRecursos* recursosDelNivel = malloc(sizeof(t_nivelConRecursos));
					recursosDelNivel->nomNivel=malloc(strlen(nivel->nombre)+1);
					recursosDelNivel->recursos=malloc(strlen(recursosLibres)+1);

					strcpy(recursosDelNivel->nomNivel,nivel->nombre);
					strcpy(recursosDelNivel->recursos,recursosLibres);

					char *streamRecursos = serializarNivelConRecursos(recursosDelNivel);
					paquete = crear_paquete(recursosLiberados,  streamRecursos, strlen(recursosDelNivel->nomNivel)+1 + strlen(recursosDelNivel->recursos)+1);
					//log_debug(nivel->logger,"Envie los datos al orquestador");
					enviar_paquete(paquete, socketOrquestador);
					destruir_paquete(paquete);
					free(streamRecursos);
					free(recursosDelNivel->recursos);
					free(recursosDelNivel->nomNivel);
					free(recursosDelNivel);
					free(recursosLibres);
					//log_debug(nivel->logger,"Esperando los datos del orquestador");
					paquete = recibir_paquete(socketOrquestador);	//Me da los recursos que le sobran
					if(paquete->payloadLength==0){
						close(socketOrquestador);
						log_debug(nivel->logger,"La plataforma esta ejecutando al mister koopa!");
						return EXIT_SUCCESS;
						//EL orquestador termino, y esta ejecutando koopa
					}

					log_debug(nivel->logger,"Recursos restantes: %s", paquete->payload);
					int i;
					int instancia;
					char id;
					//Leo toda la cadena y voy incrementando las instancias a los recursos
					log_debug(nivel->logger,"Actualizo las Cajas");
					for(i=0;paquete->payload[i]!='\0';i++){
						instancia= atoi(&paquete->payload[i++]);
						id = paquete->payload[i++];
						if(paquete->payload[i]=='\0')
							i--;
						sumarRecurso(ListaItems,nivel->cajas,id,instancia);

					}
					destruir_paquete(paquete);
					list_remove(nivel->personajes,indice);
					imprimirCajas(nivel->logger,nivel->cajas);
					imprimirPersonajes(nivel->logger, nivel->personajes);

					log_debug(nivel->logger,"a punto de dibujar en BytesRecibidos==0");
					nivel_gui_dibujar(ListaItems);
					log_debug(nivel->logger,"Se dibujo");

					done = 1;
					break;
				}
				if (done){
					printf ("Closed connection on descriptor %d\n",events[i].data.fd);

					close (events[i].data.fd);
				}	//Fin - if(done)
			} 		//Fin - else
                // pthread_mutex_unlock(mutexDeadlock); //Lo cambie de lugar Mariano
		} 			//Fin - for (i = 0; i < n; i++)

		pthread_mutex_unlock(mutexDeadlock);                   // --------------------------

	} 				//Fin while(1)

	free(rec);

	free (events);
	close (socketEscucha);
	return EXIT_SUCCESS;
}


int realizarHandshakeOrquestador(t_nivel* nivel,int socketOrquestador,char* direccionNivel){
	package* paquete=NULL;
	printf("Realizadno el Handshake Nivel!");
	t_presentacionNivel* presentacionNivel=malloc(sizeof(t_presentacionNivel));
	presentacionNivel->nomNivel = nivel->nombre;      //cambiar a nivel que va
	presentacionNivel->ipPuertoNivel = direccionNivel;  //cambiar a nombre por config
	char *streamPresent = serializarPresNivel(presentacionNivel);
	printf("presentacion de nivel que se envia: %s\n",streamPresent);
	paquete = crear_paquete(handshakeNivel, streamPresent, strlen(presentacionNivel->nomNivel)+1 + strlen(presentacionNivel->ipPuertoNivel)+1 );
	free(presentacionNivel);
	free(streamPresent);
	enviar_paquete(paquete, socketOrquestador);
	destruir_paquete(paquete);
	paquete = recibir_paquete(socketOrquestador);
	if((paquete->type == respuestaHandshakeNivel)){
		destruir_paquete(paquete);
		return 1;
	};	//OK
	destruir_paquete(paquete);
	return 0;	//ERROR
}

char* streamRecursosAgarrados(t_list* recursos){

	t_link_element* elementoLista = recursos->head;	//Primer elemento de la lista
	char* stream= string_new();
	t_recurso* unRecurso;
	while(elementoLista!=NULL){
		unRecurso=(t_recurso*)elementoLista->data;
		string_append_with_format(&stream,"%d%c",unRecurso->instancias,unRecurso->identificador);
		elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
		if(elementoLista!=NULL)
			string_append(&stream,"|");
	}
	return stream;
}

void* deadlock(void* nivelFeo){

	t_nivel* nivel=(t_nivel*) nivelFeo;
	seTerminoLaActualizacionDeRecursos=true;
	//unPersonaje* personaje;
	//int indice;
	while(1){

		sleep(nivel->t_chequeoDL);

		pthread_mutex_lock(mutexDeadlock);                  // --------------------------
		log_debug(nivel->logger,"<<<<<<<<COMIENZO CHEQUEO DEADLOCK>>>>>>>>>>");

		//imprimirCajas(nivel->logger,nivel->cajas);

		t_list* culpablesDelInterbloqueo= chequearInterbloqueo(nivel->personajes,nivel->cajas);
		if(culpablesDelInterbloqueo!=NULL){
			char* culpables= loguearInterbloqueo(nivel,culpablesDelInterbloqueo);
			if(nivel->recovery==1){
				t_nivelConCulpables* culpablesDelNivel = malloc(sizeof(t_nivelConCulpables));
				culpablesDelNivel->nomNivel=malloc(strlen(nivel->nombre)+1);
				culpablesDelNivel->personajes=malloc(strlen(culpables)+1);

				strcpy(culpablesDelNivel->nomNivel,nivel->nombre);
				strcpy(culpablesDelNivel->personajes,culpables);

				char *streamCulpables = serializarNivelConCulpables(culpablesDelNivel);

				package* paquete = crear_paquete(notificacion_interbloqueo, streamCulpables, strlen(culpablesDelNivel->nomNivel)+1 + strlen(culpablesDelNivel->personajes)+1 );
				enviar_paquete(paquete, socketOrquestador);
				destruir_paquete(paquete);
				free(streamCulpables);
				actualizados=culpablesDelInterbloqueo->elements_count-1;//Le resto uno por

				//FREE de culpables de interbloqueo y pero no de todos sus elementos FIXME

				paquete = recibir_paquete(socketOrquestador);		//Personaje que elimino
				log_debug(nivel->logger,"RECOVERY activado --- culpable de interbloqueo: %s",paquete->payload);
				destruir_paquete(paquete);

			}else{
				log_debug(nivel->logger,"Hay Interbloqueo pero no esta activado el RECOVERY");
			}

		}
		else
			log_debug(nivel->logger,"No hay Interbloqueo,esta todo joya, sigan jugando muchachos");

		log_debug(nivel->logger,"<<<<<<<FIN CHEQUEO DEADLOCK>>>>>>>>>");
		pthread_mutex_unlock(mutexDeadlock);

	}
	return NULL;
}

char* loguearInterbloqueo(t_nivel* nivel,t_list* personajes){
	char* mensaje=(char*)malloc(strlen("Culpables de Interbloqueo: ")+1);
	char* culpables=string_new();
	strcpy(mensaje,"Culpables de Interbloqueo: ");
	t_link_element* elemento=personajes->head;
	unPersonaje* personaje;
	while(elemento!=NULL){
		personaje=(unPersonaje*)elemento->data;
		if(!string_ends_with(mensaje,": ")){
			string_append(&culpables,", ");
			string_append(&mensaje,", ");
		}
		string_append(&culpables,personaje->nombre);
		string_append(&mensaje,personaje->nombre);
		elemento=elemento->next;
	}

	log_debug(nivel->logger,mensaje);
	free(mensaje);
	return culpables;

}

void actualizarRecursosDisponibles(unPersonaje* personaje,t_list* cajas){

			void _actualizarCaja(t_recurso *unObjetivo) {
				t_caja* unaCaja=encontrarCaja(cajas,unObjetivo->identificador);
				unaCaja->instancias= unaCaja->instancias + unObjetivo->instancias;
			}
			list_iterate(personaje->objetivosCumplidods, (void*) _actualizarCaja);

			//printf("Ejecuta: %s\n",personaje->nombre);
			//imprimirCajas(cajas);
	}



bool puedeEjecutar(unPersonaje* personaje,t_list* cajas){

	if(personaje->solicitud=='\0')
		return true;

	t_caja* unaCaja=NULL;

	unaCaja=encontrarCaja(cajas,personaje->solicitud);

	if(unaCaja->instancias>=1)
		return true;
	else
		return false;

}



t_list* generarCajasParaSimulacion(t_list* cajas){
	t_list* listaCajas = list_create();
	t_caja* unaCaja;
	t_caja* nuevaCaja;

	t_link_element* elementoLista=cajas->head;
	while(elementoLista!=NULL){
		unaCaja=(t_caja*)elementoLista->data;
		nuevaCaja=(t_caja*)malloc(sizeof(t_caja));

		nuevaCaja->simbolo=unaCaja->simbolo;
		nuevaCaja->instancias=unaCaja->instancias;
		list_add(listaCajas,nuevaCaja);

		elementoLista=elementoLista->next;
	}
	return listaCajas;
}


unPersonaje* personajeParaEjectuar(t_list *personajes,t_list* cajas) {

	unPersonaje* personaje=NULL;
	t_link_element *elementoLista = personajes->head;
	int position = 0;
	while (elementoLista != NULL && !puedeEjecutar((unPersonaje*)elementoLista->data,cajas)) { //Falta hacer la funcion puedeEjecutar!!! esa es la grosa
		elementoLista = elementoLista->next;
		position++;
	}
	if (elementoLista != NULL) {		//Encontre uno que puede ejecutar
		personaje=(unPersonaje*)list_remove(personajes, position);
		return personaje;
	}
	return NULL;
}

//HACER free de caja simulacion con todos sus  elementos FIXME
//
t_list* chequearInterbloqueo(t_list* personajes,t_list* cajas){
	//esta funcion tiene la posta!!!!
	unPersonaje* personaje=NULL;
	t_list* cajasSimulacion=generarCajasParaSimulacion(cajas);
	//Hago esto porque voy a modificar la lista de cajas para simular ,la entrega de recursos.
	//El vector disponibilidad se calcula con las cajas y se modifica las instancias de las cajas

	t_list* personajesSimulacion=list_create();
	//los personajes no se modifican, por eso no genero otra lista nueva desde cero,
	//solamente copio la anterior.Al copiar, solamente se copia la direccio nde memoria, y si modifico
	//en una lista, se modifica en la otra, ya lo probe.En esta, puedo, sacar personajes pero no pasa nada.tods OK!
	list_add_all(personajesSimulacion,personajes);

	imprimirCajas(loggerGlobal,cajasSimulacion);
	imprimirPersonajes(loggerGlobal,personajesSimulacion);

	personaje=personajeParaEjectuar(personajesSimulacion,cajasSimulacion);
	while(personaje!=NULL){
		log_debug(loggerGlobal,"personaje para ejecutar: %s",personaje->nombre);
		log_debug(loggerGlobal,"tiene los siguientes objetivosCumplidos=");

		imprimirObjetivosCumplidos(loggerGlobal,personaje->objetivosCumplidods);


		actualizarRecursosDisponibles(personaje,cajasSimulacion);
		imprimirCajas(loggerGlobal,cajasSimulacion);
		personaje=personajeParaEjectuar(personajesSimulacion,cajasSimulacion);
	}

	list_destroy(cajasSimulacion);

	if(personajesSimulacion->head==NULL)	//IF (No quedo ningun personaje en la cola para ejecutar)
		return NULL;						//Entonces no hay interbloqueo
	else									//
		return personajesSimulacion;

}




t_caja* encontrarCaja(t_list* cajas, char simbolo){
	t_link_element* elementoLista = cajas->head;	//Primer elemento de la lista
	t_caja* caja=NULL;

	while(elementoLista!=NULL){ //mientras que sea el fin de la lista
		caja=(t_caja*)elementoLista->data; //data contiene el nivel propiamente dicho
		if(caja->simbolo==simbolo)
			break;
		elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
		}
return caja;

}

void tomarInstancia(unPersonaje* personaje, t_caja* caja){
	--caja->instancias;
	list_add(personaje->objetivosCumplidods,&caja->simbolo);
}
