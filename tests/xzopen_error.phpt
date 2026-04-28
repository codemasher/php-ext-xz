--TEST--
Test `xzopen`: error conditions.
--SKIPIF--
<?php
if(!extension_loaded('xz')){
	exit('skip XZ extension is not loaded!');
}
?>
--FILE--
<?php

var_dump(xzopen('./test.txt', 'a'));

?>
--EXPECTF--
Warning: xzopen(): Can only open in read (r) or write (w) mode.%s
bool(false)
