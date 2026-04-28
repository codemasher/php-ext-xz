--TEST--
Test lzma stream
--SKIPIF--
<?php
if(!extension_loaded('xz')){
	exit('skip XZ extension is not loaded!');
}
if(getenv('SKIP_SLOW_TESTS')){
	exit('skip slow test')
}
?>
--FILE--
<?php

$tmp1 = tempnam(sys_get_temp_dir(), 'LZMA');
$tmp2 = tempnam(sys_get_temp_dir(), 'LZMA');
$tmp3 = tempnam(sys_get_temp_dir(), 'LZMA');

$len0 = filesize(PHP_BINARY);

printf("Compress level 2 (%s)\n", $tmp1);
$opts = [
	'xz' => [
		'compression_level' => '2',
		'max_memory'        => 0,
	]
];
$ctx = stream_context_create($opts);
copy(PHP_BINARY, "compress.lzma://$tmp1", $ctx);
$len1 = filesize($tmp1);
var_dump($len1 > 0);
var_dump($len1 <= $len0);

printf("Compress level 9 (%s)\n", $tmp2);
$opts = [
	'xz' => [
		'compression_level' => '9',
		'max_memory'        => 64*1024*1024,
	]
];
$ctx = stream_context_create($opts);
copy(PHP_BINARY, "compress.lzma://$tmp2", $ctx);
$len2 = filesize($tmp2);
var_dump($len2 > 0);
var_dump($len2 <= $len1);

printf("Uncompress (%s)\n", $tmp3);
copy("compress.lzma://$tmp1", $tmp3);
$len3 = filesize($tmp3);
var_dump($len3 > 0);
var_dump($len3 === $len0);

// compare the final result
var_dump(file_get_contents(PHP_BINARY) === file_get_contents($tmp3));

unlink($tmp1);
unlink($tmp2);
unlink($tmp3);

?>
--EXPECTF--
Compress level 2 (%s)
bool(true)
bool(true)
Compress level 9 (%s)
bool(true)
bool(true)
Uncompress (%s)
bool(true)
bool(true)
bool(true)
