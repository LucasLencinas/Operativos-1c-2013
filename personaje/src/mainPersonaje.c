
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/string.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "nipc.h"
#include <curses.h>
#include "serializar.h"
#include "funcionesPersonaje.h"
#include "lectura_configuracion.h"
#include <commons/config.h>
#include <signal.h>

int socketPlanificador;
int bloqueado = 0;
t_personaje* personaje;
int vidasIniciales;
int incrementarVida = 0;
int muerte = 0;
int muerteDeadlock = 0;
int sigtermBloqueado = 0;
void atenderSignal (int n);
t_respuestaPresentacion* realizarHandshakeOrquestador(t_personaje* personaje,
		int socketOrquestador,t_nivel* nivelActual);

int main(int argc, char **argv){
	package *paquete = NULL;
	t_rec * recursoConPosicion;
	t_nivel* nivelActual;
	char *recurso_a_buscar;
	int socketNivel,socketOrquestador;
	int pedido;
	int Perx;
	int Pery;
	bool vuelveDeBloqueados =false;
	bool primerMensajePlanificador=false;
	bool tengoElRecurso;

	personaje=(t_personaje*)malloc(sizeof(t_personaje));
	cargarConfiguracionPersonaje(personaje,argv[1]);

	signal(SIGUSR1, atenderSignal);
	signal(SIGTERM, atenderSignal);


	printf("nombre=%s\n",personaje->nombre);
	printf("simbolo=%c\n",personaje->simbolo);
	printf("vidas=%d\n",personaje->vidas);
	printf("orquestador=%s\n",personaje->dir_orquestador);
	imprimirObjetivosXnivel(personaje->niveles);

	vidasIniciales = personaje->vidas;
	t_list * listaReinicio = list_create();
	list_add_all(listaReinicio, personaje->niveles);
	char** dir_orquest;

	while(personaje->niveles->elements_count != 0){
		usleep(1);
		Perx=1;
		Pery=1;

	dir_orquest= string_split(personaje->dir_orquestador,":");
	socketOrquestador= sockets_abrir_cliente(dir_orquest[0],atoi(dir_orquest[1]));

	log_debug(personaje->logger, "Conexion a Orquestador realizada");

	nivelActual=(t_nivel*)personaje->niveles->head->data;

	t_respuestaPresentacion* dirPlanif_y_Nivel=NULL;
	if((dirPlanif_y_Nivel=realizarHandshakeOrquestador(personaje,socketOrquestador,nivelActual))==0){
		printf("Error en Handshake\n");
		return EXIT_FAILURE;
	}


	//close(socketOrquestador);

	log_debug(personaje->logger,"Desconexion de orquestador realizada");
	/////////////// conectarme al planificador y nivel

	char ** ip_puertoNivel=string_split(dirPlanif_y_Nivel->ipPuertoNivel,":");
	socketPlanificador = sockets_abrir_cliente(dir_orquest[0],atoi(dirPlanif_y_Nivel->puertoPlanif));
	free(dir_orquest[0]);
	free(dir_orquest[1]);
	//free(dir_orquest);			//FIXME
	free(dirPlanif_y_Nivel->puertoPlanif);
	free(dirPlanif_y_Nivel->ipPuertoNivel);
	//free(dirPlanif_y_Nivel);

	log_debug(personaje->logger, "Conexion a planificador de %s realizada", nivelActual->nombre);

	socketNivel = sockets_abrir_cliente(ip_puertoNivel[0],atoi(ip_puertoNivel[1]));
	free(ip_puertoNivel[0]);
	free(ip_puertoNivel[1]);
	//free(ip_puertoNivel);		//FIXME


	log_debug(personaje->logger, "Conexion a %s realizada", nivelActual->nombre);


	t_presPersonaje* presAnivel= malloc(sizeof(t_presPersonaje));
	presAnivel->nombre=malloc(strlen(personaje->nombre)+1);
	strcpy(presAnivel->nombre,personaje->nombre);
	presAnivel->simbolo=personaje->simbolo;
	char* streamPres=serializarPresPersonaje(presAnivel);
	free(presAnivel->nombre);
	free(presAnivel);

    paquete = crear_paquete(presentacionPersonaje, streamPres, strlen(streamPres) + 1 );
   	enviar_paquete(paquete, socketNivel);			//NIVEL
   	destruir_paquete(paquete);
   	free(streamPres);

   	paquete = recibir_paquete(socketNivel);
   	if(paquete->payloadLength<=0){
   		log_debug(personaje->logger,"El paquete era menor que cero"); //que se caiga
   		return EXIT_FAILURE;
   	}else{
    log_debug(personaje->logger,"Recibi la confirmacion de creacion de personaje");
   	destruir_paquete(paquete);
   	}

    paquete = crear_paquete(presentacionPersonaje, personaje->nombre, strlen(personaje->nombre) + 1 );
	enviar_paquete(paquete, socketPlanificador);	//PLANIFICADOR
	if(paquete->payloadLength <= 0){
		perror("El planificador se murio o manda basura el puto antes de movimiento autorizado.");
		return EXIT_FAILURE;
	}
	destruir_paquete(paquete);
	primerMensajePlanificador=true;	//Negrada para probar algo

	paquete = recibir_paquete(socketPlanificador);
	if(paquete->payloadLength <= 0){
		perror("El planificador se murio o manda basura el puto antes de movimiento autorizado.");
		return EXIT_FAILURE;
	}
	destruir_paquete(paquete);
	log_debug(personaje->logger,"Movimiento autorizado");

	t_nivel * unNivel;
	unNivel= (t_nivel *)personaje->niveles->head->data;
	int i = 0;

	    while(  unNivel->objetivos[i] != '\0' && ((muerte == 0)  && (muerteDeadlock == 0)) ){

		recurso_a_buscar = unNivel->objetivos[i];
		i++;
		log_debug(personaje->logger,"Posicion del recurso %s solicitada",recurso_a_buscar);
		paquete = crear_paquete(posicionRecurso,recurso_a_buscar, strlen(recurso_a_buscar) + 1);
		enviar_paquete(paquete, socketNivel);
		destruir_paquete(paquete);

		paquete = recibir_paquete(socketNivel);
		if(paquete->payloadLength ==0){
			log_debug(personaje->logger,"El nivel se cayo, la puta madre!");
			return EXIT_FAILURE;
		}
		recursoConPosicion = deserializar_posiciones(paquete->payload);
		log_debug(personaje->logger,"Posicion del recurso %s recibida= [ %d , %d ]",
				recurso_a_buscar,recursoConPosicion->posicionx,recursoConPosicion->posiciony);
		destruir_paquete(paquete);

		tengoElRecurso=false;

		while(  !tengoElRecurso &&  ((muerte == 0)  && (muerteDeadlock == 0))  ){


			if(primerMensajePlanificador)
				primerMensajePlanificador=false;
			else{
				if (!vuelveDeBloqueados){

				paquete = recibir_paquete(socketPlanificador);
				if(paquete->payloadLength <= 0){
					perror("El planificador se murio o manda basura el puto antes de movimiento autorizado.");
					return EXIT_FAILURE;
				}
				destruir_paquete(paquete);
				log_debug(personaje->logger,"Movimiento autorizado");



				}
				else{
					vuelveDeBloqueados=false;
				}
			}

			pedido = posicionarPersonaje(socketNivel,recursoConPosicion->posicionx,recursoConPosicion->posiciony,&Perx,&Pery);

			paquete = recibir_paquete(socketNivel);
		   	if(paquete->payloadLength<=0){
		   		log_debug(personaje->logger,"Paquete de posicion menor que cero"); //que se caiga
		   	}else{
		    log_debug(personaje->logger,"Recibi la confirmacion de movimiento del nivel");
		   	destruir_paquete(paquete);
		   	}

			usleep(1);
			log_debug(personaje->logger,"Posicion = [ %d , %d ]",Perx, Pery);
			if (pedido == 1){	// Encontre mi caja

				//Pedir instancia del recurso
				paquete = crear_paquete(instanciaRecurso,recurso_a_buscar, strlen(recurso_a_buscar) + 1);
				enviar_paquete(paquete, socketNivel);
				destruir_paquete(paquete);
				log_debug(personaje->logger,"Instancia del recurso %s solicitada",recurso_a_buscar);

				paquete = recibir_paquete(socketNivel);
				if(paquete->payloadLength <= 0){
					perror("el nivel me corta la conexion despues de Instancia rec solicitada y antes de rec asignada.");
					return EXIT_FAILURE;
				}
				log_debug(personaje->logger,"Llego algo grande del nivel, mide %d ",paquete->payloadLength);
				t_asignacionRecurso * rec = desserializarAsignacionRecurso(paquete->payload);
				destruir_paquete(paquete);
				if (rec->resultadoOperacion == 1){
					log_debug(personaje->logger,"Instancia del recurso %s asignada",recurso_a_buscar);
					tengoElRecurso=true;
					if(unNivel->objetivos[i] == '\0'){// Si es el ultimo objetivo del nivel le mando finNivel sino no
						paquete = crear_paquete(movimiento_objetivoEncontrado, "finNivel", strlen("lo encontre")+1 );
						enviar_paquete(paquete,socketPlanificador);
						destruir_paquete(paquete);
						paquete = recibir_paquete(socketPlanificador);
						if(paquete->payloadLength<=0){
							perror("El planificador se murio o manda basura el muy sorete");
							return EXIT_FAILURE;

						}
						destruir_paquete(paquete);
					}
					else{
					paquete = crear_paquete(movimiento_objetivoEncontrado, "lo encontre", strlen("lo encontre")+1 );
					enviar_paquete(paquete, socketPlanificador);
					destruir_paquete(paquete);
					}

				}else if (rec->resultadoOperacion == 0){
					log_debug(personaje->logger,"Personaje Bloqueado");
					paquete = crear_paquete(movimiento_bloqueadoPorRecurso, recurso_a_buscar, 2 );
					enviar_paquete(paquete, socketPlanificador);
					destruir_paquete(paquete);

					bloqueado = 1;
					paquete = recibir_paquete(socketPlanificador);	//FIXME aca hay que hacer algo con sigterm porque un bloquedao no puede ejecutar a pesar de que le mando la señal
					int bytesRecibidos= paquete->payloadLength;
					if(bytesRecibidos > 0){
						if(unNivel->objetivos[i] == '\0'){// Si no hay mas objetivos
							log_debug(personaje->logger,"El recurso %s fue asignado",recurso_a_buscar);

							destruir_paquete(paquete);
							paquete = crear_paquete(movimiento_objetivoEncontrado, "finNivel", strlen("lo encontre")+1 );
							enviar_paquete(paquete, socketPlanificador);
							destruir_paquete(paquete);

							paquete = recibir_paquete(socketPlanificador);
							if(paquete->payloadLength<=0){

								perror("El planificador se murio o manda basura el muy sorete");
								return EXIT_FAILURE;

													}
							destruir_paquete(paquete);



							//LE mando un mensaje al nivel para que me actualice el objetivo que cumpli!
							paquete = crear_paquete(handshake,"actualizame el recurso",strlen("actualizame el recurso")+1);
							enviar_paquete(paquete,socketNivel);
							destruir_paquete(paquete);

							close(socketNivel);	//FIXME, me parece que se puede sacar este close, pero hay que probarlo

							//EL CLOSE de arriba no va, tiene que ir un recibir paquete con bytes recibidos ==0 FIXME
							//close(socketPlanificador);
						}
						else{
							log_debug(personaje->logger,"El recurso %s fue asignado",recurso_a_buscar);
							log_debug(personaje->logger,"Movimiento autorizado");
							vuelveDeBloqueados =true;
							destruir_paquete(paquete);

						}

						tengoElRecurso=true;

					}
					else{	//Bytes recibidos == 0, me mataron!
						//muerte =1;			FIXME ES UNA PRUEBA--------------------------------------------------------------
						muerteDeadlock=1;
						//destruir_paquete(paquete);
					}

				}

				//free(rec->IDunRecurso);		//FIXME creo que esto si va aca
				//free(rec);

			}else{// NO encontre la caja
				paquete = crear_paquete(movimiento_simple, "me movi", strlen("me movi")+1 );
				enviar_paquete(paquete, socketPlanificador);
				destruir_paquete(paquete);
			}
		}//FIn while(no tengo el recurso)
		usleep(1);
		free(recursoConPosicion);
	   }//FIN while(objetivos del nivel)



	   	log_debug(personaje->logger,"Desconexion de %s realizada", nivelActual->nombre);
		if(muerteDeadlock == 1 && sigtermBloqueado == 0){
			personaje->vidas = personaje->vidas - 1;


			/*FIXME, aca cuando lo corro por consola me tira segmentation fault
			 * */

			log_debug(personaje->logger,"Personaje muerto por Culpable interbloqueo" );
			log_debug(personaje->logger,"Vidas Actuales: %d",personaje->vidas);
			usleep(1);

		}

	    if (muerte == 0 && muerteDeadlock ==0){
	   	   list_remove(personaje->niveles, 0);
	    }
	    //ACA CREO QUE TIEN QUE IR UN IF FIXME
	   	close(socketNivel);

	   	if(sigtermBloqueado != 1 || muerteDeadlock != 1  ){
	   	close(socketPlanificador);
	   	}
	   //	sleep(1);

	   	muerte = 0;
	   	muerteDeadlock=0;
	   	bloqueado = 0;
	   	sigtermBloqueado = 0;

	   	if (personaje ->vidas == 0){
	   		usleep(1);
	   		   //reinicializar lista de niveles y reinicializar vidas iniciales
	   		   personaje ->vidas = vidasIniciales;
	   		   list_clean(personaje->niveles);
	   		   list_add_all(personaje->niveles, listaReinicio);

	   	   }
	   
	}//FIN while(plan de niveles)

	dir_orquest= string_split(personaje->dir_orquestador,":");
	socketOrquestador= sockets_abrir_cliente(dir_orquest[0],atoi(dir_orquest[1]));
	free(dir_orquest[0]);
	free(dir_orquest[1]);
	//free(dir_orquest);	//NO se si va

	usleep(1000);
	log_debug(personaje->logger, "Conexion FINAL a Orquestador realizada");
	paquete = crear_paquete(fin_personaje, "fin", strlen("fin")+1 );
	enviar_paquete(paquete,socketOrquestador);
	destruir_paquete(paquete);

	close(socketOrquestador);

	free(personaje->dir_orquestador);
	free(personaje->nombre);
	void nivel_destroy(t_nivel *self){
		free(self->nombre);
		free(self->objetivos);
		free(self);
	}

	list_destroy_and_destroy_elements(personaje->niveles,(void*) nivel_destroy);
	//list_destroy_and_destroy_elements(listaReinicio,(void*) nivel_destroy);
	log_destroy(personaje->logger);
	free(personaje);


	return EXIT_SUCCESS;

}

t_respuestaPresentacion* realizarHandshakeOrquestador(t_personaje* personaje,int socketOrquestador,t_nivel* nivelActual){
	package* paquete=NULL;
	t_presentacionPersonaje * presentacionPers = malloc(sizeof(t_presentacionPersonaje));
	presentacionPers->nomNivel = nivelActual->nombre;
	log_debug(personaje->logger,"nivelActual: %s",nivelActual->nombre);
	presentacionPers->nomPersonaje = personaje->nombre;
	char *streamPresent = serializarPresentacionPersonaje(presentacionPers);
	paquete = crear_paquete(handshakePresentacionPersonaje, streamPresent, strlen(presentacionPers->nomNivel)+1 + strlen(presentacionPers->nomPersonaje)+1 );
	enviar_paquete(paquete, socketOrquestador);
	free(streamPresent);
	//free(presentacionPers);
	destruir_paquete(paquete);
	paquete = recibir_paquete(socketOrquestador);

	if((paquete->type == respuestaHandshakePersonaje)){
		t_respuestaPresentacion * respuesta = desserializarRespuestaPresent(paquete->payload);
		destruir_paquete(paquete);
		log_debug(personaje->logger,"%s", respuesta->puertoPlanif  );
		log_debug(personaje->logger,"%s", respuesta->ipPuertoNivel  );

		close(socketOrquestador);
		return respuesta;
	}else {
		//destruir_paquete(paquete);	//FIXME no se si hace falta
		close(socketOrquestador);//Error, no esta levantado el orquestador o el nivel que quiero
		return NULL;
	}

}

void imprimirPosicion(t_rec* recurso){

}


void atenderSignal (int n) {
	switch (n) {
	package *paquete = NULL;
	int vidasAnteriores;
		case SIGTERM:

			if(personaje->vidas > 0){
				if (bloqueado != 1){
				muerte = 1;

				vidasAnteriores = personaje->vidas;
				personaje->vidas = personaje->vidas - 1;

				log_debug(personaje->logger,"Personaje muerto por señal SIGTERM" );
				log_debug(personaje->logger,"Personaje pierde una vida--vidas anteriores: %d |vidas actuales: %d",vidasAnteriores,personaje->vidas );
				}

				if (bloqueado == 1){
					sigtermBloqueado = 1;
					paquete = crear_paquete(handshake, "DEAD", strlen("DEAD")+1);
					enviar_paquete(paquete, socketPlanificador);
					destruir_paquete(paquete);

					vidasAnteriores = personaje->vidas;
					personaje->vidas = personaje->vidas - 1;

					log_debug(personaje->logger,"Personaje muerto por señal SIGTERM" );
					log_debug(personaje->logger,"Personaje pierde una vida--vidas anteriores: %d |vidas actuales: %d",vidasAnteriores,personaje->vidas );

				}
			}

		break;
		case SIGUSR1:


			vidasAnteriores = personaje->vidas;
			personaje->vidas = personaje->vidas + 1;
			log_debug(personaje->logger,"Vida del personaje incrementada--vidas anteriores: %d |vidas actuales: %d",vidasAnteriores,personaje->vidas );

		break;
	}
}
