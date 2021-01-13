#include "meow_coroutine.h"

using Meow::PHPCoroutine;

/* 定义 Coroutine::create 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_create, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

/* 创建协程 */
static PHP_METHOD(meow_coroutine_util, create)
{
    zval result;
    /* 用来存放 create 传递的函数 */
    zend_fcall_info fci = empty_fcall_info;
    zend_fcall_info_cache fcc = empty_fcall_info_cache;

    /* -1 表示没有最大参数个数限制 */
    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_FUNC(fci, fcc)
        /* 解析可变参数 */
        Z_PARAM_VARIADIC("*", fci.params, fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    fci.retval = &result;
    if (zend_call_function(&fci, &fcc) != SUCCESS) {
        return;
    }

    PHPCoroutine::create(&fcc, fci.param_count, fci.params);

    *return_value = result;
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
