/*
 * basicosSockets.h
 *
 *  Created on: 05/05/2013
 *      Author: utnso
 */

#ifndef BASICOSSOCKETS_H_
#define BASICOSSOCKETS_H_

int crear_socket();

int bindear_socket(int descriptor,int port);

int escuchar_llamadas(int descriptor);

int aceptar_clientes (int descriptor);

int conectar_socket(int descriptor,char* ip, int port);

struct sockaddr_in tomar_datos_conexion (int descriptor);

#endif /* BASICOSSOCKETS_H_ */
