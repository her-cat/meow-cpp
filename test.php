<?php

use Meow\Co;
use function Meow\go;

go(function () {
    echo "co1 before sleep" . PHP_EOL;
    Co::sleep(1000);
    echo "co1 after sleep" . PHP_EOL;
});

go(function () {
    echo "co2 before sleep" . PHP_EOL;
    Co::sleep(2000);
    echo "co2 after sleep" . PHP_EOL;
});

echo "main co" . PHP_EOL;

Co::scheduler();
