<?php

use Meow\Coroutine\Channel;

$chan = new Channel();
var_dump($chan);

$chan = new Channel(-1);
var_dump($chan);

$chan = new Channel(5);
var_dump($chan);
