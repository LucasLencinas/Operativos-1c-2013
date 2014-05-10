#include <commons/collections/list.h>
#include <stdlib.h>
#include <commons/config.h>
#include "estructurasPersonaje.h"
#include <stdio.h>
#include <string.h>
#include <commons/string.h>




/*****************************************************
	Comienzo-Funciones relacionadas con Archivo de Configuracion
******************************************************/

int cantidadObjetivosDe(char* stringObjetivos){
  int cantidad=0;
  int i;
  for(i=0;i<=strlen(stringObjetivos);i++)
    if(stringObjetivos[i]==',')
      cantidad++;
  return ++cantidad;
}


void cargarConfiguracionPersonaje(t_personaje* personaje,char* dir_config){		//FIXME -- ANDA bien supuestamente
	t_config* config=config_create(dir_config);
	personaje->nombre=config_get_string_value(config,"nombre");
	char* simboloString=config_get_string_value(config,"simbolo");
	personaje->simbolo=*simboloString;
	personaje->vidas=config_get_int_value(config,"vidas");
	personaje->dir_orquestador=config_get_string_value(config,"orquestador");
	char* archivoLog = string_new();
	string_append_with_format(&archivoLog, "%s.log",personaje->nombre);
	personaje->logger=log_create(archivoLog,personaje->nombre,true,LOG_LEVEL_DEBUG);

	char** planDeNiveles=config_get_array_value(config,"planDeNiveles");
	personaje->niveles=list_create();
	int i=0;
	char* stringObjetivos;
	char* keyObjetivoNivel;
	int maximoNiveles= cantidadObjetivosDe(config_get_string_value(config,"planDeNiveles"));

	while(i<maximoNiveles){
		t_nivel *unNivel=malloc(sizeof(t_nivel));
		keyObjetivoNivel=string_new();
		unNivel->nombre=string_new();
		string_append(&unNivel->nombre,planDeNiveles[i]);
		string_append_with_format(&keyObjetivoNivel,"obj[%s]",planDeNiveles[i]);

	    unNivel->objetivos=config_get_array_value(config,keyObjetivoNivel);    //['M','H','M','H']
	    stringObjetivos= config_get_string_value(config,keyObjetivoNivel); //"[M,H,M,H]"
	    unNivel->cantObjetivos=cantidadObjetivosDe(stringObjetivos);
		list_add(personaje->niveles,unNivel);
		i++;
	}
	log_debug(personaje->logger,"//////////////////////////////////////COMIENZO DEL PERSONAJE//////////////////////////////////////");
	//Borrado de memoria
	free(keyObjetivoNivel);
	i=0;
	while(i<maximoNiveles){
		free(planDeNiveles[i]);
		i++;
	}
	free(planDeNiveles);
	//config_destroy(config);

}




void imprimirObjetivosXnivel(t_list* niveles){
	void _imprimirNiveles(t_nivel *unNivel) {
		printf("%s=",unNivel->nombre);

	int i;
	for(i=0;i<unNivel->cantObjetivos;i++)
		printf("%c",*unNivel->objetivos[i]);
	printf("\n");
	}

	list_iterate(niveles, (void*) _imprimirNiveles);
}

/*****************************************************
	Fin-Funciones relacionadas con Archivo de Configuracion
******************************************************/

