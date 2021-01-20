#ifndef MEOW_H
#define MEOW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// include standard library
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>

#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#define MEOW_GLOBALS_STRUCT(name)       name##_globals_s
#define MEOW_GLOBALS_TYPE(name)         name##_globals_t
#define MEOW_GLOBALS(name)              name##_globals

#define MEOW_GLOBALS_STRUCT_BEGIN(name) typedef struct MEOW_GLOBALS_STRUCT(name) {
#define MEOW_GLOBALS_STRUCT_END(name)   } MEOW_GLOBALS_TYPE(name);

#define MEOW_GLOBALS_DECLARE(name)      MEOW_GLOBALS_TYPE(name) MEOW_GLOBALS(name);
#define MEOW_GLOBALS_GET(name, value)   MEOW_GLOBALS(name).value

/* Meow 全局变量宏 */
#define MEOW_G(x)                       MEOW_GLOBALS_GET(meow, x)

typedef struct {
    int epollfd; /* 保存创建的 epoll fd */
    int size; /* events 数组的大小 */
    struct epoll_event *events; /* 保存 epoll 返回的事件 */
} meow_poll_t;

/* Meow 全局变量 */
MEOW_GLOBALS_STRUCT_BEGIN(meow)
    meow_poll_t *poll;
MEOW_GLOBALS_STRUCT_END(meow)

/* 声明 Meow 全局变量 */
extern MEOW_GLOBALS_DECLARE(meow)

/* 事件类型 */
enum meow_event_type {
    MEOW_EVENT_NULL     = 0,
    MEOW_EVENT_DEFAULT  = 1u << 8,
    MEOW_EVENT_READ     = 1u << 9,
    MEOW_EVENT_WRITE    = 1u << 10,
    MEOW_EVENT_RDWR     = MEOW_EVENT_READ | MEOW_EVENT_WRITE,
    MEOW_EVENT_ERROR    = 1u << 11,
};

/* 将 fd 和 id 编码为一个 uint_64_t 的值 */
static inline uint64_t touint64(int fd, int id)
{
    uint64_t ret;
    ret |= ((u_int64_t) fd) << 32;
    ret |= ((u_int64_t) id);
    return ret;
}

/* 从一个 uint_64_t 的值中解码出 fd 和 id */
static inline void fromuint64(uint64_t v, int *fd, int *id)
{
    *id = (int) (v >> 32);
    *id = (int) (v & 0xffffffff);
}

/* 初始化全局变量 poll */
static inline void init_meow_poll()
{
    size_t size;

    MEOW_G(poll) = (meow_poll_t *) malloc(sizeof(meow_poll_t));

    MEOW_G(poll)->epollfd = epoll_create(256);
    MEOW_G(poll)->size = 16;
    size = sizeof(struct epoll_event) * MEOW_G(poll)->size;
    MEOW_G(poll)->events = (struct epoll_event *) malloc(size);
    memset(MEOW_G(poll)->events, 0, size);
}

/* 释放全局变量 poll */
static inline void free_meow_poll()
{
    free(MEOW_G(poll)->events);
    free(MEOW_G(poll));
}

#endif /* MEOW_H */
