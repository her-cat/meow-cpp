<?php

use function Meow\go;

meow_event_init();

go(function () {
    $server = new Meow\Coroutine\Server("127.0.0.1", 8080);
    $server->set_handler(function (Meow\Coroutine\Socket $conn) use($server) {
        $data = $conn->recv();
        $responseStr = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: 11\r\n\r\nhello world\r\n";
        $conn->send($responseStr);
        $conn->close();
    });
    $server->start();
});

meow_event_wait();
