#ifndef MEOW_SOCKET_H
#define MEOW_SOCKET_H

#include "meow.h"
#include "log.h"

#define DEFAULT_LISTEN_BACKLOG 512

enum meow_sock_type {
    MEOW_SOCK_TCP = 1,
    MEOW_SOCK_UDP = 2
};

int meow_socket_create(int domain, int type, int protocol);
int meow_socket_bind(int sock, int type, char *host, int port);
int meow_socket_listen(int sock, int backlog);
int meow_socket_accept(int sock);
ssize_t meow_socket_recv(int sock, void *buf, size_t len, int flag);
ssize_t meow_socket_send(int sock, const void *buf, size_t len, int flag);
int meow_socket_close(int fd);
int meow_socket_set_nonblock(int sock);

#endif /* MEOW_SOCKET_H */
