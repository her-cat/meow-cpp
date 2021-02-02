#ifndef PHP_MEOW_COROUTINE_H
#define PHP_MEOW_COROUTINE_H

#include <stack>
#include "php_meow.h"
#include "coroutine.h"

#define TIMER_MINIMUM_TIME 0.001
#define DEFAULT_PHP_STACK_PAGE_SIZE 8192
#define php_coroutine_task_t_SLOT ((int)((ZEND_MM_ALIGNED_SIZE(sizeof(php_coroutine_task_t)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) / ZEND_MM_ALIGNED_SIZE(sizeof(zval))))

typedef struct php_coroutine_arg_s php_coroutine_arg_t;
typedef struct php_function_s php_function_t;
typedef struct php_coroutine_task_s php_coroutine_task_t;

/* 协程参数结构体 */
struct php_coroutine_arg_s
{
    zend_fcall_info_cache *fci_cache;
    zval *argv;
    uint32_t argc;
};

/* PHP 函数结构体 */
struct php_function_s
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
};

/* 协程堆栈信息 */
struct php_coroutine_task_s
{
    zval *vm_stack_top; /* 协程栈栈顶 */
    zval *vm_stack_end; /* 协程栈栈底 */
    zend_vm_stack vm_stack; /* 协程栈指针 */
    size_t vm_stack_page_size; /* 协程栈页大小 */
    zend_execute_data *execute_data; /* 当前协程栈的栈帧 */
    meow::Coroutine *coroutine;
    std::stack<php_function_t *> *defer_tasks; /* 记录协程中需要延缓执行的函数 */
};

namespace meow
{
class PHPCoroutine
{
public:
    static void init();
    static long create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv);
    static void defer(php_function_t *func);
    static int sleep(double seconds);

    static inline php_coroutine_task_t *get_origin_task(php_coroutine_task_t *task)
    {
        Coroutine *coroutine = task->coroutine->get_origin();
        return coroutine ? (php_coroutine_task_t *) coroutine->get_task() : &main_task;
    }

protected:
    static php_coroutine_task_t main_task;

    static void save_task(php_coroutine_task_t *task);
    static void save_vm_stack(php_coroutine_task_t *task);
    static php_coroutine_task_t *get_task(void);
    static void create_func(void *arg);
    static void vm_stack_init(void);
    static void run_defer_tasks(php_coroutine_task_t *task);
    static void on_yield(void *arg);
    static void on_resume(void *arg);
    static inline void restore_task(php_coroutine_task_t *task);
    static inline void restore_vm_stack(php_coroutine_task_t *task);
};
}

#endif // PHP_MEOW_COROUTINE_H
