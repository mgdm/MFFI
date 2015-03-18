<?php
/* The lib might be libc.so on Linux */
$lib = new MFFI\Library('libSystem.B.dylib');
$sqrt = $lib->bind('sqrt', [ MFFI\Type::TYPE_DOUBLE ], MFFI\Type::TYPE_DOUBLE);

var_dump($sqrt);
var_dump($sqrt(9));

