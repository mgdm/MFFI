--TEST--
MFFI\Handle::__construct()
--SKIPIF--
if (!extension_loaded('mffi')) die('skip - MFFI extension not available');
--FILE--
<?php
$handle = new MFFI\Handle();
var_dump($handle);
?>
--EXPECTF--
Stuff
