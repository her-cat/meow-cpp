#include "coroutine.h"

using meow::Coroutine;

Coroutine *Coroutine::current = nullptr;
size_t Coroutine::stack_size = DEFAULT_C_STACK_SIZE;
long Coroutine::last_cid = 0;
std::unordered_map<long, Coroutine *> Coroutine::coroutines;

/* 创建协程 */
long Coroutine::create(coroutine_func_t fn, void *args)
{
    return (new Coroutine(fn, args))->run();
}

/* 获取当前协程堆栈信息 */
void *Coroutine::get_current_task()
{
    return current ? Coroutine::current->get_task() : nullptr;
}

/* 获取当前协程堆栈信息 */
void *Coroutine::get_task()
{
    return task;
}

/* 设置当前协程堆栈信息 */
void Coroutine::set_task(void *_task)
{
    task = _task;
}

/* 获取当前协程 */
Coroutine *Coroutine::get_current()
{
    return current;
}
