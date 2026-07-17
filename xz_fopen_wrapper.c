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

#include "php.h"
#include "php_streams.h"
#include "fopen_wrappers.h"
#include "php_xz.h"

/* {{{ php_xz_stream_data_t */
struct php_xz_stream_data_t {

	/* LZMA stream used internally. */
	lzma_stream strm;

	/* The size of the input buffer. */
	size_t in_buf_sz;

	/* The size of the output buffer. */
	size_t out_buf_sz;

	/* The input buffer. */
	uint8_t *in_buf;

	/* The output buffer. */
	uint8_t *out_buf;

	/* The index of the output buffer.
	   NOTE: This is used only for decompressing. */
	uint8_t *out_buf_idx;

	/* The PHP stream. */
	php_stream *stream;

	/* The file descriptor. */
	int fd;

	/* The type of access required. */
	char mode[64];

	/* Compression level used. */
	unsigned long level;

	/* The maximum amount of memory that can be used when decompressing. */
	uint64_t memory;
};
/* }}} */

/* {{{ php_xz_decompress
   Decompresses the stream.
   Returns 0 on success, -1 on error
*/
static int php_xz_decompress(struct php_xz_stream_data_t *self)
{
	lzma_stream *strm = &self->strm;
	lzma_action action = LZMA_RUN;
	lzma_ret ret;

	if (strm->avail_in == 0 && !php_stream_eof(self->stream)) {
#if PHP_VERSION_ID >= 70400
		ssize_t read = php_stream_read(self->stream, (char *)self->in_buf, self->in_buf_sz);
		if (read < 0) {
			return -1;
		}
		strm->avail_in = read;
#else
		strm->avail_in = php_stream_read(self->stream, (char *)self->in_buf, self->in_buf_sz);
#endif
		strm->next_in = self->in_buf;
	}

	ret = lzma_code(strm, action);
	if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
		return -1;
	}

	return 0;
}
/* }}} */

/* {{{ php_xz_compress
   Compresses the stream by consuming all bytes in `lzma_stream->next_in` and
   writing them into the file.
   Returns the number of bytes to be written or -1 on error
*/
static int php_xz_compress(struct php_xz_stream_data_t *self)
{
	lzma_stream *strm = &self->strm;
	lzma_action action = LZMA_RUN;
	int to_write = strm->avail_in;

	while (strm->avail_in > 0) {
		lzma_ret ret = lzma_code(strm, action);
		size_t len = self->out_buf_sz - strm->avail_out;
		if (ret != LZMA_OK) {
			to_write = -1; /* error in compression */
			break;
		} else if (len) {
			if (php_stream_write(self->stream, (char *)self->out_buf, len) != len) {
				to_write = -1; /* error in output stream */
				break;
			}
		}
		strm->next_out = self->out_buf;
		strm->avail_out = self->out_buf_sz;
	}

	strm->next_in = self->in_buf;

	return to_write;
}
/* }}} */

/* {{{ php_xz_init_decoder
   Initializes a decoder. */
static int php_xz_init_decoder(struct php_xz_stream_data_t *self)
{
	lzma_stream *strm = &self->strm;

	if (lzma_auto_decoder(strm, self->memory ? self->memory : UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK) {
		return 0;
	}

	self->in_buf_sz = XZ_BUFFER_SIZE;
	self->in_buf = emalloc(self->in_buf_sz);
	strm->avail_in = 0;
	strm->next_in = self->in_buf;

	self->out_buf_sz = XZ_BUFFER_SIZE;
	self->out_buf = emalloc(self->out_buf_sz);
	self->out_buf_idx = self->out_buf;
	strm->avail_out = self->out_buf_sz;
	strm->next_out = self->out_buf;

	return 1;
}
/* }}} */

/* {{{ php_xz_init_encoder
   Initializes an encoder. */
static int php_xz_init_encoder(struct php_xz_stream_data_t *self)
{
	lzma_stream *strm = &self->strm;

	lzma_options_lzma opt_lzma2;
	if (lzma_lzma_preset(&opt_lzma2, self->level)) {
		return 0;
	}

	lzma_filter filters[] = {
		{ .id = LZMA_FILTER_LZMA2, .options = &opt_lzma2 },
		{ .id = LZMA_VLI_UNKNOWN,  .options = NULL },
	};

	lzma_ret ret = lzma_stream_encoder(strm, filters, LZMA_CHECK_CRC64);

	if (ret != LZMA_OK) {
		return 0;
	}

	self->in_buf_sz = XZ_BUFFER_SIZE;
	self->in_buf = emalloc(self->in_buf_sz);
	strm->avail_in = 0;
	strm->next_in = self->in_buf;

	self->out_buf_sz = XZ_BUFFER_SIZE;
	self->out_buf = emalloc(self->out_buf_sz);
	strm->avail_out = self->out_buf_sz;
	strm->next_out = self->out_buf;

	return 1;
}
/* }}} */

/* {{{ php_xziop_read
   Reads from the stream. */
#if PHP_VERSION_ID >= 70400
static ssize_t php_xziop_read(php_stream *stream, char *buf, size_t count)
#else
static size_t php_xziop_read(php_stream *stream, char *buf, size_t count)
#endif
{
	struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
	lzma_stream *strm = &self->strm;


	size_t to_read = count, have_read = 0;

	while (to_read > 0) {
		if (to_read < strm->next_out - self->out_buf_idx) {
			memcpy(buf + have_read, self->out_buf_idx, to_read);
			self->out_buf_idx += to_read;
			have_read += to_read;
			break;
		} else if (strm->next_out - self->out_buf_idx > 0) {
			memcpy(buf + have_read, self->out_buf_idx, strm->next_out - self->out_buf_idx);
			have_read += strm->next_out - self->out_buf_idx;
			to_read -= strm->next_out - self->out_buf_idx;

			if (strm->avail_out) {
				self->out_buf_idx = strm->next_out;
			} else {
				/* All bytes in the output buffer have been read. */
				self->out_buf_idx = strm->next_out;
				self->out_buf_idx = strm->next_out = self->out_buf;
				strm->avail_out = self->out_buf_sz;
			}
		}

		if (self->out_buf_idx == strm->next_out && php_stream_eof(self->stream) && strm->avail_in == 0) {
			stream->eof = 1;
			return have_read;
		}


		if (php_xz_decompress(self) < 0) {
#if PHP_VERSION_ID >= 70400
			if (!have_read) {
				return -1;
			}
#endif
			break;
		}
	}

	return have_read;
}
/* }}} */

/* {{{ php_xziop_write
   Writes to the stream. */
#if PHP_VERSION_ID >= 70400
static ssize_t php_xziop_write(php_stream *stream, const char *buf, size_t count)
#else
static size_t php_xziop_write(php_stream *stream, const char *buf, size_t count)
#endif
{
	struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
	size_t wrote = 0, bytes_consumed = 0;

	lzma_stream *strm = &self->strm;

	while (count - wrote > self->in_buf_sz - strm->avail_in) {
		/* Copy as many bytes into next_in buffer as we can. */
		memcpy((char *)self->in_buf + strm->avail_in, buf + wrote, self->in_buf_sz - strm->avail_in);
		wrote += self->in_buf_sz - strm->avail_in;
		strm->avail_in = self->in_buf_sz;
		bytes_consumed = php_xz_compress(self);
		if (bytes_consumed < 0) {
			break;
		}
	}

	if (count - wrote > 0) {
		memcpy((char *) self->in_buf + strm->avail_in, buf + wrote, count - wrote);
		strm->avail_in += count - wrote;
	}

#if PHP_VERSION_ID >= 70400
	return (bytes_consumed < 0 ? -1 : count);
#else
	return (bytes_consumed < 0 ? 0 : count);
#endif
}
/* }}} */

/* {{{ php_xziop_close
   Closes the stream. */
static int php_xziop_close(php_stream *stream, int close_handle)
{
	struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
	int ret = EOF;

	lzma_stream *strm = &self->strm;

	/* In write mode, finish writing LZMA data before continuing. */
	if (strcmp(self->mode, "w") == 0 || strcmp(self->mode, "wb") == 0) {
		lzma_ret lz_ret;

		do {
			strm->next_out = self->out_buf;
			strm->avail_out = self->out_buf_sz;
			lzma_action action = LZMA_FINISH;
			lz_ret = lzma_code(strm, action);

			if (strm->avail_out < self->out_buf_sz) {
				size_t write_size = self->out_buf_sz - strm->avail_out;
				php_stream_write(self->stream, (char *)self->out_buf, write_size);
				strm->next_out = self->out_buf;
				strm->avail_out = self->out_buf_sz;
			}

		} while (lz_ret == LZMA_OK);
	}

	lzma_end(&self->strm);

	if (self->stream) {
		php_stream_free(self->stream, PHP_STREAM_FREE_CLOSE | (close_handle == 0 ? PHP_STREAM_FREE_PRESERVE_HANDLE : 0));
	}

	efree(self->in_buf);
	efree(self->out_buf);
	efree(self);

	return ret;
}
/* }}} */

/* {{{ php_xziop_flush
   Flushes the stream. */
static int php_xziop_flush(php_stream *stream)
{
	struct php_xz_stream_data_t *self = (struct php_xz_stream_data_t *) stream->abstract;
	if (strcmp(self->mode, "w") == 0 || strcmp(self->mode, "wb") == 0) {
		php_xz_compress(self);
	}
	php_stream_flush(self->stream);
	return 0;
}
/* }}} */

/* {{{ php_stream_xzio_ops
   Table containing basic I/O operations and mapped functions. */
php_stream_ops php_stream_xzio_ops = {
	php_xziop_write, /* write */
	php_xziop_read, /* read */
	php_xziop_close, /* close */
	php_xziop_flush, /* flush */
	"XZ",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL,  /* set_option */
	.label = "XZ",
	.write = php_xziop_write,
	.read = php_xziop_read,
	.close = php_xziop_close,
	.flush = php_xziop_flush,
	.seek = NULL,
	.cast = NULL,
	.stat = NULL,
	.set_option = NULL
};
/* }}} */

/* {{{ php_stream_xzopen
   Opens a stream. */
php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, const char *path, const char *mode_pass, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	char mode[64];
	zend_ulong level = zend_ini_long_literal("xz.compression_level");;
	zend_ulong mem = zend_ini_long_literal("xz.max_memory");

	php_stream *stream = NULL, *innerstream = NULL;

	strncpy(mode, mode_pass, sizeof(mode));
	mode[sizeof(mode) - 1] = '\0';

	/* Split compression level and mode. */
	char *colonp = strchr(mode, ':');
	if (colonp) {
		level = strtoul(colonp + 1, NULL, 10);
		*colonp = '\0';
	}

	if ((strchr(mode, '+')) || ((strchr(mode, 'r')) && (strchr(mode, 'w')))) {
		php_error_docref(NULL, E_ERROR, "cannot open xz stream for reading and writing at the same time.");
		return NULL;
	}

	if ((level < 0) || (level > 9)) {
		php_error_docref(NULL, E_ERROR, "Invalid compression level");
		return NULL;
	}

	if (strncasecmp("compress.lzma://", path, 16) == 0) {
		path += 16;
	}

	if (context) {
		zval *tmpzval;
		if (NULL != (tmpzval = php_stream_context_get_option(context, "xz", "compression_level"))) {
			level = zval_get_long(tmpzval);
		}
		if (NULL != (tmpzval = php_stream_context_get_option(context, "xz", "max_memory"))) {
			mem = zval_get_long(tmpzval);
		}
	}

	innerstream = php_stream_open_wrapper_ex(path, mode, STREAM_MUST_SEEK | options | STREAM_WILL_CAST, opened_path, context);

	if (innerstream) {
		int fd;
		if (php_stream_cast(innerstream, PHP_STREAM_AS_FD, (void **) &fd, REPORT_ERRORS) == SUCCESS) {
			struct php_xz_stream_data_t *self = ecalloc(1, sizeof(struct php_xz_stream_data_t));
			self->stream = innerstream;
			self->fd = fd;
			self->level = level;
			self->memory = mem;
			strncpy(self->mode, mode, sizeof(self->mode));
			stream = php_stream_alloc_rel(&php_stream_xzio_ops, self, 0, mode);

			if (stream) {
				stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
				if ((strcmp(mode, "w") == 0) || (strcmp(mode, "wb") == 0)) {
					if (!php_xz_init_encoder(self)) {
						php_error_docref(NULL, E_WARNING, "Could not initialize xz encoder.");
						efree(self);
						php_stream_close(stream);
						return NULL;
					}
				} else if ((strcmp(mode, "r") == 0) || (strcmp(mode, "rb") == 0)) {
					if (!php_xz_init_decoder(self)) {
						php_error_docref(NULL, E_WARNING, "Could not initialize xz decoder");
						efree(self);
						php_stream_close(stream);
						return NULL;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "Can only open in read (r) or write (w) mode.");
					efree(self);
					php_stream_close(stream);
					return NULL;
				}
				return stream;
			}
			efree(self);
			php_error_docref(NULL, E_WARNING, "failed opening xz stream");
		}
		php_stream_close(innerstream);
	}

	return NULL;
}
/* }}} */

/* {{{ xz_stream_wops
   Table containing stream operations and mapped functions. */
static php_stream_wrapper_ops xz_stream_wops = {
	php_stream_xzopen, /* open */
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"XZ",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};
/* }}} */

/* {{{ php_stream_xz_wrapper
   Table describing the wrapper. */
php_stream_wrapper php_stream_xz_wrapper = {
	&xz_stream_wops, /* operations that can be performed */
	NULL, /* context for the wrapper */
	0     /* is_url */
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
