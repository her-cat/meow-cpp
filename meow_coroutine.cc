#include "meow_coroutine.h"
#include "coroutine.h"

using Meow::PHPCoroutine;
using Meow::Coroutine;

php_coroutine_task PHPCoroutine::main_task = {0};

/* 创建 PHP 协程 */
long PHPCoroutine::create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv)
{
    php_coroutine_args args;

    args.fci_cache = fci_cache;
    args.argv = argv;
    args.argc = argc;

    save_task(get_task());

    return 0;
}

/* 保存 PHP 协程堆栈信息 */
void PHPCoroutine::save_task(php_coroutine_task *task)
{
    save_vm_stack(task);
}

/* 保存虚拟机堆栈信息 */
void PHPCoroutine::save_vm_stack(php_coroutine_task *task)
{
    task->vm_stack_top = EG(vm_stack_top);
    task->vm_stack_end = EG(vm_stack_end);
    task->vm_stack = EG(vm_stack);
    task->vm_stack_page_size = EG(vm_stack_page_size);
    task->execute_data = EG(current_execute_data);
}

/* 获取当前协程堆栈信息 */
php_coroutine_task *PHPCoroutine::get_task()
{
    php_coroutine_task *task = (php_coroutine_task *) Coroutine::get_current_task();
    return task ? task : &main_task;
}
