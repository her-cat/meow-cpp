--TEST--
meow_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('meow')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = meow_test1();

var_dump($ret);
?>
--EXPECT--
The extension meow is loaded and working!
NULL
