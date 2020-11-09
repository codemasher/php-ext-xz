--TEST--
Test `xzencode` and `xzdecode`: basic functionality.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	print("XZ extension is not loaded!");
}
?>
--FILE--
<?php
$str = "Three Rings for the Elven-kings under the sky,
Seven for the Dwarf-lords in halls of stone,
Nine for Mortal Men, doomed to die,
One for the Dark Lord on his dark throne
In the Land of Mordor where the Shadows lie.
One Ring to rule them all, One Ring to find them,
One Ring to bring them all and in the darkness bind them.
In the Land of Mordor where the Shadows lie";

$encoded = xzencode($str);
$decoded = xzdecode($encoded);

var_dump($str === $decoded);
?>
--EXPECTF--
bool(true)
