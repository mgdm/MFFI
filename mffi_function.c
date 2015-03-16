#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>
#include <ffi.h>

#include "php.h"
#include "zend_exceptions.h"
#include "php_mffi.h"
#include "mffi_internal.h"

zend_class_entry *mffi_ce_function;

static zend_object_handlers mffi_function_object_handlers;

/* {{{ */
const zend_function_entry mffi_function_methods[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ */
php_mffi_function_object *php_mffi_function_fetch_object(zend_object *obj) {
	return (php_mffi_function_object *)((char*)(obj) - XtOffsetOf(php_mffi_function_object, std));
}
/* }}} */

/* {{{ */
static zend_object *mffi_function_object_new(zend_class_entry *ce)
{
	php_mffi_function_object *object = ecalloc(1, sizeof(php_mffi_function_object) + zend_object_properties_size(ce));
	zend_object_std_init(&object->std, ce);
	object_properties_init(&object->std, ce);
	object->std.handlers = &mffi_function_object_handlers;
	return &object->std;
}
/* }}} */

/* {{{ */
static void mffi_function_object_free_storage(zend_object *object)
{
	php_mffi_function_object *intern = php_mffi_function_fetch_object(object);

	if (!intern) {
		return;
	}

	zend_object_std_dtor(&intern->std);
}
/* }}} */

/* {{{ */
PHP_MINIT_FUNCTION(mffi_function)
{
	zend_class_entry function_ce;

	memcpy(&mffi_function_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	mffi_function_object_handlers.offset = XtOffsetOf(php_mffi_function_object, std);
	mffi_function_object_handlers.free_obj = mffi_function_object_free_storage;
	mffi_function_object_handlers.clone_obj = NULL;

	INIT_NS_CLASS_ENTRY(function_ce, "MFFI", "function", mffi_function_methods);
	function_ce.create_object = mffi_function_object_new;
	mffi_ce_function = zend_register_internal_class(&function_ce);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
