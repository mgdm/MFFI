#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ffi.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_mffi.h"
#include "mffi_internal.h"

zend_class_entry *mffi_ce_exception;

ZEND_DECLARE_MODULE_GLOBALS(mffi)

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mffi)
{
	zend_class_entry exception_ce;

	INIT_NS_CLASS_ENTRY(exception_ce, "MFFI", "Exception", NULL);
	mffi_ce_exception = zend_register_internal_class_ex(&exception_ce, zend_exception_get_default());

	PHP_MINIT(mffi_library)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(mffi_function)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(mffi)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(mffi)
{
#if defined(COMPILE_DL_MFFI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(mffi)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mffi)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mffi", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ mffi_functions[]
 *
 * Every user visible function must have an entry in mffi_functions[].
 */
const zend_function_entry mffi_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ mffi_module_entry
 */
zend_module_entry mffi_module_entry = {
	STANDARD_MODULE_HEADER,
	"mffi",
	mffi_functions,
	PHP_MINIT(mffi),
	PHP_MSHUTDOWN(mffi),
	NULL,
	NULL,
	PHP_MINFO(mffi),
	PHP_MFFI_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MFFI
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(mffi)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
