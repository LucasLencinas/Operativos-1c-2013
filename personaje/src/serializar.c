#include <stdlib.h>
#include "serializar.h"
#include <string.h>

/*t_stream *serializar_posiciones(t_recurso * estructura) {
	t_stream *stream = malloc(sizeof(stream));
	int size = 0, offset = 0;
	char * string = malloc(sizeof(int)*2);
	memcpy(string,&estructura->recx, size = sizeof(int));
	offset = size;
	memcpy(string + offset,&estructura->recy,size = sizeof(int));
	stream->payload = string;
	stream->length = offset + size;
	return stream;
}*/

t_rec	*deserializar_posiciones(char *cadena){
	t_rec * self = malloc (sizeof(t_rec));
	int size = 0, offset = 0;
	memcpy(&self->posicionx,cadena, size = sizeof(uint8_t));
	offset = size;
	memcpy(&self->posiciony,cadena + offset, size = sizeof(uint8_t));
	return self;
}





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

char* serializarPresPersonaje(t_presPersonaje * pres){
	char *stream = malloc(sizeof(char)+ strlen(pres->nombre)+1);
	int size=0, offset=0;
	memcpy(stream, &pres->simbolo, size = sizeof(char));
	offset += size;
	size = strlen(pres->nombre)+1;
	memcpy (stream + offset, pres->nombre, size);
	return stream;
}


t_presPersonaje * desserializarPresPersonaje(char* pres){
	int offset = 0, tmp_size = 0;
	t_presPersonaje * presentacion ;
	presentacion = malloc(sizeof(t_presPersonaje));

	memcpy(&presentacion->simbolo, pres, tmp_size = sizeof(char));

	offset = tmp_size;

	for (tmp_size=1 ; (pres+offset) [tmp_size-1] != '\0'; tmp_size++);
		presentacion->nombre = malloc (tmp_size);
	memcpy(presentacion->nombre, pres+offset, tmp_size);
	return presentacion;
}



char* serializarRespuestaPresent(t_respuestaPresentacion * resp){
	char *stream = malloc((strlen(resp->puertoPlanif)+1)  +  (strlen(resp->ipPuertoNivel)+1));
	int size=0, offset=0;
	memcpy(stream, resp->puertoPlanif, size = strlen(resp->puertoPlanif)+1);
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
	present->puertoPlanif = malloc (tmp_size);
	memcpy(present->puertoPlanif, resp+offset, tmp_size);

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

char* serializarMovimientoPermitido(t_movimientoPermitido * pers){
	char *stream = malloc(sizeof(uint8_t)+ strlen(pers->IDPersonaje)+1);
	int size=0, offset=0;
	memcpy(stream, &pers->numMovimiento, size = sizeof(uint8_t));
	offset += size;
	size = strlen(pers->IDPersonaje)+1;
	memcpy (stream + offset, pers->IDPersonaje, size);// ac aesta el problema
	return stream;
}

t_movimientoPermitido * desserializarMovimientoPermitido (char* pers){
	int offset = 0, tmp_size = 0;
	t_movimientoPermitido * mov ;
	mov = malloc(sizeof(t_movimientoPermitido));

	memcpy(&mov->numMovimiento, pers, tmp_size = sizeof(uint8_t));

	offset = tmp_size;

	for (tmp_size=1 ; (pers+offset) [tmp_size-1] != '\0'; tmp_size++);
	mov->IDPersonaje = malloc (tmp_size);
	memcpy(mov->IDPersonaje, pers+offset, tmp_size);
	return mov;
}

char* serializarAsignacionRecurso(t_asignacionRecurso * unRecurso){
	char *stream = malloc(sizeof(uint8_t)+ strlen(unRecurso->IDunRecurso)+1);
	int size=0, offset=0;
	memcpy(stream, &unRecurso->resultadoOperacion, size = sizeof(uint8_t));
	offset += size;
	size = strlen(unRecurso->IDunRecurso)+1;
	memcpy (stream + offset, unRecurso->IDunRecurso, size);// ac aesta el problema
	return stream;
}

t_asignacionRecurso * desserializarAsignacionRecurso(char* unRecurso){
	int offset = 0, tmp_size = 0;
	t_asignacionRecurso * rec ;
	rec = malloc(sizeof(t_asignacionRecurso));

	memcpy(&rec->resultadoOperacion, unRecurso, tmp_size = sizeof(uint8_t));

	offset = tmp_size;

	for (tmp_size=1 ; (unRecurso+offset) [tmp_size-1] != '\0'; tmp_size++);
	rec->IDunRecurso = malloc (tmp_size);
	memcpy(rec->IDunRecurso, unRecurso+offset, tmp_size);
	return rec;
}
