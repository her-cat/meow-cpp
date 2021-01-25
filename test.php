<?php

use Meow\Co;
use function Meow\go;

meow_event_init();

go(function () {
    $cid = Co::getCid();
    var_dump(1);
    Co::sleep(1);
    var_dump(2);
});

go(function () {
    $cid = Co::getCid();
    var_dump(3);
    Co::sleep(1);
    var_dump(4);
});

meow_event_wait();
