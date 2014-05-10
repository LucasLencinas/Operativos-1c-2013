#include <stdlib.h>
#include "serializar.h"
#include <string.h>






char* serializarPresentacionPersonaje(t_presentacionPersonaje * pers){
	char *stream = malloc(strlen(pers->nomNivel)+1 + strlen(pers->nomPersonaje)+1);
	int size=0, offset=0;
	memcpy(stream, pers->nomNivel, size = strlen(pers->nomNivel)+1);
	offset += size;
	size = strlen(pers->nomPersonaje)+1;
	memcpy (stream + offset, pers->nomPersonaje, size);
	return stream;
}

t_presentacionPersonaje * desserializarPresentPersonaje (char* pers){
	int offset = 0, tmp_size = 0;
	t_presentacionPersonaje * present ;
	present = malloc(sizeof(t_presentacionPersonaje));

	for (tmp_size=1 ; (pers+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->nomNivel = malloc (tmp_size);
	memcpy(present->nomNivel, pers+offset, tmp_size);

	offset += tmp_size;

	for (tmp_size=1 ; (pers+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->nomPersonaje = malloc (tmp_size);
	memcpy(present->nomPersonaje, pers+offset, tmp_size);


	return present;

}



char* serializarRespuestaPresent(t_respuestaPresentacion * resp){
	char *stream = malloc((strlen(resp->ipPuertoPlanif)+1)  +  (strlen(resp->ipPuertoNivel)+1));
	int size=0, offset=0;
	memcpy(stream, resp->ipPuertoPlanif, size = strlen(resp->ipPuertoPlanif)+1);
    offset += size;
	size = strlen(resp->ipPuertoNivel)+1;
	memcpy (stream + offset, resp->ipPuertoNivel, size);//
	return stream;
}




t_respuestaPresentacion * desserializarRespuestaPresent (char* resp){
	int offset = 0, tmp_size = 0;
	t_respuestaPresentacion * present ;
	present = malloc(sizeof(t_respuestaPresentacion));

	for (tmp_size=1 ; (resp+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->ipPuertoPlanif = malloc (tmp_size);
	memcpy(present->ipPuertoPlanif, resp+offset, tmp_size);

	offset += tmp_size;

	for (tmp_size=1 ; (resp+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->ipPuertoNivel = malloc (tmp_size);
	memcpy(present->ipPuertoNivel, resp+offset, tmp_size);


	return present;
}

char* serializarPresNivel(t_presentacionNivel * pres){
	char *stream = malloc(strlen(pres->nomNivel)+1 + strlen(pres->ipPuertoNivel)+1);
	int size=0, offset=0;
	memcpy(stream, pres->nomNivel, size = strlen(pres->nomNivel)+1);
	offset += size;
	size = strlen(pres->ipPuertoNivel)+1;
	memcpy (stream + offset, pres->ipPuertoNivel, size);
	return stream;
}


t_presentacionNivel * desserializarPresNivel (char* pres){
	int offset = 0, tmp_size = 0;
	t_presentacionNivel * present ;
	present = malloc(sizeof(t_presentacionNivel));

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->nomNivel = malloc (tmp_size);
	memcpy(present->nomNivel, pres+offset, tmp_size);

	offset += tmp_size;

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->ipPuertoNivel = malloc (tmp_size);
	memcpy(present->ipPuertoNivel, pres+offset, tmp_size);


	return present;
}



char* serializarNivelConRecursos(t_nivelConRecursos * pres){
	char *stream = malloc(strlen(pres->nomNivel)+1 + strlen(pres->recursos)+1);
	int size=0, offset=0;
	memcpy(stream, pres->nomNivel, size = strlen(pres->nomNivel)+1);
	offset += size;
	size = strlen(pres->recursos)+1;
	memcpy (stream + offset, pres->recursos, size);
	return stream;
}


t_nivelConRecursos * desserializarNivelConRecursos (char* pres){
	int offset = 0, tmp_size = 0;
	t_nivelConRecursos * present ;
	present = malloc(sizeof(t_nivelConRecursos));

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->nomNivel = malloc (tmp_size);
	memcpy(present->nomNivel, pres+offset, tmp_size);

	offset += tmp_size;

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->recursos = malloc (tmp_size);
	memcpy(present->recursos, pres+offset, tmp_size);


	return present;
}





// PARA deadlcok
char* serializarNivelConCulpables(t_nivelConCulpables * pres){
	char *stream = malloc(strlen(pres->nomNivel)+1 + strlen(pres->personajes)+1);
	int size=0, offset=0;
	memcpy(stream, pres->nomNivel, size = strlen(pres->nomNivel)+1);
	offset += size;
	size = strlen(pres->personajes)+1;
	memcpy (stream + offset, pres->personajes, size);
	return stream;
}


t_nivelConCulpables * desserializarNivelConCulpables (char* pres){
	int offset = 0, tmp_size = 0;
	t_nivelConCulpables * present ;
	present = malloc(sizeof(t_nivelConCulpables));

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->nomNivel = malloc (tmp_size);
	memcpy(present->nomNivel, pres+offset, tmp_size);

	offset += tmp_size;

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
	present->personajes = malloc (tmp_size);
	memcpy(present->personajes, pres+offset, tmp_size);


	return present;
}




