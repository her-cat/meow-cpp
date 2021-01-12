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

#endif	/* PHP_MEOW_H */
