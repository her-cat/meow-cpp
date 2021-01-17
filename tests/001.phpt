--TEST--
Check if meow is loaded
--SKIPIF--
<?php
if (!extension_loaded('meow')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "meow" is available';
?>
--EXPECT--
The extension "meow" is available
