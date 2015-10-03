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
    echo "Caught exception: ", $e->getMessage(), "\n";
}

switch (php_uname('s')) {
    case "Darwin":
        $handle = new MFFI\Library('libSystem.B.dylib');
        break;
    case "Linux":
        $handle = new MFFI\Library('libc.so.6');
        break;
}

var_dump($handle);

?>
--EXPECTF--
object(MFFI\Library)#%d (0) {
}
Caught exception: Could not open library Nonsense
object(MFFI\Library)#%d (0) {
}
