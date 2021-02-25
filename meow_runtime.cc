#include "meow_runtime.h"

/**
 * Define zend class entry
 */
zend_class_entry meow_runtime_ce;
zend_class_entry *meow_runtime_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_runtime_void, 0, 0, 0)
ZEND_END_ARG_INFO()

extern PHP_METHOD(meow_coroutine_util, sleep);
static void hook_func(const char *name, size_t name_len, zif_handler handler);

static PHP_METHOD(meow_runtime, enableCoroutine)
{
    hook_func(ZEND_STRL("sleep"), zim_meow_coroutine_util_sleep);
}

static void hook_func(const char *name, size_t name_len, zif_handler handler)
{
    zend_function *origin_func =  (zend_function *) zend_hash_str_find_ptr(EG(function_table), name, name_len);
    origin_func->internal_function.handler = handler;
}

static const zend_function_entry meow_runtime_methods[] =
{
    PHP_ME(meow_runtime, enableCoroutine, arginfo_meow_runtime_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

void meow_runtime_init()
{
    INIT_NS_CLASS_ENTRY(meow_runtime_ce, "Meow", "Runtime", meow_runtime_methods);
    meow_runtime_ce_ptr = zend_register_internal_class(&meow_runtime_ce TSRMLS_CC);
}
