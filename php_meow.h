/* meow extension for PHP */

#ifndef PHP_MEOW_H
# define PHP_MEOW_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "meow.h"

#define PHP_MEOW_VERSION "0.1.0"

extern zend_module_entry meow_module_entry;
# define phpext_meow_ptr &meow_module_entry

#ifdef PHP_WIN32
#    define PHP_MEOW_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_MEOW_API __attribute__ ((visibility("default")))
#else
#    define PHP_MEOW_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define MEOW_SET_CLASS_CREATE(module, function) \
    module##_ce_ptr->create_object = function

#define MEOW_SET_CLASS_FREE(module, function) \
    module##_handlers.free_obj = function

#define MEOW_SET_CLASS_CREATE_AND_FREE(module, create, free) \
    MEOW_SET_CLASS_CREATE(module, create); \
    MEOW_SET_CLASS_FREE(module, free)

/* module##_handlers.offset 保存 PHP 对象在自定义对象中的偏移量 */
#define MEOW_SET_CLASS_OBJ(module, create, free, struct_t, field) \
    MEOW_SET_CLASS_CREATE_AND_FREE(module, create, free); \
    module##_handlers.offset = XtOffsetOf(struct_t, field)

/**s
 * Declare any global variables you may need between the BEGIN and END macros here
 */
ZEND_BEGIN_MODULE_GLOBALS(meow)

ZEND_END_MODULE_GLOBALS(meow)

void meow_coroutine_util_init();
void meow_coroutine_server_init(int module_number);
void meow_coroutine_channel_init();
void meow_coroutine_socket_init(int module_number);
void meow_runtime_init();

/* 读取对象的某个属性 */
inline zval *meow_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *name, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, name, len, silent, &rv);
}

inline zval *meow_malloc_zval()
{
    return (zval *) emalloc(sizeof(zval));
}

inline zval *meow_zval_dup(zval *val)
{
    zval *dup = meow_malloc_zval();
    memcpy(dup, val, sizeof(zval));
    return dup;
}

#endif	/* PHP_MEOW_H */
