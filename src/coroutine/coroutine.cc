#include "coroutine.h"

using Meow::Coroutine;

Coroutine *Coroutine::current = nullptr;

/* 获取当前协程堆栈信息 */
void *Coroutine::get_current_task()
{
    return Coroutine::current ? Coroutine::current->get_task() : nullptr;
}

/* 获取当前协程堆栈信息 */
void *Coroutine::get_task()
{
    return task;
}
