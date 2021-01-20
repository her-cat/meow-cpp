<?php

use Meow\Co;
use function Meow\go;

go(function () {
    var_dump("here1");
    Co::sleep(0.5);
    var_dump("here2");
    Co::sleep(1);
    var_dump("here3");
    Co::sleep(1.5);
    var_dump("here4");
    Co::sleep(2);
    var_dump("here5");
});

Co::scheduler();
