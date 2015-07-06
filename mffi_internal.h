#ifndef PHP_MFFI_INTERNAL_H
#define PHP_MFFI_INTERNAL_H

extern zend_class_entry *mffi_ce_library;
extern zend_class_entry *mffi_ce_function;
extern zend_class_entry *mffi_ce_exception;
extern zend_class_entry *mffi_ce_type;
extern zend_class_entry *mffi_ce_struct;

#define PHP_MFFI_ERROR_HANDLING() \
	zend_error_handling error_handling; \
	zend_replace_error_handling(EH_THROW, mffi_ce_exception, &error_handling)

#define PHP_MFFI_RESTORE_ERRORS() \
	zend_restore_error_handling(&error_handling)

php_mffi_library_object *php_mffi_library_fetch_object(zend_object *obj);
php_mffi_function_object *php_mffi_function_fetch_object(zend_object *obj);
php_mffi_struct_object *php_mffi_struct_fetch_object(zend_object *obj);
void php_mffi_set_return_value(zval *return_value, php_mffi_value *result, long type);
void php_mffi_set_argument(zval *arg, php_mffi_value *dest, long type);
zend_bool php_mffi_types_from_array(zval *arg, long *php_type, ffi_type **type);

ffi_type *php_mffi_get_type(long type);

#endif	/* PHP_mffi_H */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
