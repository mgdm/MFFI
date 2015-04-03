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
zend_class_entry *mffi_ce_type;

ZEND_DECLARE_MODULE_GLOBALS(mffi)

ffi_type *php_mffi_get_type(long type) {
	switch(type) {
		case FFI_TYPE_VOID:
			return &ffi_type_void;

		case FFI_TYPE_INT:
			return &ffi_type_sint;

		case FFI_TYPE_FLOAT:
			return &ffi_type_float;

		case FFI_TYPE_DOUBLE:
			return &ffi_type_double;

		case FFI_TYPE_LONGDOUBLE:
			return &ffi_type_longdouble;

		case FFI_TYPE_UINT8:
			return &ffi_type_uint8;

		case FFI_TYPE_SINT8:
			return &ffi_type_sint8;

		case FFI_TYPE_UINT16:
			return &ffi_type_uint16;

		case FFI_TYPE_SINT16:
			return &ffi_type_sint16;

		case FFI_TYPE_UINT32:
			return &ffi_type_uint32;

		case FFI_TYPE_SINT32:
			return &ffi_type_sint32;

		case FFI_TYPE_UINT64:
			return &ffi_type_uint64;

		case FFI_TYPE_SINT64:
			return &ffi_type_sint64;

		case PHP_MFFI_TYPE_STRING:
			return &ffi_type_pointer;

		case FFI_TYPE_STRUCT:
		case FFI_TYPE_POINTER:
		default:
			return NULL;
	}
}

void php_mffi_set_return_value(zval *return_value, php_mffi_value *result, long type) {
	switch (type) {
		case FFI_TYPE_INT:
			ZVAL_LONG(return_value, result->i);
			break;

		case FFI_TYPE_FLOAT:
			ZVAL_DOUBLE(return_value, result->f);
			break;

		case FFI_TYPE_DOUBLE:
			ZVAL_DOUBLE(return_value, result->d);
			break;

		case FFI_TYPE_LONGDOUBLE:
			ZVAL_DOUBLE(return_value, result->D);
			break;

		case FFI_TYPE_UINT8:
		case FFI_TYPE_SINT8:
		case FFI_TYPE_UINT16:
		case FFI_TYPE_SINT16:
			ZVAL_LONG(return_value, result->i);
			break;

		case FFI_TYPE_UINT32:
		case FFI_TYPE_SINT32:
		case FFI_TYPE_UINT64:
		case FFI_TYPE_SINT64:
			ZVAL_LONG(return_value, result->l);
			break;

		case PHP_MFFI_TYPE_STRING:
			if (result->s != NULL) {
				ZVAL_STRING(return_value, result->s);
			} else {
				ZVAL_NULL(return_value);
			}

			break;

		default:
			ZVAL_NULL(return_value);
			break;
	}
}

void php_mffi_set_argument(zval *arg, php_mffi_value *dest, long type) {
	zval tmp = *arg;
	zval_copy_ctor(&tmp);

	switch(type) {
		case FFI_TYPE_INT:
			convert_to_long(&tmp);
			dest->i = Z_LVAL(tmp);
			break;

		case FFI_TYPE_FLOAT:
			convert_to_double(&tmp);
			dest->f = Z_DVAL(tmp);
			break;

		case FFI_TYPE_DOUBLE:
			convert_to_double(&tmp);
			dest->d = Z_DVAL(tmp);
			break;

		case FFI_TYPE_LONGDOUBLE:
			convert_to_double(&tmp);
			dest->D = Z_DVAL(tmp);
			break;

		case FFI_TYPE_UINT8:
		case FFI_TYPE_SINT8:
			convert_to_long(&tmp);
			dest->c = Z_LVAL(tmp);
			break;

		case FFI_TYPE_UINT16:
		case FFI_TYPE_SINT16:
			convert_to_long(&tmp);
			dest->i = Z_LVAL(tmp);
			break;

		case FFI_TYPE_UINT32:
			convert_to_long(&tmp);
			dest->l = Z_LVAL(tmp);
			break;

		case FFI_TYPE_SINT32:
			convert_to_long(&tmp);
			dest->l = Z_LVAL(tmp);
			break;

		case FFI_TYPE_UINT64:
		case FFI_TYPE_SINT64:
			convert_to_long(&tmp);
			dest->l = Z_LVAL(tmp);
			break;

		case PHP_MFFI_TYPE_STRING:
			convert_to_string(&tmp);
			dest->s = estrdup(Z_STRVAL(tmp));
			break;

		case FFI_TYPE_STRUCT:
		case FFI_TYPE_POINTER:
		default:
			zend_throw_exception(mffi_ce_exception, "Unimplemented type", 1);
			break;
	}

	zval_dtor(&tmp);
}


/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mffi)
{
	zend_class_entry exception_ce, type_ce;

	INIT_NS_CLASS_ENTRY(exception_ce, "MFFI", "Exception", NULL);
	mffi_ce_exception = zend_register_internal_class_ex(&exception_ce, zend_exception_get_default());

	INIT_NS_CLASS_ENTRY(type_ce, "MFFI", "Type", NULL);
	mffi_ce_type = zend_register_internal_class(&type_ce);

	#define REGISTER_MFFI_TYPE_LONG_CONST(const_name, value) \
	zend_declare_class_constant_long(mffi_ce_type, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC); \
	REGISTER_LONG_CONSTANT(#value,  value,  CONST_CS | CONST_PERSISTENT);

	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_VOID", FFI_TYPE_VOID);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_INT", FFI_TYPE_INT);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_FLOAT", FFI_TYPE_FLOAT);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_DOUBLE", FFI_TYPE_DOUBLE);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_LONGDOUBLE", FFI_TYPE_LONGDOUBLE);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_UINT8", FFI_TYPE_UINT8);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_SINT8", FFI_TYPE_SINT8);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_UINT16", FFI_TYPE_UINT16);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_SINT16", FFI_TYPE_SINT16);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_UINT32", FFI_TYPE_UINT32);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_SINT32", FFI_TYPE_SINT32);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_UINT64", FFI_TYPE_UINT64);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_SINT64", FFI_TYPE_SINT64);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_STRUCT", FFI_TYPE_STRUCT);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_POINTER", FFI_TYPE_POINTER);
	REGISTER_MFFI_TYPE_LONG_CONST("TYPE_STRING", PHP_MFFI_TYPE_STRING);

	PHP_MINIT(mffi_library)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(mffi_function)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(mffi_struct)(INIT_FUNC_ARGS_PASSTHRU);

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
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION(mffi) */
static PHP_GINIT_FUNCTION(mffi)
{
#if defined(COMPILE_DL_MFFI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	ALLOC_HASHTABLE(MFFI_G(struct_definitions));
	zend_hash_init(MFFI_G(struct_definitions), 8, NULL, NULL, 0);
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(mffi)
{
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(mffi)
{
	php_mffi_struct_definition *def;
	php_mffi_struct_element *elem;

	ZEND_HASH_FOREACH_PTR(MFFI_G(struct_definitions), def) {
		efree(def->elements);
		efree(def->element_types);
		zend_hash_destroy(&def->element_hash);
		efree(def);

	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(MFFI_G(struct_definitions));
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
	PHP_RINIT(mffi),
	PHP_RSHUTDOWN(mffi),
	PHP_MINFO(mffi),
	PHP_MFFI_VERSION,
	PHP_MODULE_GLOBALS(mffi),
	PHP_GINIT(mffi),
	PHP_GSHUTDOWN(mffi),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
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
