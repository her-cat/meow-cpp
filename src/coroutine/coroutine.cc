#include "timer.h"
#include "coroutine.h"

using meow::Coroutine;

Coroutine *Coroutine::current = nullptr;
size_t Coroutine::stack_size = DEFAULT_C_STACK_SIZE;
long Coroutine::last_cid = 0;
std::unordered_map<long, Coroutine *> Coroutine::coroutines;
meow_coroutine_swap_function_t Coroutine::on_yield = nullptr;
meow_coroutine_swap_function_t Coroutine::on_resume = nullptr;
meow_coroutine_swap_function_t Coroutine::on_close = nullptr;

/* 创建协程 */
long Coroutine::create(coroutine_function_t fn, void *args)
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

/* 让出协程 */
void Coroutine::yield()
{
    on_yield(task);
    /* 设置上一个协程作为当前正在运行的协程 */
    current = origin;
    /* 切换协程上下文 */
    ctx.swap_out();
}

/* 恢复协程 */
void Coroutine::resume()
{
    on_resume(task);
    origin = current;
    current = this;
    ctx.swap_in();

    if (ctx.is_end()) {
        on_close(task);
        cid = current->get_cid();
        /* 设置之前正在运行的协程 */
        current = origin;
        /* 删除当前协程 */
        coroutines.erase(cid);
        delete this;
    }
}

/* 协程休眠定时器超时回调函数 */
static void sleep_timeout(void *data)
{
    /* 到达指定时间后恢复协程 */
    ((Coroutine *) data)->resume();
}

/* 协程休眠 */
int Coroutine::sleep(double seconds)
{
    Coroutine *coroutine = Coroutine::get_current();

    /* 添加定时器，到达指定时间后执行 sleep_timeout 函数 */
    timer_manager.add_timer(seconds * Timer::SECOND, sleep_timeout, (void *) coroutine);

    /* 切换出当前协程，模拟出协程自身阻塞的效果 */
    coroutine->yield();

    return 0;
}

/* 设置让出协程时的回调函数 */
void Coroutine::set_on_yield(meow_coroutine_swap_function_t function)
{
    on_yield = function;
}

/* 设置恢复协程时的回调函数 */
void Coroutine::set_on_resume(meow_coroutine_swap_function_t function)
{
    on_resume = function;
}

void Coroutine::set_on_close(meow_coroutine_swap_function_t function)
{
    on_close = function;
}
