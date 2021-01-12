#include "php_meow.h"

/* 模块初始化阶段 */
PHP_MINIT_FUNCTION(meow)
{
    php_printf("MINIT\n");
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
