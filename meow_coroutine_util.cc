#include "meow_coroutine.h"

/* 定义 Coroutine::create 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_create, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

static PHP_METHOD(meow_coroutine_util, create)
{
    php_printf("success\n");
}

/* 定义 Coroutine 的方法列表 */
const zend_function_entry meow_coroutine_util_methods[] =
{
    PHP_ME(meow_coroutine_util, create, arginfo_meow_coroutine_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

zend_class_entry meow_coroutine_ce;
zend_class_entry *meow_coroutine_ce_ptr;

void meow_coroutine_util_init()
{
    /* 初始化 Coroutine */
    INIT_NS_CLASS_ENTRY(meow_coroutine_ce, "Meow", "Coroutine", meow_coroutine_util_methods);
    /* 在 Zend 引擎中注册 Coroutine */
    meow_coroutine_ce_ptr = zend_register_internal_class(&meow_coroutine_ce TSRMLS_CC);
}
