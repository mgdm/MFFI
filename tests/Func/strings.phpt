--TEST--
MFFI\Func::__construct()
--SKIPIF--
if (!extension_loaded('mffi')) die('skip - MFFI extension not available');
--FILE--
<?php
$library = new MFFI\Library();
var_dump($library);

/* Puts accepts a string and returns an int */
$puts = $library->bind('puts', [ MFFI\Type::TYPE_STRING ], MFFI\Type::TYPE_INT);
$puts('Hello world');
?>
--EXPECTF--
object(MFFI\Library)#%d (0) {
}
Hello world
