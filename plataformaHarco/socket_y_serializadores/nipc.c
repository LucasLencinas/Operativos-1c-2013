#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "nipc.h"



package *crear_paquete(t_paquete type, char *payload, uint16_t payloadLength) {
	package* paquete = malloc( sizeof(package));
	paquete->type = type;
	paquete->payloadLength = payloadLength;

	if (payloadLength > 0)
		paquete->payload = malloc(payloadLength);
	else
		paquete->payload = NULL;

	memcpy(paquete->payload, payload, payloadLength);
	return paquete;
}



char *serializar_paquete(package *paquete, size_t *size_stream) {
	int size = 0, offset = 0;
	*size_stream = 0;
	char *stream = NULL;

	*size_stream = (SIZE_HEADER + (paquete->payloadLength) * sizeof(char));
	stream = malloc(*size_stream);
	if( stream ){
		memset(stream, '0', *size_stream);

		size = sizeof(t_paquete);
		memcpy(stream, &paquete->type, size);

		offset += size;

		size = sizeof(uint16_t);
		memcpy(stream + offset, &paquete->payloadLength, size); //Invalid read of size 1

		offset += size;

		size = paquete->payloadLength;

		memcpy(stream+offset , paquete->payload, size);		//Invalid read of size 4

		*size_stream = offset + size;
	}
	return stream;
}

package *deserializar_header(char *stream) {
	int size = 0, offset = 0;

	package *package_tmp = malloc(sizeof(package));

	if(package_tmp){

		memset(package_tmp, 0, sizeof(package));
		size = sizeof(t_paquete);

		memcpy(&package_tmp->type, stream, size);
		offset += size;

		size = sizeof(uint16_t);
		memcpy(&package_tmp->payloadLength, stream + offset, size);
	}
	return package_tmp;
}


void deserializar_body(package *package, char *stream) {
	uint16_t size_payload = package->payloadLength;

	if( size_payload ){
		package->payload = malloc( size_payload );

		memcpy(package->payload, stream, size_payload);
	}

}

//Envio de paquetes

int enviar_paquete(package *package, int socket) {
	size_t size_stream = 0;
	char *stream = NULL;
	//int resultado = EXIT_FAILURE;
	int32_t bytesEnviados = EXIT_FAILURE;

	if(package){
		stream = serializar_paquete(package, &size_stream);

		if( socket )
			bytesEnviados= send(socket, stream, size_stream, 0);
		free(stream);
	}
	return bytesEnviados;
}


//Recibir paquete

package *recibir_paquete(uint32_t server_descriptor) {

	package *paquete = malloc(sizeof(package));
	int32_t bytes_recibidos;
	bytes_recibidos= recibir(server_descriptor, paquete);
	if(bytes_recibidos==0){
		paquete->payloadLength=0;
	}
	return paquete;
	}

int32_t recibir(uint32_t descriptor, package* paquete) {	//FIXME ya la revise, parece que esta re piola!!!
	int32_t bytes_recibidos;
	package *paquete_recibido = paquete;
	char stream[1024];		//FIXME me tiro aca el error

	bytes_recibidos = recv(descriptor, stream, 1024, 0);


	if (bytes_recibidos < 0) {
		//free(stream);
		perror("Error en recv()");
		return -1;
	}
	if (bytes_recibidos == 0) {
	    printf("\n");
		/*	free(stream);
		perror("Cerro"); */
		return 0;
	}

	paquete_recibido  = (package*) deserializar_header(stream);
	char* streamPayload= stream + sizeof(t_paquete)+sizeof(uint16_t);
	deserializar_body(paquete_recibido, streamPayload);

	paquete->type=paquete_recibido->type;
	paquete->payloadLength=paquete_recibido->payloadLength;
	paquete->payload=paquete_recibido->payload;
	free(paquete_recibido);		//NO HAGO DESTROY PORQUE EL PAYLOAD TODAVIA SIRVE!
	//free(stream);
	//free(streamPayload);		// no hago free de esto porque es una parte del stream de arriba

	return 1;
}



int32_t sockets_abrir_servidor(char ip, uint16_t port, int32_t slots)
{
	struct sockaddr_in dir;
	int32_t descriptor;
	int32_t yes = 1;

	dir.sin_family = AF_INET;
	dir.sin_addr.s_addr = ip;
	dir.sin_port = htons(port);
	memset(dir.sin_zero, 0, sizeof(dir.sin_zero));

	if(-1 == (descriptor = socket(AF_INET, SOCK_STREAM, 0)))
		return -1;

	if(-1 == setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))
	{
		close(descriptor);
		return -1;
	}

	if(-1 == bind(descriptor, (struct sockaddr*)&dir, sizeof(dir)))
	{
	   close(descriptor);
	   return -1;
	}

	if(-1 == listen(descriptor, slots))
	{
		close(descriptor);
		return -1;
	}

	return descriptor;
}

int32_t sockets_abrir_cliente(char *ip, uint16_t port)
{
	struct sockaddr_in  dir;
	int32_t             descriptor;

	dir.sin_family = AF_INET;
	dir.sin_addr.s_addr = inet_addr(ip);
	dir.sin_port = htons(port);
	memset(dir.sin_zero, 0, sizeof(dir.sin_zero));

	if(-1  == (descriptor = socket(AF_INET, SOCK_STREAM, 0)))
	{
		return -1;
	}

	if(-1 == connect(descriptor, (struct sockaddr*)&dir, sizeof(dir)))
	{
		perror("conectando");
		close(descriptor);
		return -1;
	}

	return descriptor;
}

//Destruir paquete

void destruir_paquete(package *paquete) {
	if ((paquete!=NULL)&(paquete->payloadLength!=0)) {
		if (paquete->payload !=NULL)
			free(paquete->payload);
		free(paquete);
	}
}



