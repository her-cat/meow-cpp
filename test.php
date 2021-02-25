<?php

use function Meow\go;

meow_event_init();

Meow\Runtime::enableCoroutine();

go(function ()
{
    var_dump(Meow\Coroutine::getCid());
    sleep(1);
    var_dump(Meow\Coroutine::getCid());
});

go(function ()
{
    var_dump(Meow\Coroutine::getCid());
});


meow_event_wait();
