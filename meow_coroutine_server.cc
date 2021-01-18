#include "meow_coroutine_server.h"

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

/* 构造函数 */
PHP_METHOD(meow_coroutine_server, __construct)
{
    int sock;
    zval *host;
    zend_long port;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(host)
        Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    sock = meow_socket_create(MEOW_SOCK_TCP);
    meow_socket_bind(sock, MEOW_SOCK_TCP, Z_STRVAL_P(host), port);
    meow_socket_listen(sock);

    /* 更新 Coroutine\Server 属性 */
    zend_update_property_long(meow_coroutine_sever_ce_ptr, getThis(), ZEND_STRL("sock"), sock);
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

/* Coroutine\Server 的方法列表 */
static const zend_function_entry meow_coroutine_server_methods[] =
{
    PHP_ME(meow_coroutine_server, __construct, arginfo_meow_coroutine_server_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(meow_coroutine_server, accept, arginfo_meow_coroutine_server_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void meow_coroutine_server_init()
{
    /* 初始化 Coroutine\Server */
    INIT_NS_CLASS_ENTRY(meow_coroutine_sever_ce, "Meow", "Coroutine\\Server", meow_coroutine_server_methods)
    /* 在 Zend 引擎中注册 Coroutine\Server */
    meow_coroutine_sever_ce_ptr = zend_register_internal_class(&meow_coroutine_sever_ce TSRMLS_CC);
    /* 声明 Coroutine\Server 属性 */
    zend_declare_property_long(meow_coroutine_sever_ce_ptr, ZEND_STRL("sock"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_string(meow_coroutine_sever_ce_ptr, ZEND_STRL("host"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_sever_ce_ptr, ZEND_STRL("port"), -1, ZEND_ACC_PUBLIC);
}