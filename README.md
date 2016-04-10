# MFFI

A new foreign-function interface for PHP.

## Example

```php
<?php
$handle = new MFFI\Library('libSystem.B.dylib');
$sqrt = $handle->bind('sqrt', [MFFI\Type::TYPE_DOUBLE], MFFI\Type::TYPE_DOUBLE);
var_dump($sqrt(9)); // -> float(3.0)
```

## Installing MFFI on OS X

```
$ brew install libffi
$ git clone https://github.com/mgdm/MFFI
$ cd MFFI
$ phpize
$ LIBFFI_PATH=$(brew --prefix libffi) LDFLAGS=" -L${LIBFFI_PATH}/lib" PKG_CONFIG_PATH="$PKG_CONFIG_PATH:${LIBFFI_PATH}/lib/pkgconfig" ./configure --with-php-config=/usr/local/bin/php-config
$ make && make install
```

If everything goes well, you should see no errors and one line of output that looks something like this:

```
Installing shared extensions: /usr/local/Cellar/php70/7.0.5/lib/php/extensions/no-debug-non-zts-20151012/
```

Find your loaded `php.ini` file using `php --info | grep php.ini` and add the following two lines to this file:

```
# The path copied from above
extension_dir=/usr/local/Cellar/php70/7.0.5/lib/php/extensions/no-debug-non-zts-20151012/
extension=mffi.so
```

A successful installation can be verified by running:

```
php --info | grep "mffi => enabled"
```

If you get the output `mffi => enabled`, you're all set!

## A word of warning

This extension is full of ways in which you can shoot yourself in the foot, except it'll take your entire leg off. It's like running with scissors, except the scissors are made of knives, and some of the knives are on fire. Be careful.
