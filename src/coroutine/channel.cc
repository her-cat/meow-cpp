#include "timer.h"
#include "coroutine_channel.h"

using meow::Coroutine;
using meow::coroutine::Channel;
using meow::Timer;
using meow::TimerManager;
using meow::timer_manager;

Channel::Channel(size_t _capacity):
    capacity(_capacity)
{
}

Channel::~Channel()
{
}

static void sleep_timeout(void *param)
{
    ((Coroutine *) param)->resume();
}

void *Channel::pop(double timeout)
{
    void *data;
    Coroutine *coroutine = Coroutine::get_current();

    /* 如果数据队列为空 */
    if (data_queue.empty()) {
        if (timeout > 0) {
            /* 设置了等待数据的超时时间，添加一个定时器，在指定时间后唤醒协程 */
            timer_manager.add_timer(timeout * Timer::SECOND, sleep_timeout, (void *) coroutine);
        }
        /* 将协程加入消费者等待队列 */
        consumer_queue.push(coroutine);
        /* 让出当前协程 */
        coroutine->yield();
    }

    /* 如果协程等待超时被唤醒后数据队列还是空的，返回 null */
    if (data_queue.empty()) {
        return nullptr;
    }

    /* 取出数据队列中的数据 */
    data = data_queue.front();
    data_queue.pop();

    /* 如果有生产者协程在等待就恢复该生产者协程 */
    if (!producer_queue.empty()) {
        coroutine = producer_queue.front();
        producer_queue.pop();
        coroutine->resume();
        /* 当生产者协程执行完毕或主动 yield 时会回到这里继续执行 */
    }

    return data;
}

bool Channel::push(void *data, double timeout)
{
    Coroutine *coroutine = Coroutine::get_current();

    /* 如果数据队列已满 */
    if (data_queue.size() == capacity) {
        if (timeout > 0) {
            /* 设置了等待数据的超时时间，添加一个定时器，在指定时间后唤醒当前协程 */
            timer_manager.add_timer(timeout * Timer::SECOND, sleep_timeout, (void *) coroutine);
        }
        /* 将当前协程加入生产者等待队列 */
        producer_queue.push(coroutine);
        /* 让出当前协程 */
        coroutine->yield();
    }

    /* 如果协程等待超时被唤醒后数据队列还是满的，返回 false */
    if (data_queue.size() == capacity) {
        return false;
    }

    /* 添加数据 */
    data_queue.push(data);

    /* 如果有消费者协程在等待就恢复该消费者协程 */
    if (!consumer_queue.empty()) {
        coroutine = consumer_queue.front();
        consumer_queue.pop();
        coroutine->resume();
        /* 当消费者协程执行完毕或主动 yield 时会回到这里继续执行 */
    }

    return true;
}
