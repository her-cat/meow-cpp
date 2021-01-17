#include "meow_coroutine_server.h"

/**
 * Define zend class entry.
 */
zend_class_entry meow_coroutine_sever_ce;
zend_class_entry *meow_coroutine_sever_ce_ptr;

void meow_coroutine_server_init()
{
    /* 初始化 Coroutine\Server */
    INIT_NS_CLASS_ENTRY(meow_coroutine_sever_ce, "Meow", "Coroutine\\Server", NULL)
    /* 在 Zend 引擎中注册 Coroutine\Server */
    meow_coroutine_sever_ce_ptr = zend_register_internal_class(&meow_coroutine_sever_ce TSRMLS_CC);
}