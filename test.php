<?php

use Meow\Co;
use function Meow\go;

go(function () {
    $server = new Meow\Coroutine\Server("127.0.0.1", 8080);
    while (true) {
        go(function ($conn_fd) use ($server) {
            while (true) {
                $msg = $server->recv($conn_fd);
                if ($msg == false) {
                    break;
                }
                $server->send($conn_fd, $msg);
            }
        }, $server->accept());
    }
});

Co::scheduler();