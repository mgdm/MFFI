#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dlfcn.h>
#include <ffi.h>

#include "php.h"
#include "zend_exceptions.h"
#include "php_mffi.h"
#include "mffi_internal.h"

zend_class_entry *mffi_ce_struct;

static zend_object_handlers mffi_struct_object_handlers;

/* {{{ PHP_METHOD(MFFI_Struct, define) */
PHP_METHOD(MFFI_Struct, define)
{
    HashTable *element_hash = NULL;
    zend_string *class_name = NULL, *string_key;
    zval *elements = NULL, *current_elem = NULL;
    long i = 0;
    size_t struct_size = 0;
    zend_ulong num_key = -1;
    ffi_type *type;
    zend_class_entry new_class;
    php_mffi_struct_object *template;

    PHP_MFFI_ERROR_HANDLING();
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &class_name, &elements) == FAILURE) {
        PHP_MFFI_RESTORE_ERRORS();
        return;
    }
    PHP_MFFI_RESTORE_ERRORS();

    if (zend_hash_exists(EG(class_table), class_name)) {
        zend_throw_exception(mffi_ce_exception, "Cannot redeclare class", 1);
        return;
    }

	element_hash = Z_ARRVAL_P(elements);
    template = ecalloc(1, sizeof(php_mffi_struct_object));
    template->element_count = zend_hash_num_elements(element_hash);
    template->elements = ecalloc(template->element_count + 1, sizeof(ffi_type *));
    template->php_types = ecalloc(template->element_count, sizeof(long));
    zend_hash_init(&template->element_hash, 8, NULL, NULL, 0);

	ZEND_HASH_FOREACH_KEY_VAL(element_hash, num_key, string_key, current_elem) {
        if (string_key == NULL) {
            zend_throw_exception(mffi_ce_exception, "Structure elements need names", 1);
            return;
        }

		if (Z_TYPE_P(current_elem) != IS_LONG && Z_TYPE_P(current_elem) != IS_STRING) {
			zend_throw_exception(mffi_ce_exception, "Unsupported type", 1);
			return;
		}

        type = php_mffi_get_type(Z_LVAL_P(current_elem));

        if (!type) {
            zend_throw_exception(mffi_ce_exception, "Unsupported type", 1);
            return;
        }

		zend_hash_add_ptr(&template->element_hash, string_key, type);
        template->elements[i] = type;
        template->php_types[i] = Z_LVAL_P(current_elem);
        struct_size += type->size;
		i++;

	} ZEND_HASH_FOREACH_END();

    template->elements[template->element_count] = NULL;
    template->type.elements = template->elements;
    zend_hash_add_ptr(MFFI_G(struct_definitions), class_name, template);

	INIT_CLASS_ENTRY_EX(new_class, class_name->val, class_name->len, NULL);
	zend_register_internal_class_ex(&new_class, mffi_ce_struct);
}
/* }}} */

/* {{{ */
const zend_function_entry mffi_struct_methods[] = {
	PHP_ME(MFFI_Struct, define, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};
/* }}} */

/* {{{ */
php_mffi_struct_object *php_mffi_struct_fetch_object(zend_object *obj) {
	return (php_mffi_struct_object *)((char*)(obj) - XtOffsetOf(php_mffi_struct_object, std));
}
/* }}} */


/* {{{ */
static zend_object *mffi_struct_object_new(zend_class_entry *ce)
{
	php_mffi_struct_object *object, *template;
    object = ecalloc(1, sizeof(php_mffi_struct_object) + zend_object_properties_size(ce));
    template = zend_hash_find_ptr(MFFI_G(struct_definitions), ce->name);

    if (!template) {
        zend_throw_exception(mffi_ce_exception, "Internal struct definition not found!", 1);
        return NULL;
    }

    memcpy(object, template, sizeof(php_mffi_struct_object));

	zend_object_std_init(&object->std, ce);
	object_properties_init(&object->std, ce);
	object->std.handlers = &mffi_struct_object_handlers;
	return &object->std;
}
/* }}} */

/* {{{ */
static void mffi_struct_object_free_storage(zend_object *object)
{
	php_mffi_struct_object *intern = php_mffi_struct_fetch_object(object);

	if (!intern) {
		return;
	}

	zend_object_std_dtor(&intern->std);
}
/* }}} */

/* {{{ */
static HashTable *php_mffi_struct_get_properties(zval *object) {
    php_mffi_struct_object *intern;
    zend_string *key;
    ffi_type *type;
    HashTable *props;
    zval *val = ecalloc(1, sizeof(zval));

    PHP_MFFI_STRUCT_FROM_OBJECT(intern, object);
    props = zend_std_get_properties(object);
    ZVAL_LONG(val, 0);
	
    ZEND_HASH_FOREACH_STR_KEY_PTR(&intern->element_hash, key, type) {
        zend_hash_update(props, key, val);
    } ZEND_HASH_FOREACH_END();
    
    return intern->std.properties;
}
/* }}} */

/* {{{ */
static zval *php_mffi_struct_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) {
    php_mffi_struct_object *intern;
    php_mffi_value *val;
    zval tmp, *retval;
    long index = 0, offset = 0;
    ffi_type *ffi_type;
    zend_string *current_key;
    char *member_key, *data = NULL;

    if (Z_TYPE_P(member) != IS_STRING) {
        tmp = *member;
        zval_copy_ctor(&tmp);
        convert_to_string(&tmp);
        member = &tmp;
    }

    member_key = Z_STRVAL_P(member);
    
    PHP_MFFI_STRUCT_FROM_OBJECT(intern, object);

    /* TODO - there has to be a better way */
    index = offset = 0;
    ZEND_HASH_FOREACH_STR_KEY_PTR(&intern->element_hash, current_key, ffi_type) {
        php_printf("Looking at member %s for member %s\n", current_key->val, member_key);
        if (strncmp(member_key, current_key->val, current_key->len) == 0) {
            php_printf("Found member %s\n", member_key);
            break;
        }

        offset += ffi_type->size;
        php_printf("Offset is now %d, index is %d\n", offset, index);
        index++;

    } ZEND_HASH_FOREACH_END();

    
    data = (char *) intern->data;
    data += offset;
    val = (php_mffi_value *) data;

    retval = ecalloc(1, sizeof(zval));
    php_mffi_set_return_value(retval, val, intern->php_types[index]);
    return retval;
}
/* }}} */

/* {{{ */
PHP_MINIT_FUNCTION(mffi_struct)
{
	zend_class_entry function_ce;

	memcpy(&mffi_struct_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	mffi_struct_object_handlers.offset = XtOffsetOf(php_mffi_struct_object, std);
	mffi_struct_object_handlers.free_obj = mffi_struct_object_free_storage;
	mffi_struct_object_handlers.clone_obj = NULL;
    mffi_struct_object_handlers.read_property = php_mffi_struct_read_property;
//    mffi_struct_object_handlers.write_property = php_mffi_struct_write_property;
//    mffi_struct_object_handlers.has_property = php_mffi_struct_has_property;
    mffi_struct_object_handlers.get_properties = php_mffi_struct_get_properties;

	INIT_NS_CLASS_ENTRY(function_ce, "MFFI", "Struct", mffi_struct_methods);
	function_ce.create_object = mffi_struct_object_new;
	mffi_ce_struct = zend_register_internal_class(&function_ce);

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
