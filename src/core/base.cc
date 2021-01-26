#include "coroutine.h"
#include "timer.h"
#include "meow.h"
#include "log.h"

using meow::Coroutine;
using meow::Timer;
using meow::TimerManager;
using meow::timer_manager;

/* 定义 Meow 全局变量 */
MEOW_GLOBALS_DECLARE(meow)

/* 初始化全局变量 poll */
int meow_poll_init()
{
    try {
        MEOW_G(poll) = new meow_poll_t();
    } catch (const std::exception &e) {
        meow_error("%s", e.what())
    }

    MEOW_G(poll)->epollfd = epoll_create(256);
    if (MEOW_G(poll)->epollfd < 0) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno))
        delete MEOW_G(poll);
        MEOW_G(poll) = nullptr;
        return -1;
    }

    MEOW_G(poll)->size = 16;
    MEOW_G(poll)->event_num = 0;

    try {
        MEOW_G(poll)->events = new epoll_event[MEOW_G(poll)->size]();
    } catch (const std::bad_alloc &e) {
        meow_error("%s", e.what())
    }

    return 0;
}

/* 释放全局变量 poll */
int meow_poll_free()
{
    if (close(MEOW_G(poll)->epollfd) < 0) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno))
    }

    delete[] MEOW_G(poll)->events;
    MEOW_G(poll)->events = nullptr;
    delete MEOW_G(poll);
    MEOW_G(poll) = nullptr;

    return 0;
}

/* 调度器初始化 */
int meow_event_init()
{
    if (!MEOW_G(poll)) {
        meow_poll_init();
    }

    MEOW_G(running) = 1;

    return 0;
}

/* 调度器 */
int meow_event_wait()
{
    meow_event_init();

    while (MEOW_G(running) > 0) {
        int n, timeout;
        epoll_event *events;

        /* 获取超时时间 */
        timeout = timer_manager.get_next_timeout();

        events = MEOW_G(poll)->events;
        /* 通过 epoll_wait 让进程进入休眠，
         * 当有文件描述符可读/写，或到达超时时间时，进程会从休眠状态醒过来 */
        n = epoll_wait(MEOW_G(poll)->epollfd, MEOW_G(poll)->events, MEOW_G(poll)->size, timeout);

        for (int i = 0; i < n; i++) {
            int fd, id;
            epoll_event *p = &events[i];
            uint64_t u64 = p->data.u64;
            Coroutine *coroutine;

            fromuint64(u64, &fd, &id);
            coroutine = Coroutine::get_by_cid(id);
            if (coroutine) {
                coroutine->resume();
            }
        }

        timer_manager.run_timers();

        /* 没有未执行的定时器并且事件数量为 0 */
        if (timer_manager.get_next_timeout() < 0 && MEOW_G(poll)->event_num == 0) {
            MEOW_G(running) = 0;
        }
    }

    meow_event_free();

    return 0;
}

/* 释放调度器 */
int meow_event_free()
{
    meow_poll_free();
    MEOW_G(running) = 0;

    return 0;
}
