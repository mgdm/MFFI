<?php

use MFFI\Library;
use MFFI\Type;
use MFFI\Struct;

Struct::define('TimeVal', [
    'tv_sec' => Type::TYPE_INT,
    'tv_usec' => Type::TYPE_INT
]);

Struct::define('TimeZone', [
    'tz_minuteswest' => Type::TYPE_INT,
    'tz_dsttime' => Type::TYPE_INT
]);

$lib = new Library();
$gettimeofday = $lib->bind('gettimeofday', [ TimeVal::class, TimeZone::class ], Type::TYPE_INT);

$tp = new TimeVal();
$tzp = new TimeZone();
var_dump($gettimeofday($tp, $tzp));
var_dump($tp, $tzp);
