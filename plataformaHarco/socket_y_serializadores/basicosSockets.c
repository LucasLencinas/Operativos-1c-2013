/*
 * basicosSockets.c
 *
 *  Created on: 05/05/2013
 *      Author: utnso
 */

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#include "basicosSockets.h"

#define DIRECCION "10.15.177.18"   //despues cambiar
#define PUERTO 5230
#define BUFF_SIZE 1024

int crear_socket(){
int miSocket;
	if ((miSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	return miSocket;
}



int bindear_socket(int descriptor,int port){
	int salida=0;
	struct sockaddr_in miAddr;

	miAddr.sin_family = AF_INET;
	miAddr.sin_port = htons(port);
	miAddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(miAddr.sin_zero), '\0', 8);

	if (bind(descriptor, (struct sockaddr *)&miAddr, sizeof(struct sockaddr))== -1) {
			perror("bind");
			exit(1);
	}
	return salida;
}


int escuchar_llamadas(int descriptor){
	int salida=0;
	if (listen(descriptor, 10) == -1) {
		perror("listen");
		exit(1);
	}
	return salida;
}


int aceptar_clientes (int descriptor){

	struct sockaddr_in suAddr;
	int infd;
	unsigned int sin_size = sizeof(struct sockaddr_in);

	if ((infd = accept(descriptor, (struct sockaddr *)&suAddr, &sin_size)) == -1) {
		perror("accept");
	}
	return infd;
}


int conectar_socket(int descriptor,char* ip, int port){
	int salida = 0;
	struct sockaddr_in suAddr;

	suAddr.sin_family = AF_INET;
	suAddr.sin_addr.s_addr = inet_addr(ip);
	suAddr.sin_port = htons(port);

	if (connect(descriptor, (struct sockaddr *)&suAddr,sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
	return salida;
}

struct sockaddr_in tomar_datos_conexion (int descriptor){
	struct sockaddr_in suAddr;
	socklen_t addrlen = sizeof(suAddr);

	if ((getpeername( descriptor, (struct sockaddr *)&suAddr, &addrlen))==-1){
		printf("error del getpeername\n");
		}

	return suAddr;
}




