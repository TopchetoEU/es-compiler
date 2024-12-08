#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct js_context *js_context_t;
typedef struct js_value *js_value_t;

extern js_value_t JS_UNDEFINED, JS_NULL, JS_TRUE, JS_FALSE;

typedef struct {
	js_value_t *value;
	int ref_count;
} js_capture_t;

// typedef struct {
// 	js_value_t
// } js_member_t;

js_value_t js_op_add(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_sub(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_mod(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_div(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_neg(js_context_t ctx, js_value_t val);

js_value_t js_op_b_and(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_b_or(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_b_xor(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_b_rsh(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_b_lsh(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_b_ursh(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_b_not(js_context_t ctx, js_value_t val);

js_value_t js_op_less(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_gr(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_leq(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_geq(js_context_t ctx, js_value_t a, js_value_t b);

js_value_t js_op_eq_loose(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_neq_loose(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_eq(js_context_t ctx, js_value_t a, js_value_t b);
js_value_t js_op_neq(js_context_t ctx, js_value_t a, js_value_t b);

js_value_t js_obj_set_proto(js_context_t ctx, js_value_t obj, js_value_t key);
js_value_t js_obj_get_proto(js_context_t ctx, js_value_t obj, js_value_t key);

js_value_t js_obj_get_own(js_context_t ctx, js_value_t obj, js_value_t key);
js_value_t js_obj_set_own(js_context_t ctx, js_value_t obj, js_value_t key, js_value_t value);
js_value_t js_obj_has_own(js_context_t ctx, js_value_t obj, js_value_t key);
js_value_t js_obj_del_own(js_context_t ctx, js_value_t obj, js_value_t key);

js_value_t js_obj_get(js_context_t ctx, js_value_t obj, js_value_t key);
js_value_t js_obj_set(js_context_t ctx, js_value_t obj, js_value_t key, js_value_t value);
js_value_t js_obj_has(js_context_t ctx, js_value_t obj, js_value_t key);

js_value_t js_obj_def_field(js_context_t ctx, js_value_t obj, js_value_t key, bool writable, bool configurable, bool enumerable);
js_value_t js_obj_def_prop(js_context_t ctx, js_value_t obj, js_value_t key, js_value_t get, js_value_t set, bool configurable, bool enumerable);

js_value_t js_func_create(js_context_t ctx, bool apply, bool construct, js_string_t name, js_func_body_t body, size_t capture_n, js_capture_t *captures);

// void *ref = malloc(1);

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
