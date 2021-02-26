#include "meow_coroutine_socket.h"
#include "meow_coroutineI_server.h"

using meow::PHPCoroutine;
using meow::coroutine::Socket;
using meow::phpcoroutine::Server;

Server::Server(char *host, int port)
{
    socket = new Socket(AF_INET, SOCK_STREAM, 0);

    if(socket->bind(MEOW_SOCK_TCP, host, port) < 0) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno))
        return;
    }

    socket->listen(DEFAULT_LISTEN_BACKLOG);
}

Server::~Server()
{

}

bool Server::start()
{
    zval zsocket;
    running = true;

    while (running) {
        Socket *conn = socket->accept();
        if(!conn) {
            return false;
        }

        php_meow_init_socket_obj(&zsocket, conn);
        PHPCoroutine::create(&(handler->fcc), 1, &zsocket);
        zval_dtor(&zsocket);
    }

    return true;
}

bool Server::shutdown()
{
    running = false;
    return true;
}

void Server::set_handler(php_function_t *_handler)
{
    handler = _handler;
}

php_function_t * Server::get_handler()
{
    return handler;
}

/**
 * Define zend class entry
 */
zend_class_entry meow_coroutine_server_ce;
zend_class_entry *meow_coroutine_server_ce_ptr;

static zend_object_handlers meow_coroutine_server_handlers;

typedef struct {
    Server *server;
    zend_object std;
} coroutine_server_t;

static coroutine_server_t *meow_coroutine_server_fetch_obj(zend_object *obj)
{
    return (coroutine_server_t *)((char *)obj - meow_coroutine_server_handlers.offset);
}

static zend_object *meow_coroutine_server_create_obj(zend_class_entry *ce)
{
    coroutine_server_t *cs = (coroutine_server_t *) ecalloc(1, sizeof(coroutine_server_t) + zend_object_properties_size(ce));
    /* 初始化对象 */
    zend_object_std_init(&cs->std, ce);
    /* 初始化对象的属性 */
    object_properties_init(&cs->std, ce);
    cs->std.handlers = &meow_coroutine_server_handlers;
    return &cs->std;
}

static void meow_coroutine_server_free_obj(zend_object *obj)
{
    coroutine_server_t *cs = meow_coroutine_server_fetch_obj(obj);

    if (cs->server) {
        php_function_t *handler = cs->server->get_handler();
        if (handler) {
            efree(handler);
            cs->server->set_handler(nullptr);
        }
        delete cs->server;
    }

    zend_object_std_dtor(&cs->std);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_construct, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_set_handler, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func,  0)
ZEND_END_ARG_INFO()

PHP_METHOD(meow_coroutine_server, __construct)
{
    zend_long port;
    coroutine_server_t *cs;
    zval *zhost;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_ZVAL(zhost)
            Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cs = (coroutine_server_t *) meow_coroutine_server_fetch_obj(Z_OBJ_P(getThis()));
    cs->server = new Server(Z_STRVAL_P(zhost), port);

    zend_update_property_string(meow_coroutine_server_ce_ptr, getThis(), ZEND_STRL("host"), Z_STRVAL_P(zhost));
    zend_update_property_long(meow_coroutine_server_ce_ptr, getThis(), ZEND_STRL("port"), port);
}

PHP_METHOD(meow_coroutine_server, start)
{
    coroutine_server_t *cs;

    cs = (coroutine_server_t *) meow_coroutine_server_fetch_obj(Z_OBJ_P(getThis()));
    if (!cs->server->start()) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

PHP_METHOD(meow_coroutine_server, shutdown)
{
    coroutine_server_t *cs;

    cs = (coroutine_server_t *) meow_coroutine_server_fetch_obj(Z_OBJ_P(getThis()));
    if (!cs->server->shutdown()) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

PHP_METHOD(meow_coroutine_server, set_handler)
{
    coroutine_server_t *cs;
    php_function_t *handler;

    handler = (php_function_t *)emalloc(sizeof(php_function_t));

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_FUNC(handler->fci, handler->fcc)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cs = (coroutine_server_t *)meow_coroutine_server_fetch_obj(Z_OBJ_P(getThis()));
    cs->server->set_handler(handler);
}

static const zend_function_entry meow_coroutine_server_coroutine_methods[] =
{
    PHP_ME(meow_coroutine_server, __construct, arginfo_meow_coroutine_server_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(meow_coroutine_server, start, arginfo_meow_coroutine_server_void, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_server, shutdown, arginfo_meow_coroutine_server_void, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_server, set_handler, arginfo_meow_coroutine_server_set_handler, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void meow_coroutine_server_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(meow_coroutine_server_ce, "Meow", "Coroutine\\Server", meow_coroutine_server_coroutine_methods);

    memcpy(&meow_coroutine_server_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    meow_coroutine_server_ce_ptr = zend_register_internal_class(&meow_coroutine_server_ce TSRMLS_CC);
    MEOW_SET_CLASS_OBJ(meow_coroutine_server, meow_coroutine_server_create_obj, meow_coroutine_server_free_obj, coroutine_server_t , std);

    zend_declare_property_string(meow_coroutine_server_ce_ptr, ZEND_STRL("host"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_server_ce_ptr, ZEND_STRL("port"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_server_ce_ptr, ZEND_STRL("errCode"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_string(meow_coroutine_server_ce_ptr, ZEND_STRL("errMsg"), "", ZEND_ACC_PUBLIC);
}
