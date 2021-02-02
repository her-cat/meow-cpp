#include "meow_coroutine.h"
#include "coroutine.h"

using meow::PHPCoroutine;
using meow::Coroutine;

php_coroutine_task_t PHPCoroutine::main_task = {0};

/* 创建 PHP 协程 */
long PHPCoroutine::create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv)
{
    php_coroutine_arg_t args;

    args.fci_cache = fci_cache;
    args.argv = argv;
    args.argc = argc;

    save_task(get_task());

    return Coroutine::create(create_func, (void *) &args);
}

/* 添加延迟执行的函数 */
void PHPCoroutine::defer(php_function_t *func)
{
    php_coroutine_task_t *task = (php_coroutine_task_t *) get_task();
    if (task->defer_tasks == nullptr) {
        task->defer_tasks = new std::stack<php_function_t *>;
    }

    task->defer_tasks->push(func);
}

/* 保存当前协程堆栈信息 */
void PHPCoroutine::save_task(php_coroutine_task_t *task)
{
    save_vm_stack(task);
}

/* 保存当前协程堆栈信息 */
void PHPCoroutine::save_vm_stack(php_coroutine_task_t *task)
{
    task->vm_stack_top = EG(vm_stack_top);
    task->vm_stack_end = EG(vm_stack_end);
    task->vm_stack = EG(vm_stack);
    task->vm_stack_page_size = EG(vm_stack_page_size);
    task->execute_data = EG(current_execute_data);
}

/* 获取当前协程堆栈信息 */
php_coroutine_task_t *PHPCoroutine::get_task()
{
    php_coroutine_task_t *task = (php_coroutine_task_t *) Coroutine::get_current_task();
    return task ? task : &main_task;
}

void PHPCoroutine::create_func(void *arg)
{
    int i;
    php_coroutine_task_t *task;
    zend_execute_data *call;
    zval _retval, *retval = &_retval;

    /* 取出 arg 中的参数 */
    php_coroutine_arg_t *php_arg = (php_coroutine_arg_t *) arg;
    zend_fcall_info_cache fci_cache = *php_arg->fci_cache;
    zend_function *func = fci_cache.function_handler;
    zval *argv = php_arg->argv;
    int argc = php_arg->argc;

    /* 初始化一个新的 PHP 栈 */
    vm_stack_init();
    call = (zend_execute_data *) EG(vm_stack_top);
    task = (php_coroutine_task_t *) EG(vm_stack_top);
    EG(vm_stack_top) = (zval *) ((char *) call + php_coroutine_task_t_SLOT * sizeof(zval));

    /* 分配一块用于当前作用域的内存空间 */
    call = zend_vm_stack_push_call_frame(
            ZEND_CALL_TOP_FUNCTION | ZEND_CALL_ALLOCATED,
            func, argc, fci_cache.called_scope, fci_cache.object
    );

    /* 将我们传递给用户函数的参数一个一个的拷贝到 zend_execute_data */
    for (i = 0; i < argc; ++i) {
        zval *param;
        zval *val = &argv[i];
        param = ZEND_CALL_ARG(call, i + 1);
        ZVAL_COPY(param, val);
    }

    call->symbol_table = NULL;
    EG(current_execute_data) = call;

    /* 将当前的协程栈信息保存到 task */
    save_vm_stack(task);

    task->coroutine = Coroutine::get_current();
    task->coroutine->set_task((void *) task);
    task->defer_tasks = nullptr;

    if (func->type == ZEND_USER_FUNCTION) {
        ZVAL_UNDEF(retval);
        EG(current_execute_data) = NULL;
        zend_init_func_execute_data(call, &func->op_array, retval);
        zend_execute_ex(EG(current_execute_data));
    }

    run_defer_tasks(get_task());

    zval_ptr_dtor(retval);
}

/* 初始化一个新的 PHP 栈 */
void PHPCoroutine::vm_stack_init()
{
    uint32_t size = DEFAULT_PHP_STACK_PAGE_SIZE;
    zend_vm_stack page = (zend_vm_stack) emalloc(size);

    /* 将堆模拟成栈 */
    page->top = ZEND_VM_STACK_ELEMENTS(page);
    page->end = (zval *) ((char *) page + size);
    page->prev = NULL;

    /* 修改现在的 PHP 栈，让它指向我们申请出来的新的PHP栈空间 */
    EG(vm_stack) = page;
    EG(vm_stack)->top++;
    EG(vm_stack_top) = EG(vm_stack)->top;
    EG(vm_stack_end) = EG(vm_stack)->end;
    EG(vm_stack_page_size) = size;
}

/* 运行延迟函数 */
void PHPCoroutine::run_defer_tasks(php_coroutine_task_t *task)
{
    if (task->defer_tasks == nullptr) {
        return;
    }

    zval result;
    php_function_t *func;
    std::stack<php_function_t *> *defer_tasks = task->defer_tasks;

    while (!defer_tasks->empty()) {
        func = defer_tasks->top();
        defer_tasks->pop();
        func->fci.retval = &result;

        if (zend_call_function(&func->fci, &func->fcc) != SUCCESS) {
            php_error_docref(NULL, E_WARNING, "execute defer error");
            return;
        }

        efree(func);
    }

    delete defer_tasks;
    task->defer_tasks = nullptr;
}

/* 协程休眠 */
int PHPCoroutine::sleep(double seconds)
{
    return Coroutine::sleep(seconds);
}

/* 当协程被让出时的回调函数 */
void PHPCoroutine::on_yield(void *arg)
{
    php_coroutine_task_t *task = (php_coroutine_task_t *) arg;
    php_coroutine_task_t *origin_task = get_origin_task(task);

    save_task(task);
    restore_task(origin_task);
}

/* 当协程被恢复时的回调函数 */
void PHPCoroutine::on_resume(void *arg)
{
    php_coroutine_task_t *task = (php_coroutine_task_t *) arg;
    php_coroutine_task_t *current_task = get_task();

    save_task(current_task);
    restore_task(task);
}

/* 重载 PHP 堆栈 */
void PHPCoroutine::restore_task(php_coroutine_task_t *task)
{
    restore_vm_stack(task);
}

/* 重载 PHP 堆栈 */
inline void PHPCoroutine::restore_vm_stack(php_coroutine_task_t *task)
{
    EG(vm_stack_top) = task->vm_stack_top;
    EG(vm_stack_end) = task->vm_stack_end;
    EG(vm_stack) = task->vm_stack;
    EG(vm_stack_page_size) = task->vm_stack_page_size;
    EG(current_execute_data) = task->execute_data;
}

void PHPCoroutine::init()
{
    Coroutine::set_on_yield(on_yield);
    Coroutine::set_on_resume(on_resume);
}
