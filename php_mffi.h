/* __header_here__ */

#ifndef PHP_MFFI_H
#define PHP_MFFI_H

extern zend_module_entry mffi_module_entry;
#define phpext_mffi_ptr &mffi_module_entry

#define PHP_MFFI_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_MFFI_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_MFFI_API __attribute__ ((visibility("default")))
#else
#	define PHP_MFFI_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(mffi)
	HashTable *struct_definitions;
ZEND_END_MODULE_GLOBALS(mffi)

ZEND_EXTERN_MODULE_GLOBALS(mffi)

/* Always refer to the globals in your function as mffi_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MFFI_G(v) ZEND_TSRMG(mffi_globals_id, zend_mffi_globals *, v)
#ifdef COMPILE_DL_MFFI
ZEND_TSRMLS_CACHE_EXTERN();
#endif
#else
#define MFFI_G(v) (mffi_globals.v)
#endif

PHP_MINIT_FUNCTION(mffi);
PHP_MINIT_FUNCTION(mffi_library);
PHP_MINIT_FUNCTION(mffi_function);
PHP_MINIT_FUNCTION(mffi_struct);
PHP_MSHUTDOWN_FUNCTION(mffi);
PHP_MINIT_FUNCTION(mffi);

typedef union _php_mffi_value {
	char c;
	int i;
	long l;
	float f;
	double d;
	long double D;
	void *p;
	char *s;
} php_mffi_value;

typedef struct _php_mffi_library_object {
	void *handle;
	zend_object std;
} php_mffi_library_object;

typedef struct _php_mffi_function_object {
	void *function;
	zval *library_zval;
	ffi_type *return_type;
	long php_return_type;
	ffi_type **ffi_arg_types;
	long *php_arg_types;
	long arg_count;
	ffi_cif cif;

	zend_object std;
} php_mffi_function_object;

typedef struct _php_mffi_struct_element {
	long index;
	long php_type;
	size_t offset;
	ffi_type *type;
} php_mffi_struct_element;

typedef struct _php_mffi_struct_definition {
	ffi_type type;
	php_mffi_struct_element *elements;
	HashTable element_hash;
	ffi_type **element_types;
	long element_count;
	size_t struct_size;
} php_mffi_struct_definition;

typedef struct _php_mffi_struct_object {
	php_mffi_struct_definition *template;

	void *data;

	zend_object std;
} php_mffi_struct_object;


#define PHP_MFFI_TYPE_STRING 64

#endif	/* PHP_mffi_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
