--TEST--
Test `xzencode`: error conditions.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
if (PHP_VERSION_ID < 80000) {
    die('skip for PHP 8+ only');
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
try {
    var_dump(xzencode($data, $bad_level));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with lower than 0 compression level --\n";
$bad_level = -99;
try {
    var_dump(xzencode($data, $bad_level));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing xzencode() : error conditions ***

-- Testing with larger than 9 compression level --
xzencode(): Argument #2 ($compression_level) must be between 0 and 9

-- Testing with lower than 0 compression level --
xzencode(): Argument #2 ($compression_level) must be between 0 and 9
