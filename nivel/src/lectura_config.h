
#ifndef LECTURA_CONFIG_H_
#define LECTURA_CONFIG_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <commons/config.h>
#include "estructurasNivel.h"
#include <stdio.h>

void cargarConfiguracionNivel(t_nivel* nivel,char* dir_config);
void inicializarCaja(t_caja* unaCaja,char** arrayComponentes);
void imprimirCajas(t_log* logger,t_list* cajas);
void imprimirPersonajes(t_log* logger,t_list* personajes);
void imprimirObjetivosCumplidos(t_log* logger,t_list* cajas);

#endif /* LECTURA_CONFIG_H_ */
