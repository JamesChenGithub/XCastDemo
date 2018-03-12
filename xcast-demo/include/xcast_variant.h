/*
** Copyright (c) 2017 The XCast project. All rights reserved.
*/
#ifndef XCAST_VARIANT_H_
#define XCAST_VARIANT_H_

#include "xcast_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  xc_vtype_null = 0,
  xc_vtype_bool = 'b',
  xc_vtype_uint8 = 'y',
  xc_vtype_int16 = 'n',
  xc_vtype_uint16 = 'q',
  xc_vtype_int32 = 'i',
  xc_vtype_uint32 = 'u',
  xc_vtype_int64 = 'x',
  xc_vtype_uint64 = 't',
  xc_vtype_float = 'f',
  xc_vtype_double = 'd',
  xc_vtype_string = 'c',
  xc_vtype_ptr = '*',
  xc_vtype_array = '[',
  xc_vtype_dict = '{',
  xc_vtype_variant = 'v'
} xcast_variant_type;

xcast_export xcast_variant_t *xcast_variant_bool_new(bool value);
xcast_export bool xcast_variant_bool_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_bool_set(xcast_variant_t *v, bool value);
xcast_export xcast_variant_t *xcast_variant_uint8_new(uint8_t value);
xcast_export uint8_t xcast_variant_uint8_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_uint8_set(xcast_variant_t *v, uint8_t value);
xcast_export xcast_variant_t *xcast_variant_int16_new(int16_t value);
xcast_export int16_t xcast_variant_int16_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_int16_set(xcast_variant_t *v, int16_t value);
xcast_export xcast_variant_t *xcast_variant_uint16_new(uint16_t value);
xcast_export uint16_t xcast_variant_uint16_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_uint16_set(xcast_variant_t *v, uint16_t value);
xcast_export xcast_variant_t *xcast_variant_int32_new(int32_t value);
xcast_export int32_t xcast_variant_int32_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_int32_set(xcast_variant_t *v, int32_t value);
xcast_export xcast_variant_t *xcast_variant_uint32_new(uint32_t value);
xcast_export uint32_t xcast_variant_uint32_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_uint32_set(xcast_variant_t *v, uint32_t value);
xcast_export xcast_variant_t *xcast_variant_int64_new(int64_t value);
xcast_export int64_t xcast_variant_int64_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_int64_set(xcast_variant_t *v, int64_t value);
xcast_export xcast_variant_t *xcast_variant_uint64_new(uint64_t value);
xcast_export uint64_t xcast_variant_uint64_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_uint64_set(xcast_variant_t *v, uint64_t value);
xcast_export xcast_variant_t *xcast_variant_float_new(float value);
xcast_export float xcast_variant_float_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_float_set(xcast_variant_t *v, float value);
xcast_export xcast_variant_t *xcast_variant_double_new(double value);
xcast_export double xcast_variant_double_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_double_set(xcast_variant_t *v, double value);
xcast_export xcast_variant_t *xcast_variant_str_new(const char *value);
xcast_export const char *xcast_variant_str_get(xcast_variant_t *v);
xcast_export int32_t xcast_variant_str_cmp(xcast_variant_t *v, const char *value);
xcast_export xcast_variant_t *xcast_variant_str_set(xcast_variant_t *v, const char *value);
xcast_export xcast_variant_t *xcast_variant_ptr_new(void *value);
xcast_export void *xcast_variant_ptr_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_ptr_set(xcast_variant_t *v, void *value);
xcast_export xcast_variant_t *xcast_variant_buf_new(const uint8_t *value, uint32_t len);
xcast_export const uint8_t *xcast_variant_buf_get(xcast_variant_t *v);
xcast_export xcast_variant_t *xcast_variant_buf_owned_new(uint8_t *value, uint32_t len);

xcast_export xcast_variant_type xcast_variant_get_type(xcast_variant_t *v);
xcast_export uint32_t xcast_variant_get_size(xcast_variant_t *v);
xcast_export bool xcast_variant_empty(xcast_variant_t *v);

xcast_export xcast_variant_t *xcast_variant_ref(xcast_variant_t *variant);
xcast_export void xcast_variant_unref(xcast_variant_t *variant);
xcast_export xcast_variant_t *xcast_variant_change_type(xcast_variant_t *variant, xcast_variant_type vt);

/* variant array */
xcast_export xcast_variant_t *xcast_variant_array_new();
xcast_export void xcast_variant_array_append(xcast_variant_t *va, xcast_variant_t *value);
xcast_export xcast_variant_t *xcast_variant_array_index(xcast_variant_t *v, uint32_t index);

xcast_export bool xcast_varray_get_bool(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_bool(xcast_variant_t *v, bool value);
xcast_export uint8_t xcast_varray_get_uint8(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_uint8(xcast_variant_t *v, uint8_t value);
xcast_export int16_t xcast_varray_get_int16(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_int16(xcast_variant_t *v, int16_t value);
xcast_export uint16_t xcast_varray_get_uint16(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_uint16(xcast_variant_t *v, uint16_t value);
xcast_export int32_t xcast_varray_get_int32(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_int32(xcast_variant_t *v, int32_t value);
xcast_export uint32_t xcast_varray_get_uint32(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_uint32(xcast_variant_t *v, uint32_t value);
xcast_export int64_t xcast_varray_get_int64(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_int64(xcast_variant_t *v, int64_t value);
xcast_export uint64_t xcast_varray_get_uint64(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_uint64(xcast_variant_t *v, uint64_t value);
xcast_export float xcast_varray_get_float(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_float(xcast_variant_t *v, float value);
xcast_export double xcast_varray_get_double(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_double(xcast_variant_t *v, double value);
xcast_export const char *xcast_varray_get_str(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_str(xcast_variant_t *v, const char *value);
xcast_export const uint8_t *xcast_varray_get_buf(xcast_variant_t *v, uint32_t index);
xcast_export void xcast_varray_add_buf(xcast_variant_t *v, const uint8_t *value, uint32_t len);
xcast_export void xcast_varray_add_buf_owned(xcast_variant_t *v, uint8_t *value, uint32_t len);

/* variant dictionary */
xcast_export xcast_variant_t *xcast_variant_dict_new();
xcast_export void xcast_variant_dict_set(xcast_variant_t *dict, const char *key, xcast_variant_t *value);
xcast_export xcast_variant_t *xcast_variant_dict_get(xcast_variant_t *dict, const char *key);
xcast_export bool xcast_variant_dict_contains(xcast_variant_t *dict, const char *key);

xcast_export bool xcast_vdict_get_bool(xcast_variant_t *v, const char *key, bool def);
xcast_export void xcast_vdict_set_bool(xcast_variant_t *v, const char *key, bool value);
xcast_export uint8_t xcast_vdict_get_uint8(xcast_variant_t *v, const char *key, uint8_t def);
xcast_export void xcast_vdict_set_uint8(xcast_variant_t *v, const char *key, uint8_t value);
xcast_export int16_t xcast_vdict_get_int16(xcast_variant_t *v, const char *key, int16_t def);
xcast_export void xcast_vdict_set_int16(xcast_variant_t *v, const char *key, int16_t value);
xcast_export uint16_t xcast_vdict_get_uint16(xcast_variant_t *v, const char *key, uint16_t def);
xcast_export void xcast_vdict_set_uint16(xcast_variant_t *v, const char *key, uint16_t value);
xcast_export int32_t xcast_vdict_get_int32(xcast_variant_t *v, const char *key, int32_t def);
xcast_export void xcast_vdict_set_int32(xcast_variant_t *v, const char *key, int32_t value);
xcast_export uint32_t xcast_vdict_get_uint32(xcast_variant_t *v, const char *key, uint32_t def);
xcast_export void xcast_vdict_set_uint32(xcast_variant_t *v, const char *key, uint32_t value);
xcast_export int64_t xcast_vdict_get_int64(xcast_variant_t *v, const char *key, int64_t def);
xcast_export void xcast_vdict_set_int64(xcast_variant_t *v, const char *key, int64_t value);
xcast_export uint64_t xcast_vdict_get_uint64(xcast_variant_t *v, const char *key, uint64_t def);
xcast_export void xcast_vdict_set_uint64(xcast_variant_t *v, const char *key, uint64_t value);
xcast_export float xcast_vdict_get_float(xcast_variant_t *v, const char *key, float def);
xcast_export void xcast_vdict_set_float(xcast_variant_t *v, const char *key, float value);
xcast_export double xcast_vdict_get_double(xcast_variant_t *v, const char *key, double def);
xcast_export void xcast_vdict_set_double(xcast_variant_t *v, const char *key, double value);
xcast_export const char *xcast_vdict_get_str(xcast_variant_t *v, const char *key, const char *def);
xcast_export void xcast_vdict_set_str(xcast_variant_t *v, const char *key, const char *value);
xcast_export const uint8_t *xcast_vdict_get_buf(xcast_variant_t *v, const char *key, const uint8_t *def);
xcast_export void xcast_vdict_set_buf(xcast_variant_t *v, const char *key, const uint8_t *value, uint32_t len);
xcast_export void xcast_vdict_set_buf_owned(xcast_variant_t *v, const char *key, uint8_t *value, uint32_t len);
xcast_export char *xcast_variant_dump(const xcast_variant_t *v);

#ifdef __cplusplus
}
#endif

#endif /* XCAST_VARIANT_H_ */
