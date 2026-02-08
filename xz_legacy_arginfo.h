/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 66a217b3ff195bfd290132e29a53db417487c8db */

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzread, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzwrite, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzclose, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

#define arginfo_xzpassthru arginfo_xzclose

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzencode, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, compression_level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzdecode, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzopen, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, compression_level)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(fread);
ZEND_FUNCTION(fwrite);
ZEND_FUNCTION(fclose);
ZEND_FUNCTION(fpassthru);
ZEND_FUNCTION(xzencode);
ZEND_FUNCTION(xzdecode);
ZEND_FUNCTION(xzopen);

static const zend_function_entry ext_functions[] = {
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzread", zif_fread, arginfo_xzread, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzread", zif_fread, arginfo_xzread, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzwrite", zif_fwrite, arginfo_xzwrite, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzwrite", zif_fwrite, arginfo_xzwrite, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzclose", zif_fclose, arginfo_xzclose, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzclose", zif_fclose, arginfo_xzclose, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzpassthru", zif_fpassthru, arginfo_xzpassthru, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzpassthru", zif_fpassthru, arginfo_xzpassthru, 0)
#endif
	ZEND_FE(xzencode, arginfo_xzencode)
	ZEND_FE(xzdecode, arginfo_xzdecode)
	ZEND_FE(xzopen, arginfo_xzopen)
	ZEND_FE_END
};
