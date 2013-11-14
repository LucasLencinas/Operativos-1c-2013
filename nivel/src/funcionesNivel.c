
#include "funcionesNivel.h"
#include "tad_items.h"
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


//Funcion que usa el Nivel para obtener la posicion X e Y del recurso
t_recurso *buscarRecurso( ITEM_NIVEL *itemss,char rec){
	ITEM_NIVEL *aux = itemss;
	while (aux != NULL && aux->id != rec){
		aux = aux->next;
	}
	t_recurso *estructura = (t_recurso *) malloc(sizeof(t_recurso));
	estructura->recx = aux->posx;
	estructura->recy = aux->posy;
	estructura->identificador = aux->id;
	estructura->instancias = aux->quantity;
	return estructura;
}

unPersonaje *nuevoPersonaje(int unDescriptor, char peridentificador, char* nombrePersonaje, int posx, int posy)
{
		unPersonaje *new = malloc( sizeof(unPersonaje) );
		new->socket_cliente = unDescriptor;
		new->nombre=malloc(strlen(nombrePersonaje)+1);
		strcpy(new->nombre,nombrePersonaje);
		new->identificador = peridentificador;
		new->objetivosCumplidods = list_create();
		new->pos_x = posx;
		new->pos_y = posy;
		new->solicitud='\0';
		return new;
}

	/*Busca un personaje en una lista de acuerdo a una condición*/
void* buscaPersonaje(t_list *self,int unDescriptor) {
		t_link_element *element = buscaPersona(self, NULL, unDescriptor);
		return element != NULL ? element->data : NULL;
}

	/*Condición para función buscaPersona */
int esElPersonaje(unPersonaje *p, int unDescriptor) {
			        return ((p->socket_cliente) == unDescriptor);
}

	/*Busca en una lista una estructura que contiene un personaje */
t_link_element* buscaPersona(t_list *self,int* index,int unDescriptor) {
		t_link_element *element = self->head;
		int position = 0;

		while (element != NULL && !esElPersonaje(element->data, unDescriptor)) {
			element = element->next;
			position++;
		}

		if (index != NULL) {
			*index = position;
		}

		return element;
}

unPersonaje* encontrarPersonaje(t_list* lista, char* nombre){

t_link_element* elementoLista = lista->head;	//Primer elemento de la lista
unPersonaje* personaje=NULL;

while(elementoLista!=NULL){ //mientras que sea el fin de la lista
	personaje=(unPersonaje*)elementoLista->data; //data contiene el nivel propiamente dicho
	if((strcmp(personaje->nombre,nombre))==0){ //se fija si el nombre del nivel es el mismo que elnombre pasado por parametro
		return personaje;		//termina el while
	}
	elementoLista=elementoLista->next; //pasa al siguiente elemento de la lista
}
return NULL;

}



/*Busca un recurso en una lista de acuerdo a una condición*/
void* buscaRecurso(t_list *self,char unRecurso) {
	t_link_element *element = buscaUnRecurso(self, NULL, unRecurso);
	return element != NULL ? element->data : NULL;
}

/*Condición para función buscaUnRecurso */
int esElRecurso(t_recurso *p, char unRecurso) {
		        return ((p->identificador) == unRecurso);
}

/*Busca en una lista una estructura que contiene un Recurso */
t_link_element* buscaUnRecurso(t_list *self,int* index,char unRecurso) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !esElRecurso(element->data, unRecurso)) {
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}

void	cargarCajas(t_list *cajas,ITEM_NIVEL** ListaItems,int limitex,int limitey){
		t_link_element *element = cajas->head;
		t_caja *unaCaja;
		int auxPosX,auxPosY;
		while (element != NULL){
		unaCaja= (t_caja *)element->data;

		auxPosX = comparaBordes(unaCaja->posX,limitex);
		auxPosY = comparaBordes(unaCaja->posY,limitey);

		CrearCaja(ListaItems, unaCaja->simbolo, auxPosX, auxPosY,unaCaja->instancias);
					element = element->next;
		}

}

int comparaBordes(int posicion,int borde){
	int auxiliar;
	if (posicion >= borde){
				auxiliar= borde;
			     }
			else{
				  auxiliar = posicion;
			     }
	return auxiliar;
}

void sumarRecurso(ITEM_NIVEL* ListaItems,t_list* cajas, char id,int instancias) {

        ITEM_NIVEL * temp;
        temp = ListaItems;

        t_link_element* elementoLista = cajas->head;	//Primer elemento de la lista
        t_caja* caja=NULL;

        while(elementoLista!=NULL){
        	caja=(t_caja*)elementoLista->data;
        	if(caja->simbolo==id){
        		break;
        	}
        	elementoLista=elementoLista->next;
        	caja=NULL;
        }


        while ((temp != NULL) && (temp->id != id)) {
                temp = temp->next;
        }
        if ((temp != NULL) && (temp->id == id)) {
                if (temp->item_type) {
                	caja->instancias=caja->instancias + instancias;
                	temp->quantity = temp->quantity + instancias;
                }
        }

}
