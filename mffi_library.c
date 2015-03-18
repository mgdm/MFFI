#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>
#include <ffi.h>

#include "php.h"
#include "zend_exceptions.h"
#include "php_mffi.h"
#include "mffi_internal.h"

zend_class_entry *mffi_ce_library;

static zend_object_handlers mffi_library_object_handlers;


/* {{{ */
PHP_METHOD(MFFI_Library, __construct)
{
	zend_string *lib_name;
	void *handle;
	php_mffi_library_object *intern;
	zval *self;

	PHP_MFFI_ERROR_HANDLING();
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &lib_name) == FAILURE) {
		PHP_MFFI_RESTORE_ERRORS();
		return;
	}
	PHP_MFFI_RESTORE_ERRORS();

	PHP_MFFI_LIBRARY_FROM_OBJECT(intern, getThis());

	handle = dlopen(lib_name->val, RTLD_LAZY);
	if (!handle) {
		zend_throw_exception(mffi_ce_exception, "Could not open library", 1);
		return;
	}

	intern->handle = handle;
}
/* }}} */

/* {{{ */
PHP_METHOD(MFFI_Library, bind)
{
	zend_string *func_name = NULL;
	zval *args = NULL, *current_arg = NULL;
	HashTable *args_hash = NULL;
	long return_type = 0;
	php_mffi_library_object *intern = NULL;
	php_mffi_function_object *function = NULL;
	zend_long num_key = 0, i = 0;
	zend_string *string_key = NULL;
	void *handle = NULL;
	char *err = NULL;

	PHP_MFFI_ERROR_HANDLING();
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|al", &func_name, &args, &return_value) == FAILURE) {
		PHP_MFFI_RESTORE_ERRORS();
		return;
	}
	PHP_MFFI_RESTORE_ERRORS();

	PHP_MFFI_LIBRARY_FROM_OBJECT(intern, getThis());

	if (!intern->handle) {
		zend_throw_exception(mffi_ce_exception, "Library object is uninitialized", 1);
		return;
	}

	object_init_ex(return_value, mffi_ce_function);
	PHP_MFFI_FUNCTION_FROM_OBJECT(function, return_value);
	handle = dlsym(intern->handle, func_name->val);
	err = dlerror();

	if (err) {
		zend_throw_exception(mffi_ce_exception, err, 1);
		return;
	}

	function->function = handle;

	args_hash = Z_ARRVAL_P(args);
	function->arg_count = zend_hash_num_elements(args_hash);
	function->arg_types = ecalloc(function->arg_count, sizeof(ffi_type));

	ZEND_HASH_FOREACH_VAL(args_hash, current_arg) {
		if (Z_TYPE_P(current_arg) != IS_LONG) {
			zend_throw_exception(mffi_ce_exception, "That wasn't a long, stop it", 1);
			return;
		}

		switch(Z_LVAL_P(current_arg)) {
		case FFI_TYPE_VOID:
			function->arg_types[i] = &ffi_type_void;
			break;

		case FFI_TYPE_INT:
			function->arg_types[i] = &ffi_type_sint;
			break;

		case FFI_TYPE_FLOAT:
			function->arg_types[i] = &ffi_type_float;
			break;

		case FFI_TYPE_DOUBLE:
			function->arg_types[i] = &ffi_type_longdouble;
			break;

		case FFI_TYPE_LONGDOUBLE:
			function->arg_types[i] = &ffi_type_longdouble;
			break;

		case FFI_TYPE_UINT8:
			function->arg_types[i] = &ffi_type_uint8;
			break;

		case FFI_TYPE_SINT8:
			function->arg_types[i] = &ffi_type_sint8;
			break;

		case FFI_TYPE_UINT16:
			function->arg_types[i] = &ffi_type_uint16;
			break;

		case FFI_TYPE_SINT16:
			function->arg_types[i] = &ffi_type_sint16;
			break;

		case FFI_TYPE_UINT32:
			function->arg_types[i] = &ffi_type_uint32;
			break;

		case FFI_TYPE_SINT32:
			function->arg_types[i] = &ffi_type_sint32;
			break;

		case FFI_TYPE_UINT64:
			function->arg_types[i] = &ffi_type_uint64;
			break;

		case FFI_TYPE_SINT64:
			function->arg_types[i] = &ffi_type_sint64;
			break;

		case FFI_TYPE_STRUCT:
		case FFI_TYPE_POINTER:
		default:
			zend_throw_exception(mffi_ce_exception, "Unimplemented type", 1);
			return;
		}

		i++;
	} ZEND_HASH_FOREACH_END();

}
/* }}} */

/* {{{ */
const zend_function_entry mffi_library_methods[] = {
	PHP_ME(MFFI_Library, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(MFFI_Library, bind, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ */
php_mffi_library_object *php_mffi_library_fetch_object(zend_object *obj) {
	return (php_mffi_library_object *)((char*)(obj) - XtOffsetOf(php_mffi_library_object, std));
}
/* }}} */

/* {{{ */
static zend_object *mffi_library_object_new(zend_class_entry *ce)
{
	php_mffi_library_object *object = ecalloc(1, sizeof(php_mffi_library_object) + zend_object_properties_size(ce));
	zend_object_std_init(&object->std, ce);
	object_properties_init(&object->std, ce);
	object->std.handlers = &mffi_library_object_handlers;
	return &object->std;
}
/* }}} */

/* {{{ */
static void mffi_library_object_free_storage(zend_object *object)
{
	php_mffi_library_object *intern = php_mffi_library_fetch_object(object);

	if (!intern) {
		return;
	}

	zend_object_std_dtor(&intern->std);
}
/* }}} */

/* {{{ */
PHP_MINIT_FUNCTION(mffi_library)
{
	zend_class_entry library_ce;

	memcpy(&mffi_library_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	mffi_library_object_handlers.offset = XtOffsetOf(php_mffi_library_object, std);
	mffi_library_object_handlers.free_obj = mffi_library_object_free_storage;
	mffi_library_object_handlers.clone_obj = NULL;

	INIT_NS_CLASS_ENTRY(library_ce, "MFFI", "Library", mffi_library_methods);
	library_ce.create_object = mffi_library_object_new;
	mffi_ce_library = zend_register_internal_class(&library_ce);

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
