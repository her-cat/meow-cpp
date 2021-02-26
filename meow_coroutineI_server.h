#ifndef PHP_MEOW_COROUTINE_SERVER_H
#define PHP_MEOW_COROUTINE_SERVER_H

#include "error.h"
#include "socket.h"
#include "php_meow.h"
#include "meow_coroutine.h"
#include "coroutine_socket.h"

namespace meow
{
    namespace phpcoroutine
    {
        class Server
        {
        private:
            meow::coroutine::Socket *socket = nullptr;
            php_function_t *handler = nullptr;
            bool running = false;
        public:
            Server(char *host, int port);
            ~Server();
            bool start();
            bool shutdown();
            void set_handler(php_function_t *handler);
            php_function_t *get_handler();
        };
    }
}

#endif
