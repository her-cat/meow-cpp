#ifndef PHP_MEOW_COROUTINE_SOCKET_H
#define PHP_MEOW_COROUTINE_SOCKET_H

#include "php_meow.h"
#include "socket.h"
#include "coroutine_socket.h"
#include "error.h"

void php_meow_init_socket_obj(zval *zsocket, meow::coroutine::Socket *socket);

#endif /* PHP_MEOW_COROUTINE_SOCKET_H */
