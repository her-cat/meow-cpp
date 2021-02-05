<?php

use Meow\Coroutine\Channel;
use function Meow\go;

meow_event_init();

$chan = new Channel();

go(function () use ($chan) {
    $ret = $chan->push("hello world");
    var_dump($ret);
});

go(function () use ($chan) {
    $ret = $chan->pop();
    var_dump($ret);
});

meow_event_wait();
