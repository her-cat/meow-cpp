<?php

use Meow\Co;
use Meow\Coroutine\Server;
use function Meow\go;

$server = new Server("127.0.0.1", 8080);

var_dump($server);
