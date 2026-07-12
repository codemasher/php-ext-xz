--TEST--
Test issue #14 incomplete data. https://github.com/codemasher/php-ext-xz/issues/14
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	exit("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php

$xzFilePath       = __DIR__.'/issue-14-data.xz';
$compressedData   = file_get_contents($xzFilePath);
$decompressedData = xzdecode($compressedData);

$expected = 12360;
$actual   = strlen($decompressedData);

printf("Expected size: %d bytes, actual size: %d bytes.\n", $expected, $actual);

var_dump($expected === $actual);
var_dump(xzdecode(substr($compressedData, 0, -1)));

$first  = str_repeat('first', 1000);
$second = str_repeat('second', 1000);
var_dump(xzdecode(xzencode($first).xzencode($second)) === $first.$second);

?>
--EXPECTF--
Expected size: 12360 bytes, actual size: 12360 bytes.
bool(true)
bool(false)
bool(true)
