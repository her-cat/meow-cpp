#include "php_meow.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_create, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(meow_coroutine_create);

/* 模块初始化阶段 */
PHP_MINIT_FUNCTION(meow)
{
    meow_coroutine_util_init();
    meow_coroutine_server_init();
    return SUCCESS;
}

/* 模块关闭阶段 */
PHP_MSHUTDOWN_FUNCTION(meow)
{
    php_printf("MSHUTDOWN\n");
    return SUCCESS;
}

/* 请求初始化阶段 */
PHP_RINIT_FUNCTION(meow)
{
    php_printf("RINIT\n");
    return SUCCESS;
}

/* 请求关闭阶段 */
PHP_RSHUTDOWN_FUNCTION(meow)
{
    php_printf("RSHUTDOWN\n");
    return SUCCESS;
}

/* 模块信息 */
PHP_MINFO_FUNCTION(meow)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "meow support", "enabled");
    php_info_print_table_end();
}

const zend_function_entry meow_functions[] ={
        PHP_FE(meow_coroutine_create, arginfo_meow_coroutine_create)
        ZEND_NS_FALIAS("Meow", go, meow_coroutine_create, arginfo_meow_coroutine_create)
        PHP_FE_END
};

zend_module_entry meow_module_entry ={
        STANDARD_MODULE_HEADER,
        "meow",
        meow_functions,
        PHP_MINIT(meow),
        PHP_MSHUTDOWN(meow),
        PHP_RINIT(meow),
        PHP_RSHUTDOWN(meow),
        PHP_MINFO(meow),
        PHP_MEOW_VERSION,
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MEOW
ZEND_GET_MODULE(meow)
#endif
