<?php

use Meow\Co;
use function Meow\go;

go(function () {
    $server = new Meow\Coroutine\Server("127.0.0.1", 8080);
    while (true) {
        $conn_fd = $server->accept();
        if ($conn_fd < 0) {
            break;
        }

        while (true) {
            $msg = $server->recv($conn_fd);
            if ($msg == false) {
                break;
            }
            var_dump($msg);
        }
    }
});

Co::scheduler();