
#include <commons/collections/list.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "estructurasNivel.h"
#include <stdio.h>
#include <string.h>


/*****************************************************
	Comienzo-Funciones relacionadas con Archivo de Configuracion
******************************************************/


/* @DESC: carga la informacion de arrayComponentes y la mete en unaCaja
 * @PARAMS:
 * 		unaCaja - es una caja de la lista de cajas que esta dentro de nivel
 *		arrayComponentes es un array sacado del archivo de configuracion
 */

void inicializarCaja(t_caja* unaCaja,char** arrayComponentes){
	//arrayComponente= [nombre,simbolo,instancias,posX,posY]

	unaCaja->nombre=string_duplicate(arrayComponentes[0]);
	memcpy(&unaCaja->simbolo,arrayComponentes[1],1); // es medio raro pero funciona, preguntar!!
	unaCaja->instancias=atoi(arrayComponentes[2]);
	unaCaja->posX=atoi(arrayComponentes[3]);
	unaCaja->posY=atoi(arrayComponentes[4]);
}

/* @DESC: lee el archivo de configuracion y lo guarda en la estructura nivel
 * @PARAMS:
 * 		nivel - estructura general del nivel
 *		dir_config - direccion del archivo de configuracion
 *		dir_log - direccion del archivo de logueo
 */


void cargarConfiguracionNivel(t_nivel* nivel,char* dir_config){
	t_config* config=config_create(dir_config);
	nivel->nombre=config_get_string_value(config,"Nombre");
	nivel->recovery=config_get_int_value(config,"Recovery");
	nivel->t_chequeoDL=config_get_int_value(config,"TiempoChequeoDeadlock");
	nivel->dir_orquestador=config_get_string_value(config,"Orquestador");
	char* archivoLog = string_new();
	string_append_with_format(&archivoLog, "%s.log",nivel->nombre);
	nivel->logger=log_create(archivoLog,nivel->nombre,false,LOG_LEVEL_DEBUG);
	nivel->miDireccion=config_get_string_value(config,"miDireccion");
	nivel->cajas=list_create();
	nivel->personajes= list_create();
	int i=1;
	char** arrayComponentes;
	char* stringComponentes;
	char* keyCaja=(char*)malloc(7);	 //Volver a revisar ese 7, es para "cajaX"
	sprintf(keyCaja,"Caja%d",i);
	while(config_has_property(config,keyCaja)){
		t_caja *unaCaja=malloc(sizeof(t_caja));
		stringComponentes=config_get_string_value(config,keyCaja);
		arrayComponentes=string_split(stringComponentes,",");
		inicializarCaja(unaCaja,arrayComponentes);
		list_add(nivel->cajas,unaCaja);
		i++;
		sprintf(keyCaja,"Caja%d",i);
	}
	free(archivoLog);
	log_debug(nivel->logger,"//////////////////////////////////////COMIENZO DEL NIVEL//////////////////////////////////////\n");
	free(keyCaja);
	for(i=0;i<=4;i++) //5 componentes tiene una CajaX
		free(arrayComponentes[i]);
	free(arrayComponentes);
}

void imprimirCajas(t_log* logger,t_list* cajas){
	char* cajitas=string_new();
	string_append(&cajitas,"cajas =");
	void _imprimirUnaCaja(t_caja *unaCaja) {
		string_append_with_format(&cajitas," %d%c",unaCaja->instancias,unaCaja->simbolo);
	}
	list_iterate(cajas, (void*) _imprimirUnaCaja);
	log_debug(logger,cajitas);
	free(cajitas);
}

void imprimirObjetivosCumplidos(t_log* logger,t_list* cajas){
	if(cajas->elements_count==0)
		log_debug(logger,"---");
	else{
		char* cadenaObjetivos=string_new();
		void _imprimirUnObjetivo(t_recurso *recurso) {
			string_append_with_format(&cadenaObjetivos,"%d%c ",recurso->instancias,recurso->identificador);
		}
		list_iterate(cajas, (void*) _imprimirUnObjetivo);
		log_debug(logger,cadenaObjetivos);
		free(cadenaObjetivos);
	}
}

void imprimirPersonajes(t_log* logger,t_list* personajes){
	char* chabones =string_new();
	string_append(&chabones,"personajes: ");
	if(personajes->elements_count==0)
		string_append(&chabones," ---");
	else{
		void _imprimirUnPersonaje(unPersonaje *personaje) {
			string_append_with_format(&chabones,"%s ",personaje->nombre);
		}
		list_iterate(personajes, (void*) _imprimirUnPersonaje);
	}
	log_debug(logger,chabones);
	free(chabones);
}




/*****************************************************
	Comienzo-Funciones relacionadas con Archivo de Configuracion
******************************************************/
