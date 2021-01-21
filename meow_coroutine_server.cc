#include "meow_coroutine_server.h"

using meow::coroutine::Socket;

/**
 * Define zend class entry.
 */
zend_class_entry meow_coroutine_sever_ce;
zend_class_entry *meow_coroutine_sever_ce_ptr;

/* 定义无参参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_void, 0, 0, 0)
ZEND_END_ARG_INFO()

/* 定义 Coroutine\Server::__construct 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_construct, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

/* 定义 Coroutine\Server::recv 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_recv, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

/* 定义 Coroutine\Server::send 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_server_send, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

/* 构造函数 */
PHP_METHOD(meow_coroutine_server, __construct)
{
    zval *host;
    zend_long port;
    zval zsocket;
    Socket *socket;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(host)
        Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    socket = new Socket(AF_INET, SOCK_STREAM, 0);
    socket->bind(MEOW_SOCK_TCP, Z_STRVAL_P(host), port);
    socket->listen();

    ZVAL_PTR(&zsocket, socket);

    /* 更新 Coroutine\Server 属性 */
    zend_update_property(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("zsocket"), &zsocket);
    zend_update_property_string(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("host"), Z_STRVAL_P(host));
    zend_update_property_long(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("port"), port);
}

/* 接受新连接 */
PHP_METHOD(meow_coroutine_server, accept)
{
    zval *sock;
    int conn_fd;

    sock = meow_zend_read_property(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("sock"), 0);
    conn_fd = meow_socket_accept(Z_LVAL_P(sock));

    RETURN_LONG(conn_fd)
}

/* 接收数据 */
PHP_METHOD(meow_coroutine_server, recv)
{
    int ret;
    zend_long fd;
    zend_long length = 65536;
    zend_string *buf;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_LONG(fd)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    buf = zend_string_alloc(length, 0);

    ret = meow_socket_recv(fd, ZSTR_VAL(buf), length, 0);
    if (ret == 0) {
        zend_update_property_long(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("errCode"), MEOW_ERROR_SESSION_CLOSED_BY_CLIENT);
        zend_update_property_string(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("errMsg"), meow_strerror(MEOW_ERROR_SESSION_CLOSED_BY_CLIENT));
        RETURN_FALSE
    }

    if (ret < 0) {
        php_error_docref(NULL, E_WARNING, "recv error");
        RETURN_FALSE
    }

    ZSTR_VAL(buf)[ret] = '\0';

    RETURN_STR(buf)
}

/* 发送数据 */
PHP_METHOD(meow_coroutine_server, send)
{
    ssize_t ret;
    zend_long fd;
    char *data;
    size_t length;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(fd)
        Z_PARAM_STRING(data, length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    ret = meow_socket_send(fd, data, length, 0);
    if (ret < 0) {
        php_error_docref(NULL, E_WARNING, "send error");
        RETURN_FALSE
    }

    RETURN_LONG(ret)
}

/* Coroutine\Server 的方法列表 */
static const zend_function_entry meow_coroutine_server_methods[] =
{
    PHP_ME(meow_coroutine_server, __construct, arginfo_meow_coroutine_server_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(meow_coroutine_server, accept, arginfo_meow_coroutine_server_void, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_server, recv, arginfo_meow_coroutine_server_recv, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_server, send, arginfo_meow_coroutine_server_send, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void meow_coroutine_server_init()
{
    /* 初始化 Coroutine\Server */
    INIT_NS_CLASS_ENTRY(meow_coroutine_sever_ce, "Meow", "Coroutine\\Server", meow_coroutine_server_methods)
    /* 在 Zend 引擎中注册 Coroutine\Server */
    meow_coroutine_sever_ce_ptr = zend_register_internal_class(&meow_coroutine_sever_ce TSRMLS_CC);
    /* 声明 Coroutine\Server 属性 */
    zval *zsocket = (zval *) malloc(sizeof(zval));
    zend_declare_property(meow_coroutine_sever_ce_ptr, ZEND_STRL("zsocket"), zsocket, ZEND_ACC_PUBLIC);
    zend_declare_property_string(meow_coroutine_sever_ce_ptr, ZEND_STRL("host"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_sever_ce_ptr, ZEND_STRL("port"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_sever_ce_ptr, ZEND_STRL("errCode"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_string(meow_coroutine_sever_ce_ptr, ZEND_STRL("errMsg"), "", ZEND_ACC_PUBLIC);
}