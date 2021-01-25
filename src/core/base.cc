#include <uv.h>
#include "coroutine.h"
#include "meow.h"
#include "log.h"

using meow::Coroutine;

/* 定义 Meow 全局变量 */
MEOW_GLOBALS_DECLARE(meow)

typedef enum
{
    UV_CLOCK_PRECISE = 0,  /* Use the highest resolution clock available. */
    UV_CLOCK_FAST = 1      /* Use the fastest clock with <= 1ms granularity. */
} uv_clocktype_t;

extern "C" void uv__run_timers(uv_loop_t* loop);
extern "C" uint64_t uv__hrtime(uv_clocktype_t type);
extern "C" int uv__next_timeout(const uv_loop_t* loop);

/* 初始化全局变量 poll */
int meow_poll_init()
{
    size_t size;

    MEOW_G(poll) = (meow_poll_t *) malloc(sizeof(meow_poll_t));
    if (MEOW_G(poll) == NULL) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno))
        return -1;
    }

    MEOW_G(poll)->epollfd = epoll_create(256);
    if (MEOW_G(poll)->epollfd < 0) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno))
        free(MEOW_G(poll));
        return -1;
    }

    MEOW_G(poll)->size = 16;
    size = sizeof(struct epoll_event) * MEOW_G(poll)->size;
    MEOW_G(poll)->events = (struct epoll_event *) malloc(size);
    memset(MEOW_G(poll)->events, 0, size);

    return 0;
}

/* 释放全局变量 poll */
int meow_poll_free()
{
    free(MEOW_G(poll)->events);
    free(MEOW_G(poll));
    return 0;
}

/* 调度器初始化 */
int meow_event_init()
{
    if (!MEOW_G(poll)) {
        meow_poll_init();
    }

    return 0;
}

/* 调度器 */
int meow_event_wait()
{
    uv_loop_t *loop = uv_default_loop();

    if (!MEOW_G(poll)) {
        meow_error("Need to call st_event_init first.");
    }

    while (loop->stop_flag == 0) {
        int n, timeout;
        epoll_event *events;

        /* 获取超时时间 */
        timeout = uv__next_timeout(loop);

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

        /* 修改当前的时间 */
        loop->time = uv__hrtime(UV_CLOCK_FAST) / 1000000;
        /* 运行定时器，遍历整个定时器堆，让每个定时器节点时间和 loop->time 进行比较，
         * 如果定时器节点的时间大于 loop->time，就会指定这个定时器节点的回调函数 */
        uv__run_timers(loop);

        /* 没有未执行的定时器并且 poll 已经被释放 */
        if (uv__next_timeout(loop) < 0 && !MEOW_G(poll)) {
            uv_stop(loop);
        }
    }

    meow_event_free();

    return 0;
}

/* 释放调度器 */
int meow_event_free()
{
    meow_poll_free();
    return 0;
}
