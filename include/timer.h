#ifndef MEOW_TIMER_H
#define MEOW_TIMER_H

#include "meow.h"

/* 定时器回调函数原型 */
typedef void (*timer_function_t)(void *);

namespace meow
{
    class TimerManager;

    class Timer
    {
        friend class TimerManager;
        friend class CompareTimerPointer;

    public:
        static const uint64_t MILLISECOND;
        static const uint64_t SECOND;

        Timer(uint64_t _timeout, timer_function_t _callback, void *_private_data, TimerManager *_timer_manager);
        static uint64_t get_current_ms();

    private:
        uint64_t timeout = 0; /* 定时器过期的时间 */
        uint64_t exec_msec = 0; /* 定时器执行时间 */
        timer_function_t callback; /* 定时器回调函数 */
        void *private_data; /* 定时器携带的数据 */
        TimerManager *timer_manager;
    };

    class CompareTimerPointer
    {
    public:
        bool operator () (Timer *&timer1, Timer *&timer2) const
        {
            return timer1->exec_msec > timer2->exec_msec;
        }
    };

    class TimerManager
    {
    public:
        TimerManager();
        ~TimerManager();

        void add_timer(uint64_t _timeout, timer_function_t _callback, void *_private_data);
        int64_t get_next_timeout();
        void run_timers();

    private:
        std::priority_queue<Timer *, std::vector<Timer *>, CompareTimerPointer> timers; /* 定时器优先队列 */
    };

    extern TimerManager timer_manager;
}

#endif /* MEOW_TIMER_H */
