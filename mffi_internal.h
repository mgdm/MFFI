#ifndef PHP_MFFI_INTERNAL_H
#define PHP_MFFI_INTERNAL_H

extern zend_class_entry *mffi_ce_handle;
extern zend_class_entry *mffi_ce_exception;

#define PHP_MFFI_ERROR_HANDLING() \
	zend_error_handling error_handling; \
	zend_replace_error_handling(EH_THROW, mffi_ce_exception, &error_handling)

#define PHP_MFFI_RESTORE_ERRORS() \
	zend_restore_error_handling(&error_handling)

#define PHP_MFFI_HANDLE_FROM_OBJECT(object, zv) \
	object = php_mffi_handle_fetch_object(Z_OBJ_P(zv))

#endif	/* PHP_mffi_H */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
