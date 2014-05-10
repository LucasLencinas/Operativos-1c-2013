#ifndef SERIALIZAR_H_
#define SERIALIZAR_H_


#include <stdint.h>
#include <commons/collections/list.h>


typedef struct{
	char* nomNivel;
	char* nomPersonaje;
}__attribute__((packed)) t_presentacionPersonaje;

typedef struct{
	char* ipPuertoPlanif;
	char* ipPuertoNivel;
}__attribute__((packed)) t_respuestaPresentacion;

typedef struct{
	char *nomNivel;
	char *ipPuertoNivel;
}__attribute__((packed)) t_presentacionNivel;

typedef struct{
	char *nomNivel;
	char *recursos;
}__attribute__((packed)) t_nivelConRecursos;

typedef struct{
	char *nomNivel;
	char *personajes;
}__attribute__((packed)) t_nivelConCulpables;





char* serializarPresentacionPersonaje(t_presentacionPersonaje * pers);
t_presentacionPersonaje * desserializarPresentPersonaje (char* pers);

char* serializarRespuestaPresent(t_respuestaPresentacion * resp);
t_respuestaPresentacion * desserializarRespuestaPresent (char* resp);

char* serializarPresNivel(t_presentacionNivel * pres);
t_presentacionNivel * desserializarPresNivel (char* pres);

char* serializarNivelConRecursos(t_nivelConRecursos * pres);
t_nivelConRecursos * desserializarNivelConRecursos(char* pres);

char* serializarNivelConCulpables(t_nivelConCulpables * pres);
t_nivelConCulpables * desserializarNivelConCulpables(char* pres);


#endif /* SERIALIZAR_H_ */
