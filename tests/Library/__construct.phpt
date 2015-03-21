--TEST--
MFFI\Library::__construct()
--SKIPIF--
if (!extension_loaded('mffi')) die('skip - MFFI extension not available');
--FILE--
<?php

/* No params means bind to the PHP process */
$handle = new MFFI\Library();
var_dump($handle);

try {
$handle = new MFFI\Library('Nonsense');
} catch (MFFI\Exception $e) {
    echo "Caught exception: ", $e->getMessage();
}

?>
--EXPECTF--
object(MFFI\Library)#1 (0) {
}
Caught exception: Could not open library
