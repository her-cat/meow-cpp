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

static PHP_METHOD(meow_coroutine_socket, listen)
{
    zend_long backlog = 512;
    coroutine_socket_t *cs;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(backlog)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));

    if (cs->socket->listen(backlog) < 0) {
        RETURN_FALSE
    }

    RETURN_TRUE;
}

static PHP_METHOD(meow_coroutine_socket, accept)
{
    zend_object *conn;
    Socket *conn_socket;
    coroutine_socket_t *server_cs, *conn_cs;

    server_cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));

    conn_socket = server_cs->socket->accept();
    if (!conn_socket) {
        RETURN_NULL()
    }

    conn = meow_coroutine_socket_create_obj(meow_coroutine_socket_ce_ptr);
    conn_cs = meow_coroutine_socket_fetch_obj(conn);
    conn_cs->socket = conn_socket;

    ZVAL_OBJ(return_value, &(conn_cs->std));

    zend_update_property_long(meow_coroutine_socket_ce_ptr, return_value, ZEND_STRL("fd"), conn_socket->get_fd());
}

static PHP_METHOD(meow_coroutine_socket, recv)
{
    ssize_t ret;
    zend_long length = 65536;
    coroutine_socket_t *conn_cs;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Socket::init_read_buffer();

    if (length < 0 || length > Socket::read_buffer_len) {
        length = Socket::read_buffer_len;
    }

    conn_cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));

    ret = conn_cs->socket->recv(Socket::read_buffer, length);
    if (ret == 0) {
        zend_update_property_long(meow_coroutine_socket_ce_ptr, getThis(), ZEND_STRL("errCode"), MEOW_ERROR_SESSION_CLOSED_BY_CLIENT);
        zend_update_property_string(meow_coroutine_socket_ce_ptr, getThis(), ZEND_STRL("errMsg"), meow_strerror(MEOW_ERROR_SESSION_CLOSED_BY_CLIENT));
        RETURN_FALSE
    } else if (ret < 0) {
        php_error_docref(NULL, E_WARNING, "recv error");
        RETURN_FALSE
    }

    Socket::read_buffer[ret] = '\0';
    RETURN_STRING(Socket::read_buffer);
}

static PHP_METHOD(meow_coroutine_socket, send)
{
    char *data;
    ssize_t ret;
    size_t length;
    coroutine_socket_t *conn_cs;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(data, length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    conn_cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));

    ret = conn_cs->socket->send(data, length);
    if (ret < 0) {
        php_error_docref(NULL, E_WARNING, "send error");
        RETURN_FALSE
    }

    RETURN_LONG(ret)
}

static PHP_METHOD(meow_coroutine_socket, close)
{
    coroutine_socket_t *conn_cs;

    conn_cs = meow_coroutine_socket_fetch_obj(Z_OBJ_P(getThis()));

    if (conn_cs->socket->close() < 0) {
        php_error_docref(NULL, E_WARNING, "close error");
        RETURN_FALSE
    }

    delete conn_cs->socket;
    conn_cs->socket = MEOW_BAD_SOCKET;

    RETURN_TRUE
}

static const zend_function_entry meow_coroutine_socket_methods[] =
{
    PHP_ME(meow_coroutine_socket, __construct, arginfo_meow_coroutine_socket_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(meow_coroutine_socket, bind, arginfo_meow_coroutine_socket_bind, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_socket, listen, arginfo_meow_coroutine_socket_listen, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_socket, accept, arginfo_meow_coroutine_socket_void, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_socket, recv, arginfo_meow_coroutine_socket_recv, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_socket, send, arginfo_meow_coroutine_socket_send, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_socket, close, arginfo_meow_coroutine_socket_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void meow_coroutine_socket_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(meow_coroutine_socket_ce, "Meow", "Coroutine\\Socket", meow_coroutine_socket_methods);
    memcpy(&meow_coroutine_socket_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    meow_coroutine_socket_ce_ptr = zend_register_internal_class(&meow_coroutine_socket_ce TSRMLS_CC);
    MEOW_SET_CLASS_OBJ(meow_coroutine_socket, meow_coroutine_socket_create_obj, meow_coroutine_socket_free_obj, coroutine_socket_t, std);

    zend_declare_property_long(meow_coroutine_socket_ce_ptr, ZEND_STRL("fd"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_socket_ce_ptr, ZEND_STRL("errCode"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_string(meow_coroutine_socket_ce_ptr, ZEND_STRL("errMsg"), "", ZEND_ACC_PUBLIC);
}
