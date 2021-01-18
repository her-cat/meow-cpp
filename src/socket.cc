#include "socket.h"

/* 创建 socket */
int meow_socket_create(int type)
{
    int _domain, _type;

    if (type == MEOW_SOCK_TCP) {
        _domain = AF_INET;
        _type = SOCK_STREAM;
    } else if (type == MEOW_SOCK_UDP) {
        _domain = AF_INET;
        _type = SOCK_DGRAM;
    } else {
        return -1;
    }

    return socket(_domain, _type, 0);
}

/* 绑定 socket */
int meow_socket_bind(int sock, int type, char *host, int port)
{
    int ret;
    struct sockaddr_in servaddr;

    if (type == MEOW_SOCK_TCP) {
        bzero(&servaddr, sizeof(servaddr));
        inet_aton(host, &(servaddr.sin_addr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);

        ret = bind(sock, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (ret < 0) {
            meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno));
            return -1;
        }

        return ret;
    }

    return -1;
}

/* 监听 socket */
int meow_socket_listen(int sock)
{
    int ret;

    ret = listen(sock, DEFAULT_LISTEN_BACKLOG);
    if (ret < 0) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno));
    }

    return ret;
}

/* 接受新连接 */
int meow_socket_accept(int sock)
{
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);

    return accept(sock, (struct sockaddr *) &sa, &len);
}

/* 接收数据 */
ssize_t meow_socket_recv(int sock, void *buf, size_t len, int flag)
{
    ssize_t ret;

    ret = recv(sock, buf, len, flag);
    if (ret < 0) {
        meow_warn("Error has occurred: (errno %d) %s", errno, strerror(errno));
    }

    return ret;
}
