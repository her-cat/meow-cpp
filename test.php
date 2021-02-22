<?php

use Meow\Coroutine\Socket;
use function Meow\go;

meow_event_init();

go(function () {
    $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    $socket->bind("127.0.0.1", 8080);
    $socket->listen();
    while (true) {
        $conn = $socket->accept();
        go(function () use($conn) {
            $data = $conn->recv();
            $responseStr = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: 11\r\n\r\nhello world\r\n";
            $conn->send($responseStr);
            $conn->close();
        });
    }
});

meow_event_wait();
