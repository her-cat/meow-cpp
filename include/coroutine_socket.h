#ifndef MEOW_COROUTINE_SOCKET_H
#define MEOW_COROUTINE_SOCKET_H

#include "meow.h"

#define MEOW_BAD_SOCKET ((Socket *) - 1)

namespace meow
{
namespace coroutine
{
class Socket
{
private:
    int sockfd;
public:
    /* 读写缓冲区 */
    static char *read_buffer;
    static size_t read_buffer_len;
    static char *write_buffer;
    static size_t write_buffer_len;

    static int init_read_buffer();
    static int init_write_buffer();

    Socket(int domain, int type, int protocol);
    Socket(int fd);
    ~Socket();
    int bind(int type, char *host, int port);
    int listen();
    int accept();
    ssize_t recv(void *buf, size_t len);
    ssize_t send(const void *buf, size_t len);
    int close();
    bool wait_event(int event);
    int get_fd();
};
}
}

#endif /* MEOW_COROUTINE_SOCKET_H */
