#ifndef NIPC_H_
#define NIPC_H_

#include <stdint.h>

// Cabecera paquetes
typedef enum nipc_type{
	handshake,								//00
	handshakePresentacionPersonaje,			//01
	respuestaHandshakePersonaje,			//02
	respuestaHandshakePersonajeSinNivel,	//03
	handshakeNivel,							//04
	respuestaHandshakeNivel,				//05
	presentacionPersonaje,					//06
	presentacionNivel,						//07
	recursosLiberados,						//08
	recursosRestantes,						//09

	movimientoPermitido,					//10
	movimiento_simple,						//11
	movimiento_bloqueadoPorRecurso,			//12
	movimiento_objetivoEncontrado,			//13
	movimiento_finNivel,					//14
	movimientoPersonaje,					//15
	posicionRecurso,						//16
	instanciaRecurso,						//17
	solicitudMovimiento,					//18
	notificacion_interbloqueo,				//19
	fin_personaje							//20
}t_paquete;


// Tipo paquete
typedef struct paquete{
		t_paquete type;
        uint16_t payloadLength;
        char *payload;
}__attribute__((packed)) package;

#define SIZE_HEADER ( sizeof(t_paquete) + sizeof(uint16_t) )

package	*crear_paquete(t_paquete type, char *payload, uint16_t payloadLength);
int 	enviar_paquete(package *package, int socket);

package *recibir_paquete(uint32_t server_descriptor);
int32_t recibir(uint32_t descriptor, package *paquete_recibido);

void	destruir_paquete(package *package);
int32_t sockets_abrir_servidor(char ip, uint16_t port, int32_t slots);
int32_t sockets_abrir_cliente(char *ip, uint16_t port);


#endif /* NIPC_H_ */
