--TEST--
Test function xzwrite() by calling it with its expected arguments
--SKIPIF--
<?php
if(!extension_loaded('xz')){
	exit('skip XZ extension is not loaded!');
}
?>
--FILE--
<?php

$filename = tempnam(sys_get_temp_dir(), 'LZMA');
$h        = xzopen($filename, 'w');
$str      = 'Here is the string to be written. ';

// write entire string
var_dump(xzwrite($h, $str));
// write with specified length
var_dump(xzwrite($h, $str, 10));

xzclose($h);

$h   = xzopen($filename, 'r');
// read the entire content
$len = xzpassthru($h);

xzclose($h);

echo "\n";

var_dump($len);

unlink($filename);

?>
--EXPECT--
int(34)
int(10)
Here is the string to be written. Here is th
int(44)
