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
	zend_string *lib_name = NULL;
	void *handle = NULL;
	php_mffi_library_object *intern = NULL;

	PHP_MFFI_ERROR_HANDLING();
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &lib_name) == FAILURE) {
		PHP_MFFI_RESTORE_ERRORS();
		return;
	}
	PHP_MFFI_RESTORE_ERRORS();

	intern = php_mffi_library_fetch_object(Z_OBJ_P(getThis()));

	if (lib_name != NULL) {
		handle = dlopen(lib_name->val, RTLD_LAZY);
	} else {
		handle = dlopen(NULL, RTLD_LAZY);
	}

	if (!handle) {
		if (lib_name != NULL) {
			zend_throw_exception_ex(mffi_ce_exception, 0, "Could not open library %s", lib_name->val);
		} else {
			zend_throw_exception(mffi_ce_exception, "Could not open library", 0);
		}

		return;
	}

	intern->handle = handle;
}
/* }}} */

/* {{{ */
PHP_METHOD(MFFI_Library, bind)
{
	zend_string *func_name = NULL;
	zval *args = NULL, *current_arg = NULL, *return_type = NULL;
	HashTable *args_hash = NULL;
	php_mffi_library_object *intern = NULL;
	php_mffi_function_object *function = NULL;
	php_mffi_struct_definition *def = NULL;
	zend_long i = 0;
	ffi_status status;
	void *handle = NULL;
	char *err = NULL;

	PHP_MFFI_ERROR_HANDLING();
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|az", &func_name, &args, &return_type) == FAILURE) {
		PHP_MFFI_RESTORE_ERRORS();
		return;
	}
	PHP_MFFI_RESTORE_ERRORS();

	intern = php_mffi_library_fetch_object(Z_OBJ_P(getThis()));

	if (!intern->handle) {
		zend_throw_exception(mffi_ce_exception, "Library object is uninitialized", 1);
		return;
	}

	object_init_ex(return_value, mffi_ce_function);
	function = php_mffi_function_fetch_object(Z_OBJ_P(return_value));
	handle = dlsym(intern->handle, func_name->val);
	err = dlerror();

	if (err) {
		zend_throw_exception(mffi_ce_exception, err, 1);
		return;
	}

	function->function = handle;

	args_hash = Z_ARRVAL_P(args);
	function->arg_count = zend_hash_num_elements(args_hash);
	function->ffi_arg_types = ecalloc(function->arg_count + 1, sizeof(ffi_type *));
	function->php_arg_types = ecalloc(function->arg_count, sizeof(long));

	ZEND_HASH_FOREACH_VAL(args_hash, current_arg) {
		switch (Z_TYPE_P(current_arg)) {

			case IS_LONG:
				function->ffi_arg_types[i] = php_mffi_get_type(Z_LVAL_P(current_arg));
				function->php_arg_types[i] = Z_LVAL_P(current_arg);
				break;

			case IS_STRING:
				def = zend_hash_find_ptr(MFFI_G(struct_definitions), Z_STR_P(current_arg));

				if (def == NULL) {
					zend_throw_exception_ex(mffi_ce_exception, 0, "Struct definition for %s not found", Z_STRVAL_P(current_arg));
					efree(function->php_arg_types);
					efree(function->ffi_arg_types);
					return;
				}

//				function->ffi_arg_types[i] = &def->type;
				function->ffi_arg_types[i] = &ffi_type_pointer;
				function->php_arg_types[i] = FFI_TYPE_POINTER;
				break;

			default:
				zend_throw_exception(mffi_ce_exception, "Unsupported type", 1);
				efree(function->php_arg_types);
				efree(function->ffi_arg_types);
				return;
		}

		i++;

	} ZEND_HASH_FOREACH_END();
	function->ffi_arg_types[i] = NULL;

	def = NULL;

	if (return_type == NULL) {
		function->return_type = &ffi_type_void;
		function->php_return_type = FFI_TYPE_VOID;
	} else {
		switch (Z_TYPE_P(return_type)) {
			case IS_NULL:
				function->return_type = &ffi_type_void;
				function->php_return_type = FFI_TYPE_VOID;
				break;

			case IS_LONG:
				function->return_type = php_mffi_get_type(Z_LVAL_P(return_type));
				function->php_return_type = Z_LVAL_P(return_type);
				break;

			case IS_STRING:
				def = zend_hash_find_ptr(MFFI_G(struct_definitions), Z_STR_P(return_type));

				if (def == NULL) {
					zend_throw_exception_ex(mffi_ce_exception, 0, "Struct definition %s not found", Z_STRVAL_P(return_type));
					efree(function->php_arg_types);
					efree(function->ffi_arg_types);
					return;
				}

				function->return_type = &ffi_type_pointer;
				function->php_return_type = FFI_TYPE_POINTER;
				break;
		}
	}

	status = ffi_prep_cif(&function->cif, FFI_DEFAULT_ABI,
			function->arg_count, function->return_type, function->ffi_arg_types);

	if (status != FFI_OK) {
		zend_throw_exception(mffi_ce_exception, "Something went wrong preparing the function", 1);
		efree(function->php_arg_types);
		efree(function->ffi_arg_types);
		return;
	}

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
