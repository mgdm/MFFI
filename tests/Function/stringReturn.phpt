--TEST--
MFFI\Function::__construct()
--SKIPIF--
if (!extension_loaded('mffi')) die('skip - MFFI extension not available');
--FILE--
<?php
$library = new MFFI\Library();
var_dump($library);

/* getenv accepts a string and returns a different string */
$getenv = $library->bind('getenv', [ MFFI\Type::TYPE_STRING ], MFFI\Type::TYPE_STRING);
var_dump($getenv('USER'));

?>
--EXPECTF--
object(MFFI\Library)#1 (0) {
}
string(%d) "%s"
