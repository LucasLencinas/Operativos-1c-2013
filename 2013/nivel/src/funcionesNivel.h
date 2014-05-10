#ifndef FUNCIONESNIVEL_H_
#define FUNCIONESNIVEL_H_

#include <stdint.h>
#include "nivel.h"
#include <stddef.h>
#include "nipc.h"
#include "serializar.h"
#include "estructurasNivel.h"

t_recurso *buscarRecurso( ITEM_NIVEL* itemss,char rec);
unPersonaje *nuevoPersonaje(int unDescriptor, char peridentificador,char* nombrePersonaje, int posx, int posy);
void* buscaPersonaje(t_list *self,int unDescriptor);
int esElPersonaje(unPersonaje *p, int unDescriptor);
t_link_element* buscaPersona(t_list *self,int* index,int unDescriptor);
void	cargarCajas(t_list *cajas,ITEM_NIVEL** ListaItems,int limitex,int limitey);
int comparaBordes(int unaPosicion,int unLimite);


void* buscaRecurso(t_list *self,char unRecurso);
int esElRecurso(t_recurso *p, char unRecurso);
t_link_element* buscaUnRecurso(t_list *self,int* index,char unRecurso);

void sumarRecurso(ITEM_NIVEL* ListaItems,t_list* cajas, char id, int instancias);

void notificarInterbloqueo(t_nivel* nivel,t_list* personajes);
void actualizarRecursosDisponibles(unPersonaje* unPersonaje,t_list* cajas);
t_list* generarCajasParaSimulacion(t_list* cajas);
t_caja* encontrarCaja(t_list* cajas, char simbolo);
void tomarInstancia(unPersonaje* personaje, t_caja* caja);
t_list* chequearInterbloqueo(t_list* personajes,t_list* cajas);


#endif /* FUNCIONESNIVEL_H_ */
