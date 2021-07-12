--TEST--
Test lzma stream
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
$tmp1 = tempnam(sys_get_temp_dir(), "LZMA");
$tmp2 = tempnam(sys_get_temp_dir(), "LZMA");
$tmp3 = tempnam(sys_get_temp_dir(), "LZMA");

$len0 = filesize(PHP_BINARY);

echo "Compress level 2 ($tmp1)\n";
$opts = [
	'xz' => [
		'compression_level' => '2',
		'max_memory' => 64*1024*1024,
	]
];
$ctx = stream_context_create($opts);
copy(PHP_BINARY, "compress.lzma://$tmp1");
$len1 = filesize($tmp1);
var_dump($len1 > 0);
var_dump($len1 <= $len0);

echo "Compress level 9 ($tmp2)\n";
$opts = [
	'xz' => [
		'compression_level' => '9',
		'max_memory' => 64*1024*1024,
	]
];
$ctx = stream_context_create($opts);
copy(PHP_BINARY, "compress.lzma://$tmp2", $ctx);
$len2 = filesize($tmp2);
var_dump($len2 > 0);
var_dump($len2 <= $len1);

echo "Uncompress ($tmp3)\n";
copy("compress.lzma://$tmp1", $tmp3);
$len3 = filesize($tmp3);
var_dump($len3 > 0);
var_dump($len3 == $len0);

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
