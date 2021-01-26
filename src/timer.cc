#include "timer.h"

using meow::Timer;
using meow::TimerManager;

/* 定义定时器管理类的全局变量 */
TimerManager meow::timer_manager;

const uint64_t Timer::MILLISECOND = 1;
const uint64_t Timer::SECOND = 1000;

/* 定时器的构造函数 */
Timer::Timer(uint64_t _timeout, timer_function_t _callback, void *_private_data, TimerManager *_timer_manager) :
    timeout(_timeout), callback(_callback), private_data(_private_data), timer_manager(_timer_manager)
{
    exec_msec = get_current_ms() + _timeout;
}

/* 获取当前时间戳 */
uint64_t Timer::get_current_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

/* 添加定时器 */
void TimerManager::add_timer(uint64_t _timeout, timer_function_t _callback, void *_private_data)
{
    timers.push(new Timer(_timeout, _callback, _private_data, this));
}

/* 获取距离下一个定时器超时的时间 */
int64_t TimerManager::get_next_timeout()
{
    int64_t diff;

    if (timers.empty()) {
        return -1;
    }

    diff = timers.top()->exec_msec - Timer::get_current_ms();

    return diff < 0 ? 0 : diff;
}

/* 运行定时器 */
void TimerManager::run_timers()
{
    uint64_t now = Timer::get_current_ms();

    while (true) {
        if (timers.empty()) {
            break;
        }

        Timer *t = timers.top();
        if (now < t->exec_msec) {
            break;
        }

        timers.pop();
        t->callback(t->private_data);
        delete t;
    }
}
