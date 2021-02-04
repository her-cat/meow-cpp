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

/**
 * Declare any global variables you may need between the BEGIN and END macros here
 */
ZEND_BEGIN_MODULE_GLOBALS(meow)

ZEND_END_MODULE_GLOBALS(meow)

void meow_coroutine_util_init();
void meow_coroutine_server_init();
void meow_coroutine_channel_init();

/* 读取对象的某个属性 */
inline zval *meow_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *name, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, name, len, silent, &rv);
}

#endif	/* PHP_MEOW_H */
