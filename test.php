<?php

use Meow\Co;
use function Meow\go;

go(function () {
    $server = new Meow\Coroutine\Server("127.0.0.1", 8080);
    while (true) {
        $conn_fd = $server->accept();
        var_dump($conn_fd);
        $server->close($conn_fd);
    }
});

Co::scheduler();