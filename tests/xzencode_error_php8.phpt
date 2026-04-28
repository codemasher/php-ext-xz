--TEST--
Test `xzencode`: error conditions.
--SKIPIF--
<?php
if(!extension_loaded('xz')){
	exit('skip XZ extension is not loaded!');
}
if(PHP_VERSION_ID < 80000){
    exit('skipped: for PHP 8+ only');
}
?>
--FILE--
<?php

echo "*** Testing xzencode() : error conditions ***\n";

$data          = 'string_val';
$level         = 2;
$encoding_mode = FORCE_DEFLATE;

$bad_level = 99;
printf("\n-- Testing with larger than 9 compression level (%d) --\n", $bad_level);

try{
	var_dump(xzencode($data, $bad_level));
}
catch(\ValueError $e){
	printf("%s\n", $e->getMessage());
}

$bad_level = -99;
printf("\n-- Testing with lower than 0 compression level (%d) --\n", $bad_level);

try{
	var_dump(xzencode($data, $bad_level));
}
catch(\ValueError $e){
	printf("%s\n", $e->getMessage());
}

?>
--EXPECT--
*** Testing xzencode() : error conditions ***

-- Testing with larger than 9 compression level (99) --
xzencode(): Argument #2 ($compression_level) must be between 0 and 9

-- Testing with lower than 0 compression level (-99) --
xzencode(): Argument #2 ($compression_level) must be between 0 and 9
