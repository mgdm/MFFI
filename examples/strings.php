<?php

$library = new MFFI\Library();
var_dump($library);

$puts = $library->bind('puts', [ MFFI\Type::TYPE_STRING ], MFFI\Type::TYPE_INT);
$puts('Hello world');

$getenv = $library->bind('getenv', [ MFFI\Type::TYPE_STRING ], MFFI\Type::TYPE_STRING);
var_dump($getenv('USER'));

$strlen = $library->bind('strlen', [ MFFI\Type::TYPE_STRING ], MFFI\Type::TYPE_INT);
var_dump($strlen("Hello world"));
?>
