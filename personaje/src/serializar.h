#ifndef SERIALIZAR_H_
#define SERIALIZAR_H_

#include <stdint.h>
//#include "estructurasNivel.h"
#include "estructurasPersonaje.h"

typedef struct{
	int32_t length;
	char*	payload;
} __attribute__((__packed__))t_stream;

typedef struct {
	uint8_t		posicionx	;
	uint8_t		posiciony	;
}__attribute__((__packed__)) t_rec;


typedef struct{
	char* nomNivel;
	char* nomPersonaje;
}__attribute__((packed)) t_presentacionPersonaje;

typedef struct{
	char* puertoPlanif;
	char* ipPuertoNivel;
}__attribute__((packed)) t_respuestaPresentacion;

typedef struct{
	char *nomNivel;
	char *ipPuertoNivel;
}__attribute__((packed)) t_presentacionNivel;

typedef struct{
	uint8_t numMovimiento;
	char* IDPersonaje;
}__attribute__((packed)) t_movimientoPermitido;

typedef struct{
	uint8_t resultadoOperacion;
	char* IDunRecurso;
}__attribute__((packed)) t_asignacionRecurso;

typedef struct{
	char* nombre;
	char simbolo;
}__attribute__((packed)) t_presPersonaje;


//t_stream *serializar_posiciones(t_recurso * estructura);
t_rec* deserializar_posiciones(char * cadena);

char* serializarPresentacionPersonaje(t_presentacionPersonaje * pers);
t_presentacionPersonaje * desserializarPresentPersonaje (char* pers);

char* serializarPresPersonaje(t_presPersonaje * pres);
t_presPersonaje * desserializarPresPersonaje(char* pres);

char* serializarRespuestaPresent(t_respuestaPresentacion * resp);
t_respuestaPresentacion * desserializarRespuestaPresent (char* resp);

char* serializarPresNivel(t_presentacionNivel * pres);
t_presentacionNivel * desserializarPresNivel (char* pres);

char* serializarMovimientoPermitido(t_movimientoPermitido * pers);
t_movimientoPermitido * desserializarMovimientoPermitido(char* pers);

char* serializarAsignacionRecurso(t_asignacionRecurso * pers);
t_asignacionRecurso * desserializarAsignacionRecurso(char* pers);

#endif /* SERIALIZAR_H_ */
