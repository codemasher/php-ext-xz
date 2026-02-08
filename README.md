# php-ext-xz

PHP Extension providing XZ (LZMA2) compression/decompression functions.<br/>
(see [Implement lzma (xz?) compression](https://news-web.php.net/php.internals/106654))

[![Linux build](https://github.com/codemasher/php-ext-xz/workflows/Linux/badge.svg)](https://github.com/codemasher/php-ext-xz/actions/workflows/linux.yml)
[![Windows PHP8 build](https://github.com/codemasher/php-ext-xz/workflows/Windows/badge.svg)](https://github.com/codemasher/php-ext-xz/actions/workflows/windows.yml)

## Installation

The recommended way to install [the extension](https://packagist.org/packages/codemasher/php-ext-xz) is using [PIE](https://github.com/php/pie):

```bash
pie install codemasher/php-ext-xz
```

Windows builds are now done automatically; you can download them from the  [releases](https://github.com/codemasher/php-ext-xz/releases).
Copy the dll file into the `/ext` directory of your PHP installation and add the line `extension=xz-1.2.0-8.5-ts-vs17-x86_64` to your `php.ini` (whatever the filename may be, you may omit the leading "php_" and the extension), se also: [Loading an extension](https://www.php.net/manual/en/install.pecl.windows.php#install.pecl.windows.loading) in the PHP manual.

You can check if the extension is loaded via `phpinfo()`, or from within PHP via:
```php
if(!extension_loaded('xz')){
	throw new Exception('ext-xz not loaded!');
}

// ...continue to do stuff with ext-xz...
```

## Basic usage

### String-based operations

You can easily compress and decompress strings.

```php
$string = 'This is a test string that will be compressed and then decompressed.';

// Compress a string
$compressed = xzencode($string);

// Decompress a string
$decompressed = xzdecode($compressed);
```

### File-based operations

The extension also supports stream-based operations for working with `.xz` files.

```php
$file = '/tmp/test.xz';

// Writing to an .xz file
$wh = xzopen($file, 'w');
xzwrite($wh, 'Data to write');
xzclose($wh);

// Reading from an .xz file and outputting its contents
$rh = xzopen($file, 'r');
xzpassthru($rh);
xzclose($rh);
```


## Configuration

You can configure the default compression level and memory limit:

```ini

; Default compression level. Affects `xzencode` and `xzopen`, 
; but only when the level was not specified. Values 0-9, default is 5.
xz.compression_level=5

; The maximum amount of memory that can be used when decompressing. Default is 0 (no limit).
xz.max_memory=65536
```

Alternatively, the compression level can be supplied as a parameter to the `xzencode()` and `xzopen()` functions:

```php
const COMPRESSION_LEVEL = 7;

$compressed = xzencode($string, COMPRESSION_LEVEL);

$rh = xzopen($file, 'w', COMPRESSION_LEVEL);
```


## Build from source

### Linux

This module requires [`liblzma-dev`](https://packages.ubuntu.com/search?lang=de&keywords=liblzma-dev&searchon=names) (https://tukaani.org/xz/) as well as php7-dev or php8-dev.
If you are using Ubuntu, you can easily install all of them by typing the following command in your terminal:
```bash
sudo apt-get install git php7.4-dev liblzma-dev
```
To build and install as module, perform the following steps:
```bash
git clone https://github.com/codemasher/php-ext-xz.git
cd php-ext-xz
phpize
./configure
make
sudo make install
```

Do not forget to add `extension=xz.so` to your `php.ini`.

### Windows

If you want to build it on your own, follow the steps under "[Build your own PHP on Windows](https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2)" to setup your build environment.
Before the compilation step, clone this repository to `[...]\php-src\ext\xz` and proceed.

```bat
git clone https://github.com/Microsoft/php-sdk-binary-tools.git c:\php-sdk
cd c:\php-sdk
phpsdk-vs16-x64.bat
```
Run the buildtree script and check out the php source:
```bat
phpsdk_buildtree php-8.0
git clone https://github.com/php/php-src.git
cd php-src
git checkout PHP-8.0
```
Clone the xz extension and run the build:
```bat
git clone https://github.com/codemasher/php-ext-xz .\ext\xz
phpsdk_deps -u
buildconf --force
configure --enable-xz
nmake snap
```

Please note that the `liblzma` dependency is not included with PHP < 8, so you will need to [download it manually](https://windows.php.net/downloads/php-sdk/deps/vs16/x64/liblzma-5.2.5-vs16-x64.zip) and extract it into the `deps` directory.


## Disclaimer
May or may not contain bugs. Use at your own risk.
