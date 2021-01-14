#include "context.h"
#include "meow.h"

using meow::Context;

Context::Context(size_t stack_size, coroutine_func_t fn, void *private_data) :
        stack_size_(stack_size), fn_(fn), private_data_(private_data)
{
    swap_ctx_ = nullptr;
    /* 创建一个 C 栈（实际上是从堆中分配的内存） */
    stack_ = (char *) malloc(stack_size_);
    /* 将堆模拟成栈 */
    void *sp = (void *) ((char *) stack_ + stack_size_);
    /* 将 context_func 回调函数和自定义的堆栈填充到 ctx_ 中 */
    ctx_ = make_fcontext(sp, stack_size_, (void (*)(intptr_t)) &context_func);
}

/* 上下文回调函数 */
void Context::context_func(void *arg)
{
    Context *_this = (Context *) arg;
    _this->fn_(_this->private_data_);
    _this->end_ = true;
    _this->swap_out();
}
/* 进入当前协程的上下文 */
bool Context::swap_in()
{
    jump_fcontext(&swap_ctx_, ctx_, (intptr_t) this, true);
    return true;
}

/* 让出当前协程的上下文 */
bool Context::swap_out()
{
    jump_fcontext(&ctx_, swap_ctx_, (intptr_t) this, true);
    return true;
}

Context::~Context()
{
    if (swap_ctx_) {
        delete[] stack_;
        stack_ = nullptr;
    }
}
