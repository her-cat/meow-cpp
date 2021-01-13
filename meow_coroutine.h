#ifndef PHP_MEOW_COROUTINE_H
#define PHP_MEOW_COROUTINE_H

#include "php_meow.h"

namespace Meow
{
class PHPCoroutine
{
    static long create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv);
};
}

#endif
