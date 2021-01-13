#ifndef PHP_MEOW_COROUTINE_H
#define PHP_MEOW_COROUTINE_H

#include "php_meow.h"
#include "coroutine.h"

#define DEFAULT_PHP_STACK_PAGE_SIZE 8192
#define PHP_COROUTINE_TASK_SLOT ((int)((ZEND_MM_ALIGNED_SIZE(sizeof(php_coroutine_task)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) / ZEND_MM_ALIGNED_SIZE(sizeof(zval))))

/* 协程参数结构体 */
struct php_coroutine_args
{
    zend_fcall_info_cache *fci_cache;
    zval *argv;
    uint32_t argc;
};

/* 协程堆栈信息 */
struct php_coroutine_task
{
    zval *vm_stack_top; /* 协程栈栈顶 */
    zval *vm_stack_end; /* 协程栈栈底 */
    zend_vm_stack vm_stack; /* 协程栈指针 */
    size_t vm_stack_page_size; /* 协程栈页大小 */
    zend_execute_data *execute_data; /* 当前协程栈的栈帧 */
    meow::Coroutine *coroutine;
};

namespace meow
{
class PHPCoroutine
{
public:
    static long create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv);

protected:
    static php_coroutine_task main_task;

    static void save_task(php_coroutine_task *task);
    static void save_vm_stack(php_coroutine_task *task);
    static php_coroutine_task *get_task(void);
    static void create_func(void *arg);
    static void vm_stack_init(void);
};
}

#endif // PHP_MEOW_COROUTINE_H
