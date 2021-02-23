#ifndef MEOW_COROUTINE_H
#define MEOW_COROUTINE_H

#include <unordered_map>
#include "context.h"

#define DEFAULT_C_STACK_SIZE (2 * 1024 * 1024)

typedef void (*meow_coroutine_swap_function_t)(void *);

namespace meow
{
class Coroutine
{
public:
    static std::unordered_map<long, Coroutine *> coroutines;

    static long create(coroutine_function_t fn, void *args = nullptr);
    static void *get_current_task(void);
    void *get_task();
    void set_task(void *_task);
    static Coroutine *get_current(void);
    void yield();
    void resume();
    static int sleep(double seconds);
    static void set_on_yield(meow_coroutine_swap_function_t function);
    static void set_on_resume(meow_coroutine_swap_function_t function);
    static void set_on_close(meow_coroutine_swap_function_t function);

    inline long get_cid() {
        return cid;
    }

    inline Coroutine *get_origin() {
        return origin;
    }

    static inline Coroutine *get_by_cid(long id) {
        auto i = coroutines.find(id);
        return i != coroutines.end() ? i->second : nullptr;
    }

protected:
    void *task = nullptr;
    Coroutine *origin; /* 前一个协程 */
    static Coroutine *current; /* 正在运行的协程 */
    static size_t stack_size; /* 协程栈大小 */
    Context ctx; /* 协程的上下文 */
    long cid; /* 协程 id */
    static long last_cid; /* 最后一个协程 id */
    static meow_coroutine_swap_function_t on_yield;
    static meow_coroutine_swap_function_t on_resume;
    static meow_coroutine_swap_function_t on_close;

    Coroutine(coroutine_function_t fn, void *private_data) :
        ctx(stack_size, fn, private_data)
    {
        cid = ++last_cid;
        coroutines[cid] = this;
    }

    long run()
    {
        long _cid = this->cid;
        /* 保存正在运行的协程 */
        origin = current;
        /* 将当前协程设置为正在运行的协程 */
        current = this;
        /* 进入当前协程的上下文 */
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

        return _cid;
    }
};
}

#endif  /* MEOW_COROUTINE_H */
