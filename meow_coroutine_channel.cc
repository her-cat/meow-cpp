#include "meow_coroutine_channel.h"

/**
 * Define zend class entry
 */
zend_class_entry meow_coroutine_channel_ce;
zend_class_entry *meow_coroutine_channel_ce_ptr;

/* 定义 Meow\Coroutine\Channel::__construct() 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_channel_construct, 0, 0, 0)
    ZEND_ARG_INFO(0, capacity)
ZEND_END_ARG_INFO()

/* 构造函数 */
static PHP_METHOD(meow_coroutine_channel, __construct)
{
    zend_long capacity = 1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(capacity)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (capacity <= 0) {
        capacity = 1;
    }

    zend_update_property_long(meow_coroutine_channel_ce_ptr, getThis(), ZEND_STRL("capacity"), capacity);
}

/* Coroutine\Channel 的方法列表 */
static const zend_function_entry meow_coroutine_channel_methods[] =
{
    PHP_ME(meow_coroutine_channel, __construct, arginfo_meow_coroutine_channel_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};

void meow_coroutine_channel_init()
{
    /* 初始化 Coroutine\Channel */
    INIT_NS_CLASS_ENTRY(meow_coroutine_channel_ce, "Meow", "Coroutine\\Channel", meow_coroutine_channel_methods);
    /* 在 Zend 引擎中注册 Coroutine\Channel */
    meow_coroutine_channel_ce_ptr = zend_register_internal_class(&meow_coroutine_channel_ce TSRMLS_CC);

    zend_declare_property_long(meow_coroutine_channel_ce_ptr, ZEND_STRL("capacity"), 1, ZEND_ACC_PUBLIC);
}
