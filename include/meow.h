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
    meow_poll_t poll;
MEOW_GLOBALS_STRUCT_END(meow)

/* 声明 Meow 全局变量 */
extern MEOW_GLOBALS_DECLARE(meow)

#endif /* MEOW_H */
