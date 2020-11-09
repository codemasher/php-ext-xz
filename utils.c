/*
	+----------------------------------------------------------------------+
	| PHP Version 5                                                        |
	+----------------------------------------------------------------------+
	| Copyright (c) 1997-2015 The PHP Group                                |
	+----------------------------------------------------------------------+
	| This source file is subject to version 3.01 of the PHP license,      |
	| that is bundled with this package in the file LICENSE, and is        |
	| available through the world-wide-web at the following url:           |
	| http://www.php.net/license/3_01.txt                                  |
	| If you did not receive a copy of the PHP license and are unable to   |
	| obtain it through the world-wide-web, please send a note to          |
	| license@php.net so we can mail you a copy immediately.               |
	+----------------------------------------------------------------------+
	| Authors: Payden Sutherland <payden@paydensutherland.com>             |
	|          Dan Ungureanu <udan1107@gmail.com>                          |
	|          authors of the `zlib` extension (for guidance)              |
	+----------------------------------------------------------------------+
*/

#include "php.h"

void *memmerge(char *ptr1, char *ptr2, size_t len1, size_t len2) /* {{{ */
{
	if ((ptr2 == NULL) || (len2 < 1)) {
		return ptr1;
	}
	ptr1 = erealloc(ptr1, len1 + len2);
	if (ptr1 == NULL) {
		return NULL;
	}
	memcpy(ptr1 + len1, ptr2, len2);
	return ptr1;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
