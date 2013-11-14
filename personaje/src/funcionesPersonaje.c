
#include "funcionesPersonaje.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>




//Funcion que usa el Personaje para moverse hacia el recurso
int	posicionarPersonaje(int unSocket, int Recx, int Recy,int *Perx,int *Pery){
		package *paquete = NULL;
		int movete = 1;
		int i = 0;

		if ((*Perx > Recx) &&(movete)){
			//MOVER A LA IZQUIERDA
			paquete = crear_paquete(movimientoPersonaje, "IZ", strlen("IZ") + 1 );
			enviar_paquete(paquete, unSocket);
			destruir_paquete(paquete);
			*Perx = *Perx - 1;
			movete = 0;
		  }
		else if ((*Perx < Recx)&&(movete)){
			//MOVER A LA DERECHA
			paquete = crear_paquete(movimientoPersonaje, "DE", strlen("DE") + 1 );
			enviar_paquete(paquete, unSocket);
			destruir_paquete(paquete);
			*Perx = *Perx + 1;
			movete = 0;
 		 }

		else if ((*Perx == Recx) && (movete)){

				if ((*Pery > Recy)&&(movete)){
					//MOVER PARA ARRIBA
					*Pery = *Pery - 1;
					paquete = crear_paquete(movimientoPersonaje, "AR", strlen("AR") + 1 );
					enviar_paquete(paquete, unSocket);
					destruir_paquete(paquete);
					movete = 0;
				}
				else if ((*Pery < Recy) &&(movete)){
					//MOVER PARA ABAJO
					*Pery = *Pery + 1;
					paquete = crear_paquete(movimientoPersonaje, "AB", strlen("AB") + 1 );
					enviar_paquete(paquete, unSocket);
					destruir_paquete(paquete);
					movete = 0;
				}
				else if ((*Pery == Recy) && (movete)){
					i = 1;
				}
		}
		if (*Pery==Recy && *Perx==Recx){
					return 1;
		}
	    else{
		return i;
	    }
 }


t_rec * pedirProximoRecurso(char *recurso, int unSocket){
	package *paquete = NULL;
	paquete = crear_paquete(handshake, recurso, strlen(recurso) + 1);
	enviar_paquete(paquete, unSocket);
	destruir_paquete(paquete);

	paquete = recibir_paquete(unSocket);
	t_rec * recursos = deserializar_posiciones(paquete->payload);
	destruir_paquete(paquete);

	return recursos;

}







