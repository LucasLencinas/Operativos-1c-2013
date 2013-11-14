#ifndef FUNCIONESPERSONAJE_H_
#define FUNCIONESPERSONAJE_H_

#include <stdint.h>
//#include "nivel.h"
#include <stddef.h>
#include "nipc.h"
#include "serializar.h"



int	posicionarPersonaje(int unSocket,int Recx, int Recy,int *Perx,int *Pery);
t_rec * pedirProximoRecurso(char * recurso, int unSocket);



#endif /* FUNCIONESPERSONAJE_H_ */
