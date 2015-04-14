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

/* {{{ PHP_METHOD(MFFI_Func, __construct) */
PHP_METHOD(MFFI_Func, __construct)
{
	zend_throw_exception(mffi_ce_exception, "MFFI\\Func cannot be constructed directly", 1);
}
/* }}} */


static void php_mffi_free_argument(php_mffi_value *arg, long type) {
	if (type == PHP_MFFI_TYPE_STRING) {
		efree(arg->s);
	}
}

/* {{{ */
PHP_METHOD(MFFI_Func, __invoke)
{
	zval *args = NULL;
	long arg_count = 0, i = 0;
	php_mffi_function_object *intern;
	php_mffi_value ret_value, *values;
	php_mffi_struct_object *obj;
	void **arguments;

	PHP_MFFI_ERROR_HANDLING();
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &arg_count) == FAILURE) {
		PHP_MFFI_RESTORE_ERRORS();
		return;
	}
	PHP_MFFI_RESTORE_ERRORS();

	PHP_MFFI_FUNCTION_FROM_OBJECT(intern, getThis());

	if (arg_count != intern->arg_count) {
		zend_throw_exception(mffi_ce_exception, "Incorrect argument count", 1);
		return;
	}

	php_printf("Arg count: %d\n", arg_count);
	values = (php_mffi_value *) ecalloc(arg_count, sizeof(php_mffi_value));
	arguments = (void **) ecalloc(arg_count, sizeof(void *));

	for (i = 0; i < arg_count; i++) {
		switch (Z_TYPE(args[i])) {
			case IS_OBJECT:
				obj = php_mffi_struct_fetch_object(Z_OBJ(args[i]));
				arguments[i] = obj->data;
				break;

			default:
				php_mffi_set_argument(&args[i], &values[i], intern->php_arg_types[i]);
				arguments[i] = &values[i];
				break;
		}
	}

	ffi_call(&intern->cif, intern->function, &ret_value, arguments);
	php_printf("Ret val: %p %p\n", &ret_value, ret_value.p);

	php_mffi_set_return_value(return_value, &ret_value, intern->php_return_type);

	/* Free the string arguments */
	for (i = 0; i < arg_count; i++) {
	//	php_mffi_free_argument(&values[i], intern->php_arg_types[i]);
	}

	efree(values);
	efree(arguments);
}
/* }}} */

/* {{{ */
const zend_function_entry mffi_function_methods[] = {
	PHP_ME(MFFI_Func, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR|ZEND_ACC_FINAL)
	PHP_ME(MFFI_Func, __invoke, NULL, ZEND_ACC_PUBLIC)
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

	if (intern->ffi_arg_types) {
		efree(intern->ffi_arg_types);
	}

	if (intern->php_arg_types) {
		efree(intern->php_arg_types);
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

	INIT_NS_CLASS_ENTRY(function_ce, "MFFI", "Func", mffi_function_methods);
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
