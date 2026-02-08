--TEST--
Test `xzencode`: error conditions.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
if (PHP_VERSION_ID >= 80000) {
    die('skip for PHP 7 only');
}
?>
--FILE--
<?php
echo "*** Testing xzencode() : error conditions ***\n";

$data = 'string_val';
$level = 2;
$encoding_mode = FORCE_DEFLATE;

echo "\n-- Testing with larger than 9 compression level --\n";
$bad_level = 99;

var_dump(xzencode($data, $bad_level));

echo "\n-- Testing with lower than 0 compression level --\n";
$bad_level = -99;

var_dump(xzencode($data, $bad_level));

?>
--EXPECTF--
*** Testing xzencode() : error conditions ***

-- Testing with larger than 9 compression level --

Warning: xzencode(): compression level must be between 0 and 9 in %s on line %d
bool(false)

-- Testing with lower than 0 compression level --

Warning: xzencode(): compression level must be between 0 and 9 in %s on line %d
bool(false)