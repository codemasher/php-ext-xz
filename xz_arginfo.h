/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 66a217b3ff195bfd290132e29a53db417487c8db */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzread, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzwrite, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xzclose, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzpassthru, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzencode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compression_level, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzdecode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xzopen, 0, 2, resource, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, compression_level, IS_LONG, 0)
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
