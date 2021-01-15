#ifndef MEOW_CONTEXT_H
#define MEOW_CONTEXT_H

#include "asm_context.h"

typedef fcontext_t coroutine_context_t;
typedef void (*coroutine_func_t)(void *);

namespace meow
{
class Context
{
public:
    Context(size_t stack_size, coroutine_func_t fn, void *private_data);
    ~Context();
    static void context_func(void *arg);
    bool swap_in();
    bool swap_out();
    inline bool is_end()
    {
        return end_;
    }

protected:
    char *stack_; /* 当前上下文使用的堆栈 */
    coroutine_context_t ctx_; /* 当前协程上下文 */
    coroutine_context_t swap_ctx_; /* 前一个协程上下文 */
    uint32_t stack_size_; /* 当前上下文使用的堆栈大小 */
    coroutine_func_t fn_; /* 协程回调函数 */
    void *private_data_; /* 协程回调函数携带的数据 */
    bool end_ = false; /* 是否执行结束 */
};
}

#endif  /* MEOW_CONTEXT_H */
