<?php
use MFFI\Type;

class TimeStruct extends MFFI\Struct
{
    static function definition()
    {
        return [
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
        ];
    }
}

$tm = new TimeStruct();

$tm->tm_sec = 0;
$tm->tm_min = 30;
$tm->tm_hour = 15;
$tm->tm_mday = 5;
$tm->tm_mon = 3;
$tm->tm_year = 115;
$tm->tm_zone = "BST";
var_dump($tm);

$lib = new MFFI\Library();
$asctime = $lib->bind('asctime', [ TimeStruct::class ], MFFI\Type::TYPE_STRING);
var_dump($asctime($tm));

