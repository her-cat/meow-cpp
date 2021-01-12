/* meow extension for PHP */

#ifndef PHP_MEOW_H
# define PHP_MEOW_H

extern zend_module_entry meow_module_entry;
# define phpext_meow_ptr &meow_module_entry

# define PHP_MEOW_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_MEOW)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_MEOW_H */

