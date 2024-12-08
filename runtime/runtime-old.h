#include <stdint.h>
#include <stddef.h>

const int8_t JS_UNDEFINED = 0x00;
const int8_t JS_NULL = 0x01;
const int8_t JS_TRUE = 0x02;
const int8_t JS_FALSE = 0x03;
const int8_t JS_STRING = 0x05;
// const int8_t JS_INT = 0x06;
const int8_t JS_FLOAT = 0x07;
const int8_t JS_SYMBOL = 0x09;
const int8_t JS_OBJECT = 0x0A;
const int8_t JS_ARRAY = 0x0B;
const int8_t JS_FUNCTION = 0x0C;

typedef struct js_string {
	int8_t mark;
	size_t n;
	const uint16_t *val;
} js_string_t;
typedef struct js_symbol {
	int8_t mark;
	js_string_t *description;
} js_symbol_t;
typedef struct js_object {
	int8_t mark;
} js_object_t;
typedef struct js_function {
	js_object_t object;
	js_function_body_t body;
} js_function_t;

typedef struct js_val {
	int8_t type;

	union {
		double number;
		js_string_t *string;
		js_symbol_t *symbol;
		js_object_t *object;
		js_function_t *function;
	} value;
} js_val_t;

typedef struct js_weak_ref {
	int8_t type;

	union {
		js_string_t *string;
		js_symbol_t *symbol;
		js_object_t *object;
		js_function_t *function;
	} value;
} js_weak_ref_t;

typedef js_val (*js_function_body_t)();

bool js_is_undefined(js_val_t);

js_val_t js_undefined();
js_val_t js_null();

js_val_t js_number_new(double val);
js_val_t js_string_new_c(const char *val);
js_val_t js_string_new_cn(size_t n, const char *val);
js_val_t js_string_new_cp(size_t n, const uint16_t *val);
js_val_t js_string_new_br(size_t n, const uint16_t *val);
js_val_t js_boolean_new(bool val);
js_val_t js_object_new(size_t alloc);
js_val_t js_array_new(size_t alloc);
js_val_t js_function_new(js_function_body_t val);
