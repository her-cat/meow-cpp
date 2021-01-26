<?php

use Meow\Co;
use function Meow\go;

meow_event_init();

go(function () {
    var_dump(Co::getCid());
    Co::sleep(1);
    var_dump(Co::getCid());
});

go(function () {
    var_dump(Co::getCid());
    Co::sleep(1);
    var_dump(Co::getCid());
});

meow_event_wait();
