
#ifndef LECTURA_CONFIGURACION_H_
#define LECTURA_CONFIGURACION_H_

#include <commons/collections/list.h>
#include "estructurasPersonaje.h"
#include <commons/config.h>

void cargarConfiguracionPersonaje(t_personaje* personaje,char* dir_config);
t_list* objetivosDe(char** arrayObjetivos,t_config* config);

void imprimirObjetivosXnivel(t_list* niveles);


#endif /* LECTURA_CONFIGURACION_H_ */
