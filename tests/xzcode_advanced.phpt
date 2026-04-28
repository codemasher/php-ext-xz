--TEST--
Test `xzencode` and `xzdecode`: big inputs.
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

// The size of the chunk that is going to be repeated.
$chunkSize = 1024 * 1024; // 1 MB

// The number of chunks.
$chunkNumber = 8;

// A random chunk of data.
$chunk = '';

// Generating a random chunk.
for($i = 0; $i < $chunkSize; ++$i){
	// adds one random byte
	$chunk .= chr(rand(0, 255));
}

// A random string of size $chunkNumber * $chunkSize.
$str = '';

// Generating a random string.
for($i = 0; $i < $chunkNumber; ++$i){
	$str .= $chunk;
}

var_dump(($chunkSize * $chunkNumber) === strlen($str));
$encoded = xzencode($str);
echo "encoding finished\n";
$decoded = xzdecode($encoded);
echo "decoding finished\n";
var_dump($str === $decoded);

?>
--EXPECTF--
bool(true)
encoding finished
decoding finished
bool(true)
