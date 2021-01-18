#ifndef MEOW_SOCKET_H
#define MEOW_SOCKET_H

#include "meow.h"

enum meow_sock_type {
    MEOW_SOCK_TCP = 1,
    MEOW_SOCK_UDP = 2
};

int meow_socket_create(int type);
int meow_socket_bind(int sock, int type, char *host, int port);
int meow_socket_accept(int sock);

#endif /* MEOW_SOCKET_H */
