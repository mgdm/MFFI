<?php
use MFFI\Type;

MFFI\Struct::define('TimeStruct', [
    'tm_sec' => Type::TYPE_INT,     /* seconds (0 - 60) */
    'tm_min' => Type::TYPE_INT,     /* minutes (0 - 59) */
    'tm_hour' => Type::TYPE_INT,    /* hours (0 - 23) */
    'tm_mday' => Type::TYPE_INT,    /* day of month (1 - 31) */
    'tm_mon' => Type::TYPE_INT,     /* month of year (0 - 11) */
    'tm_year' => Type::TYPE_INT,    /* year - 1900 */
    'tm_wday' => Type::TYPE_INT,    /* day of week (Sunday = 0) */
    'tm_yday' => Type::TYPE_INT,    /* day of year (0 - 365) */
    'tm_isdst' => Type::TYPE_INT,   /* is summer time in effect? */
    'tm_zone' => Type::TYPE_STRING,  /* abbreviation of timezone name */
//    'tm_gmtoff' => Type::TYPE_INT, /* offset from UTC in seconds */
]);

$tm = new TimeStruct();
var_dump($tm);
var_dump("tm_sec", $tm->tm_sec);
var_dump("tm_isdst", $tm->tm_isdst);
var_dump("doesnotexist", $tm->doesnotexist);
var_dump("isset tm_sec", isset($tm->tm_sec));
var_dump("isset nope", isset($tm->nope));
var_dump("empty tm_sec", empty($tm->tm_sec));
var_dump("empty nope", empty($tm->nope));

$tm->tm_sec = 0;
$tm->tm_min = 30;
$tm->tm_hour = 15;
$tm->tm_mday = 5;
$tm->tm_mon = 3;
$tm->tm_year = 115;
//$tm->tm_zone = "BST";
var_dump($tm);
//var_dump($tm->tm_zone);

$lib = new MFFI\Library();
$asctime = $lib->bind('asctime', [ TimeStruct::class ], MFFI\Type::TYPE_STRING);
var_dump($asctime);
var_dump($asctime($tm));
