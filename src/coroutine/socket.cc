#include "coroutine_socket.h"
#include "coroutine.h"
#include "socket.h"

using meow::coroutine::Socket;
using meow::Coroutine;

/* Socket 构造函数 */
Socket::Socket(int domain, int type, int protocol)
{
    sockfd = meow_socket_create(domain, type, protocol);
    if (sockfd < 0) {
        return;
    }

    meow_socket_set_nonblock(sockfd);
}

Socket::Socket(int fd)
{
    sockfd = fd;
    meow_socket_set_nonblock(fd);
}

/* 析构函数 */
Socket::~Socket()
{

}

/* 绑定 socket */
int Socket::bind(int type, char *host, int port)
{
    return meow_socket_bind(sockfd, type, host, port);
}

/* 监听 socket */
int Socket::listen()
{
    return meow_socket_listen(sockfd);
}

/* 接受请求 */
int Socket::accept()
{
    int connfd;

    do {
        connfd = meow_socket_accept(sockfd);
    } while (connfd < 0 && errno == EAGAIN && wait_event(MEOW_EVENT_READ));

    return connfd;
}

/* 接收数据 */
ssize_t Socket::recv(void *buf, size_t len)
{
    ssize_t ret;

    do {
        ret = meow_socket_recv(sockfd, buf, len, 0);
    } while (ret < 0 && errno == EAGAIN && wait_event(MEOW_EVENT_READ));

    return ret;
}

/* 发送数据 */
ssize_t Socket::send(const void *buf, size_t len)
{
    ssize_t ret;

    do {
        ret = meow_socket_send(sockfd, buf, len, 0);
    } while (ret < 0 && errno == EAGAIN && wait_event(MEOW_EVENT_WRITE));

    return ret;
}

/* 关闭 socket */
int Socket::close()
{
    return meow_socket_close(sockfd);
}

/* 等待事件并让出协程 */
bool Socket::wait_event(int event)
{
    long id;
    Coroutine *coroutine;
    epoll_event *ev;

    if (!MEOW_G(poll)) {
        init_meow_poll();
    }

    coroutine = Coroutine::get_current();
    id = coroutine->get_cid();
    ev = MEOW_G(poll)->events;

    ev->events = event == MEOW_EVENT_READ ? EPOLLIN : EPOLLOUT;
    ev->data.u64 = touint64(sockfd, id);
    /* 将事件注册到全局的 epollfd 上 */
    epoll_ctl(MEOW_G(poll)->epollfd, EPOLL_CTL_ADD, sockfd, ev);

    /* 让出当前协程 */
    coroutine->yield();

    return true;
}

