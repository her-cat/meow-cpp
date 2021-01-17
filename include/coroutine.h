#ifndef MEOW_COROUTINE_H
#define MEOW_COROUTINE_H

#include <unordered_map>
#include "context.h"

#define DEFAULT_C_STACK_SIZE (2 * 1024 * 1024)

namespace meow
{
class Coroutine
{
public:
    static std::unordered_map<long, Coroutine *> coroutines;

    static long create(coroutine_func_t fn, void *args = nullptr);
    static void *get_current_task(void);
    void *get_task();
    void set_task(void *_task);
    static Coroutine *get_current(void);
    void yield();
    void resume();
    inline long get_cid() {
        return cid;
    }

protected:
    void *task = nullptr;
    Coroutine *origin; /* 前一个协程 */
    static Coroutine *current; /* 正在运行的协程 */
    static size_t stack_size; /* 协程栈大小 */
    Context ctx; /* 协程的上下文 */
    long cid; /* 协程 id */
    static long last_cid; /* 最后一个协程 id */

    Coroutine(coroutine_func_t fn, void *private_data) :
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
