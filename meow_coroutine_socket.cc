#include "meow_coroutine_socket.h"

using meow::coroutine::Socket;

/**
 * Define zend class entry.
 */
zend_class_entry meow_coroutine_socket_ce;
zend_class_entry *meow_coroutine_socket_ce_ptr;

typedef struct {
    Socket *socket;
    zend_object std;
} coroutine_socket_t;

static zend_object_handlers meow_coroutine_socket_handlers;

/* 通过 PHP 对象找到 Socket 对象 */
static coroutine_socket_t *meow_coroutine_socket_fetch_obj(zend_object *obj)
{
    return (coroutine_socket_t *)((char *)obj - meow_coroutine_socket_handlers.offset);
}

/* 创建 PHP 对象 */
static zend_object *meow_coroutine_socket_create_obj(zend_class_entry *ce)
{
    coroutine_socket_t *cs = (coroutine_socket_t *) ecalloc(1, sizeof(coroutine_socket_t) + zend_object_properties_size(ce));
    /* 初始化对象 */
    zend_object_std_init(&cs->std, ce);
    /* 初始化对象的属性 */
    object_properties_init(&cs->std, ce);
    cs->std.handlers = &meow_coroutine_socket_handlers;
    return &cs->std;
}

/* 释放 PHP 对象 */
static void meow_coroutine_socket_free_obj(zend_object *obj)
{
    coroutine_socket_t *cs = meow_coroutine_socket_fetch_obj(obj);
    
    if (cs->socket && cs->socket != MEOW_BAD_SOCKET) {
        cs->socket->close();
        delete cs->socket;
    }

    zend_object_std_dtor(&cs->std);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_socket_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_socket_construct, 0, 0, 2)
    ZEND_ARG_INFO(0, domain)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_socket_bind, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_socket_listen, 0, 0, 0)
    ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_socket_recv, 0, 0, 0)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_socket_send, 0, 0, 1)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

static PHP_METHOD(meow_coroutine_socket, __construct)
{
    zend_long domain, type, protocol;
    coroutine_socket_t *cs;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_LONG(domain)
        Z_PARAM_LONG(type)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(protocol)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));
    cs->socket = new Socket(domain, type, protocol);

    zend_update_property_long(meow_coroutine_socket_ce_ptr, getThis(), ZEND_STRL("fd"), cs->socket->get_fd());
}

static PHP_METHOD(meow_coroutine_socket, bind)
{
    zval *host;
    zend_long port;
    coroutine_socket_t *cs;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_ZVAL(host)
            Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));

    if (cs->socket->bind(MEOW_SOCK_TCP, Z_STRVAL_P(host), port) < 0) {
        RETURN_FALSE
    }

    RETURN_TRUE;
}
