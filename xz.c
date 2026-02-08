/*
	+----------------------------------------------------------------------+
	| Copyright (c) 2019 The PHP Group                                     |
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
	|          krakjoe (updated for PHP 7)                                 |
	+----------------------------------------------------------------------+
*/

#include <lzma.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "utils.h"
#include "php_xz.h"

#if PHP_VERSION_ID >= 80000
# include "xz_arginfo.h"
#else
# include "xz_legacy_arginfo.h"
#endif

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ INI entries. */
PHP_INI_BEGIN()

	/* Default compression level. Affects `xzencode` and `xzopen`, but only when
	   the level was not specified. */
	PHP_INI_ENTRY("xz.compression_level", "5", PHP_INI_ALL, NULL)

	/* The maximum amount of memory that can be used when decompressing.
	   `0` stands for unlimited. */
	PHP_INI_ENTRY("xz.max_memory", "0", PHP_INI_SYSTEM, NULL)

PHP_INI_END()
/* }}} */

/* {{{ MINIT */
PHP_MINIT_FUNCTION(xz)
{
	REGISTER_INI_ENTRIES();
	php_register_url_stream_wrapper("compress.lzma", &php_stream_xz_wrapper);
	return SUCCESS;
}
/* }}} */

/* {{{ MSHUTDOWN */
PHP_MSHUTDOWN_FUNCTION(xz)
{
	php_unregister_url_stream_wrapper("compress.lzma");
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ MINFO */
PHP_MINFO_FUNCTION(xz)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xz support", "enabled");
	php_info_print_table_row(2, "xz extension version ", PHP_XZ_VERSION);
	if (strcmp(LZMA_VERSION_STRING, lzma_version_string())) {
		php_info_print_table_row(2, "liblzma headers version", LZMA_VERSION_STRING);
		php_info_print_table_row(2, "liblzma library version", lzma_version_string());
	} else {
		php_info_print_table_row(2, "liblzma version", lzma_version_string());
	}
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto resource xzopen(string filename, string mode)
   Opens a file stream. */
PHP_FUNCTION(xzopen)
{
	char *filename = NULL, *mode = NULL;
	zend_long filename_len = 0, mode_len = 0;
	zend_ulong compression_level = INI_INT("xz.compression_level");

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l", &filename, &filename_len, &mode, &mode_len, &compression_level) == FAILURE) {
		return;
	}

	char *mode_to_pass = emalloc(mode_len + 32);
	snprintf(mode_to_pass, mode_len + 32, "%s:%lu", mode, compression_level);

	php_stream *stream = php_stream_xzopen(NULL, filename, mode_to_pass, 0, NULL, NULL STREAMS_CC);

	if (!stream) {
		RETURN_BOOL(0);
	}

	php_stream_to_zval(stream, return_value);
}
/* }}} */

/* {{{ proto string xzencode(string str)
   Retuns the encoded string. */
PHP_FUNCTION(xzencode)
{
	/* The string to be encoded. */
	uint8_t *in = NULL;
	/* The length of the string to be encoded */
	size_t in_len = 0;

	zend_long compression_level = INI_INT("xz.compression_level");

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &in, &in_len, &compression_level) == FAILURE) {
		return;
	}

	if (compression_level < 0 || compression_level > 9) {
#if PHP_VERSION_ID >= 80000
        zend_argument_value_error(2, "must be between 0 and 9");
        RETURN_THROWS();
#else
        php_error_docref(NULL, E_WARNING, "compression level must be between 0 and 9");
        RETURN_BOOL(0);
#endif
	}

	/* The output string (encoded). */
	uint8_t *out = NULL;
	/* The length of the output string. */
	size_t out_len = 0;

	/* Internal buffer used for encoding. */
	uint8_t buff[XZ_BUFFER_SIZE];

	lzma_options_lzma opt_lzma2;
	if (lzma_lzma_preset(&opt_lzma2, compression_level)) {
		RETURN_BOOL(0);
	}

	lzma_filter filters[] = {
		{ .id = LZMA_FILTER_LZMA2, .options = &opt_lzma2 },
		{ .id = LZMA_VLI_UNKNOWN,  .options = NULL },
	};

	/* Initializing encoder. */
	lzma_stream strm = LZMA_STREAM_INIT;
	if (lzma_stream_encoder(&strm, filters, LZMA_CHECK_CRC64) != LZMA_OK) {
		RETURN_BOOL(0);
	}

	/* Setting the input source. */
	strm.avail_in = in_len;
	strm.next_in = in;

	/* Setting the output source. */
	strm.avail_out = XZ_BUFFER_SIZE;
	strm.next_out = buff;

	/* Encoding the string. */
	lzma_ret status = LZMA_OK;
	while (strm.avail_in != 0) {
		status = lzma_code(&strm, LZMA_RUN);
		/* More memory is required. */
		if (strm.avail_out == 0) {
			out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE);
			out_len += XZ_BUFFER_SIZE;
			strm.avail_out = XZ_BUFFER_SIZE;
			strm.next_out = buff;
		}
	}

	/* Finish encoding. */
	while (status != LZMA_STREAM_END) {
		status = lzma_code(&strm, LZMA_FINISH);
		/* An error occured. */
		if ((status != LZMA_STREAM_END) && (status != LZMA_OK)) {
			lzma_end(&strm);
			RETURN_LONG(status);
		}
		/* More memory is required. */
		if (strm.avail_out == 0) {
			out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE);
			out_len += XZ_BUFFER_SIZE;
			strm.avail_out = XZ_BUFFER_SIZE;
			strm.next_out = buff;
		}
	}

	/* Merging last fragment. */
	out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE - strm.avail_out);
	out_len += XZ_BUFFER_SIZE - strm.avail_out;

	lzma_end(&strm);

	RETURN_STRINGL((char *)out, out_len);
}
/* }}} */

/* {{{ proto string xzdecode(string str)
   Retuns the decoded string. */
PHP_FUNCTION(xzdecode)
{
	/* The string to be encoded. */
	uint8_t *in = NULL;
	/* The length of the string to be encoded */
	size_t in_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &in, &in_len) == FAILURE) {
		return;
	}

	if (!in_len) {
		RETURN_BOOL(0); /* empty string is not xz data */
	}

	/* The output string (encoded). */
	uint8_t *out = NULL;
	/* The length of the output string. */
	size_t out_len = 0;

	/* Internal buffer used for encoding. */
	uint8_t buff[XZ_BUFFER_SIZE];

	/* Initializing decoder. */
	lzma_stream strm = LZMA_STREAM_INIT;
	uint64_t mem = INI_INT("xz.max_memory");
	if (lzma_auto_decoder(&strm, mem ? mem : UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK) {
		RETURN_BOOL(0);
	}

	/* Setting the input source. */
	strm.avail_in = in_len;
	strm.next_in = in;

	/* Setting the output source. */
	strm.avail_out = XZ_BUFFER_SIZE;
	strm.next_out = buff;

	/* Decoding the string */
	lzma_ret status = LZMA_OK;
	while (strm.avail_in != 0) {
		status = lzma_code(&strm, LZMA_RUN);
		if (status != LZMA_OK && status != LZMA_STREAM_END) {
			if (out) {
				efree(out);
			}
			lzma_end(&strm);
			RETURN_BOOL(0); /* probably not xz data */
		}

		/* More memory is required. */
		if (strm.avail_out == 0) {
			out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE);
			out_len += XZ_BUFFER_SIZE;
			strm.avail_out = XZ_BUFFER_SIZE;
			strm.next_out = buff;
		}
	}
	/* Merging last fragment. */
	out = memmerge(out, buff, out_len, XZ_BUFFER_SIZE - strm.avail_out);
	out_len += XZ_BUFFER_SIZE - strm.avail_out;

	lzma_end(&strm);

	RETURN_STRINGL((char *)out, out_len);
}
/* }}} */

/* {{{ xz_module_entry */
zend_module_entry xz_module_entry = {
    STANDARD_MODULE_HEADER,
    "xz",
    ext_functions,
    PHP_MINIT(xz),
    PHP_MSHUTDOWN(xz),
    NULL, /* PHP_RINIT(xz) */
    NULL, /* PHP_RSHUTDOWN(xz) */
    PHP_MINFO(xz),
    PHP_XZ_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XZ
#ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(xz)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
