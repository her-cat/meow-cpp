<?php

use Meow\Co;
use function Meow\go;

$server = new Meow\Coroutine\Server("127.0.0.1", 8080);

var_dump($server);
