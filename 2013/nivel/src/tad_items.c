#include "tad_items.h"
#include "stdlib.h"
#include "funcionesNivel.h"

void CrearItem(ITEM_NIVEL** ListaItems, char id, int x , int y, char tipo, int cant_rec) {
        ITEM_NIVEL * temp;
        temp = malloc(sizeof(ITEM_NIVEL));

        temp->id = id;
        temp->posx=x;
        temp->posy=y;
        temp->item_type = tipo;
        temp->quantity = cant_rec;
        temp->next = *ListaItems;
        *ListaItems = temp;
}



void CrearPersonaje(ITEM_NIVEL** ListaItems, char id, int x , int y) {
        CrearItem(ListaItems, id, x, y, PERSONAJE_ITEM_TYPE, 0);
}

void CrearCaja(ITEM_NIVEL** ListaItems, char id, int x , int y, int cant) {
        CrearItem(ListaItems, id, x, y, RECURSO_ITEM_TYPE, cant);
}

void BorrarItem(ITEM_NIVEL** ListaItems, char id) {
        ITEM_NIVEL * temp = *ListaItems;
        ITEM_NIVEL * oldtemp;

        if ((temp != NULL) && (temp->id == id)) {
                *ListaItems = (*ListaItems)->next;
		free(temp);
        } else {
                while((temp != NULL) && (temp->id != id)) {
                        oldtemp = temp;
                        temp = temp->next;
                }
                if ((temp != NULL) && (temp->id == id)) {
                        oldtemp->next = temp->next;
			free(temp);
                }
        }

}

void MoverPersonaje(ITEM_NIVEL* ListaItems, char id, int x, int y) {

        ITEM_NIVEL * temp;
        temp = ListaItems;

        while ((temp != NULL) && (temp->id != id)) {
                temp = temp->next;
        }
        if ((temp != NULL) && (temp->id == id)) {
                temp->posx = x;
                temp->posy = y;
        }

}


void restarRecurso(ITEM_NIVEL* ListaItems,t_list* cajas, char id) {

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

        if(caja==NULL)printf("caja == NULL\n");

        while ((temp != NULL) && (temp->id != id)) {
                temp = temp->next;
        }
        if ((temp != NULL) && (temp->id == id)) {
                if ((temp->item_type) && (temp->quantity > 0)) {
                		caja->instancias--;
                        temp->quantity--;
                }
        }

}

