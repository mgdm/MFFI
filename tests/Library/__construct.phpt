--TEST--
MFFI\Library::__construct()
--SKIPIF--
if (!extension_loaded('mffi')) die('skip - MFFI extension not available');
--FILE--
<?php
$handle = new MFFI\Library();
var_dump($handle);
?>
--EXPECTF--
Stuff
