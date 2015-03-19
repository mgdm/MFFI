# MFFI

A new foreign-function interface for PHP.

## Example

```php
<?php
$handle = new MFFI\Library('libSystem.B.dylib');
$sqrt = $handle->bind('sqrt', [MFFI\Type::TYPE_DOUBLE], MFFI\Type::TYPE_DOUBLE);
var_dump($sqrt(9)); // -> float(3.0)
```

## A word of warning

This extension is full of ways in which you can shoot yourself in the foot, except it'll take your entire leg off. It's like running with scissors, except the scissors are made of knives, and some of the knives are on fire. Be careful.
