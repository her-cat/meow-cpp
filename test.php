<?php

use Meow\Co;
use function Meow\go;

meow_event_init();

go(function () {
    $serv = new Meow\Coroutine\Server("127.0.0.1", 8080);
    while (true) {
        $conn_fd = $serv->accept();

        go(function ($serv, $conn_fd) {
            $buf = $serv->recv($conn_fd);
            $response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: 11\r\n\r\nhello world\r\n";
            $serv->send($conn_fd, $response);
            $serv->close($conn_fd);
        }, $serv, $conn_fd);
    }
});

meow_event_wait();
