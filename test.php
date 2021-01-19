<?php

use Meow\Co;
use Meow\Coroutine\Server;
use function Meow\go;

$server = new Server("127.0.0.1", 8080);
$sock = $server->accept();

while (1) {
    $buf = $server->recv($sock);
    if ($buf == false)
    {
        var_dump($server->errCode);
        var_dump($server->errMsg);
        break;
    }
    $server->send($sock, $buf);
}
