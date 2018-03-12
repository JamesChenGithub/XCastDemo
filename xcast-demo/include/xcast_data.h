/*
** Copyright (c) 2016 The xcast project. All rights reserved.
*/
#ifndef XCAST_DATA_H_
#define XCAST_DATA_H_

#include "xcast_base.h"
#include "xcast_variant.h"

/*
* data type used by xcast for property operations.
* you can store basic types, such as 'bool', 'int', 'float', 'string' in
* a 'xcast_data_t' type.
* container types of 'array' and 'dictionary' are also supported.
* when 'xcast_data_t' is a container, all its elements have a type of 'xcast_data_t'.
*/
class xcast_data_t {
public:
  /* xcast_data_t constructors */
  explicit xcast_data_t() : value_(NULL) {}
  explicit xcast_data_t(xcast_variant_t *val) : value_(val) { xcast_variant_ref(val); }
  xcast_data_t(const xcast_data_t &other) : value_(NULL) {
    /* set new one if any */
    if (other.value_) {
      value_ = (xcast_variant_t *)other.value_;
      xcast_variant_ref(value_);
    }
  }

  /* construct from basic types */
  explicit xcast_data_t(bool value) { value_ = xcast_variant_bool_new(value); }
  explicit xcast_data_t(uint8_t value) { value_ = xcast_variant_uint8_new(value); }
  explicit xcast_data_t(uint16_t value) { value_ = xcast_variant_uint16_new(value); }
  explicit xcast_data_t(uint32_t value) { value_ = xcast_variant_uint32_new(value); }
  explicit xcast_data_t(uint64_t value) { value_ = xcast_variant_uint64_new(value); }
  explicit xcast_data_t(int16_t value) { value_ = xcast_variant_int16_new(value); }
  explicit xcast_data_t(int32_t value) { value_ = xcast_variant_int32_new(value); }
  explicit xcast_data_t(int64_t value) { value_ = xcast_variant_int64_new(value); }
  explicit xcast_data_t(float value) { value_ = xcast_variant_float_new(value); }
  explicit xcast_data_t(double value) { value_ = xcast_variant_double_new(value); }
  explicit xcast_data_t(const char* value) { value_ = xcast_variant_str_new(value); }
  explicit xcast_data_t(const uint8_t* value, uint32_t len) { value_ = xcast_variant_buf_new(value, len); }
  ~xcast_data_t() { if (value_) xcast_variant_unref(value_); }

  /* format null terminated string with a max size of '255' */
  const char *format(const char *fmt, ...) {
    va_list           arg;
    int32_t           len;

    if (!fmt || !*fmt) return NULL;

    if (value_) xcast_variant_unref(value_);
    value_ = xcast_variant_buf_new(NULL, 256);
    if (!value_) return NULL;

    va_start(arg, fmt);
    len = vsnprintf((char *)xcast_variant_str_get(value_), 256, fmt, arg);
    va_end(arg);

    if (len <= 0) {
      xcast_variant_unref(value_);
      return NULL;
    }

    return xcast_variant_str_get(value_);
  }

  /* container: whether 'xcast_data_t' is empty */
  bool empty() {
    if (!value_) return true;

    return xcast_variant_empty(value_);
  }

  /* container: elements count in 'xcast_data_t' */
  uint32_t size() {
    if (!value_) return 0;

    return xcast_variant_get_size(value_);
  }

  /* current 'xcast_data_t' type */
  char type() {
    if (!value_) return 0;

    return (char)xcast_variant_get_type(value_);
  }

  //char *dump() {
  //  return value_ ? xcast_variant_dump(value_) : NULL;
  //}

  /* get basic type value from 'xcast_data_t' */
  operator bool() { return value_ ? xcast_variant_bool_get(value_) : false; }
  operator uint8_t() { return value_ ? xcast_variant_uint8_get(value_) : 0; }
  operator uint16_t() { return value_ ? xcast_variant_uint16_get(value_) : 0; }
  operator uint32_t() { return value_ ? xcast_variant_uint32_get(value_) : 0; }
  operator uint64_t() { return value_ ? xcast_variant_uint64_get(value_) : 0; }
  operator int16_t() { return value_ ? xcast_variant_int16_get(value_) : 0; }
  operator int32_t() { return value_ ? xcast_variant_int32_get(value_) : 0; }
  operator int64_t() { return value_ ? xcast_variant_int64_get(value_) : 0; }
  operator float() { return value_ ? xcast_variant_float_get(value_) : 0.0f; }
  operator double() { return value_ ? xcast_variant_double_get(value_) : 0.0; }
  operator const char *() { return value_ ? xcast_variant_str_get(value_) : NULL; }
  operator const uint8_t *() { return value_ ? xcast_variant_buf_get(value_) : NULL; }

  bool bool_val() { return value_ ? xcast_variant_bool_get(value_) : false; }
  uint8_t uint8_val() { return value_ ? xcast_variant_uint8_get(value_) : 0; }
  uint16_t uint16_val() { return value_ ? xcast_variant_uint16_get(value_) : 0; }
  uint32_t uint32_val() { return value_ ? xcast_variant_uint32_get(value_) : 0; }
  uint64_t uint64_val() { return value_ ? xcast_variant_uint64_get(value_) : 0; }
  int16_t int16_val() { return value_ ? xcast_variant_int16_get(value_) : 0; }
  int32_t int32_val() { return value_ ? xcast_variant_int32_get(value_) : 0; }
  int64_t int64_val() { return value_ ? xcast_variant_int64_get(value_) : 0; }
  float float_val() { return value_ ? xcast_variant_float_get(value_) : 0.0f; }
  double double_val() { return value_ ? xcast_variant_double_get(value_) : 0.0; }
  const char *str_val() { return value_ ? xcast_variant_str_get(value_) : NULL; }
  const uint8_t *bytes_val() { return value_ ? xcast_variant_buf_get(value_) : NULL; }

  /* attach from variant type without ref */
  xcast_data_t &operator = (xcast_variant_t *val) {
    attach(val);
    return *this;
  }

  /* assignment from other data types */
  xcast_data_t &operator = (const xcast_data_t &other) {
    /* unref old one */
    if (value_) {
      xcast_variant_unref(value_);
      value_ = NULL;
    }

    /* set new one if any */
    if (other.value_) {
      value_ = (xcast_variant_t *)other.value_;
      xcast_variant_ref(value_);
    }

    return *this;
  }

  xcast_data_t &operator = (bool value) {
    if (!value_) {
      value_ = xcast_variant_bool_new(value);
    } else {
      value_ = xcast_variant_bool_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (uint8_t value) {
    if (!value_) {
      value_ = xcast_variant_uint8_new(value);
    } else {
      value_ = xcast_variant_uint8_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (uint16_t value) {
    if (!value_) {
      value_ = xcast_variant_uint16_new(value);
    } else {
      value_ = xcast_variant_uint16_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (uint32_t value) {
    if (!value_) {
      value_ = xcast_variant_uint32_new(value);
    } else {
      value_ = xcast_variant_uint32_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (uint64_t value) {
    if (!value_) {
      value_ = xcast_variant_uint64_new(value);
    } else {
      value_ = xcast_variant_uint64_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (int16_t value) {
    if (!value_) {
      value_ = xcast_variant_int16_new(value);
    } else {
      value_ = xcast_variant_int16_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (int32_t value) {
    if (!value_) {
      value_ = xcast_variant_int32_new(value);
    } else {
      value_ = xcast_variant_int32_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (int64_t value) {
    if (!value_) {
      value_ = xcast_variant_int64_new(value);
    } else {
      value_ = xcast_variant_int64_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (float value) {
    if (!value_) {
      value_ = xcast_variant_float_new(value);
    } else {
      value_ = xcast_variant_float_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (double value) {
    if (!value_) {
      value_ = xcast_variant_double_new(value);
    } else {
      value_ = xcast_variant_double_set(value_, value);
    }

    return *this;
  }

  xcast_data_t &operator = (const char *value) {
    if (!value_) {
      value_ = xcast_variant_str_new(value);
    } else {
      value_ = xcast_variant_str_set(value_, value);
    }

    return *this;
  }

  bool operator == (bool value) {
    if (!value_ || type() != xc_vtype_bool) return false;

    return bool_val() == value;
  }

  bool operator == (uint8_t value) {
    if (!value_ || type() != xc_vtype_uint8) return false;

    return uint8_val() == value;
  }

  bool operator == (uint16_t value) {
    if (!value_ || type() != xc_vtype_uint16) return false;

    return uint16_val() == value;
  }

  bool operator == (uint32_t value) {
    if (!value_ || type() != xc_vtype_uint32) return false;

    return uint32_val() == value;
  }

  bool operator == (uint64_t value) {
    if (!value_ || type() != xc_vtype_uint64) return false;

    return uint64_val() == value;
  }

  bool operator == (int16_t value) {
    if (!value_ || type() != xc_vtype_int16) return false;

    return int16_val() == value;
  }

  bool operator == (int32_t value) {
    if (!value_ || type() != xc_vtype_int32) return false;

    return int32_val() == value;
  }

  bool operator == (int64_t value) {
    if (!value_ || type() != xc_vtype_int64) return false;

    return int64_val() == value;
  }

  bool operator == (float value) {
    if (!value_ || type() != xc_vtype_float) return false;

    return float_val() == value;
  }

  bool operator == (double value) {
    if (!value_ || type() != xc_vtype_double) return false;

    return double_val() == value;
  }

  bool operator == (const char *value) {
    if (!value_ || type() != xc_vtype_string) return false;

    return xcast_variant_str_cmp(value_, value) ? false : true;
  }

  /* access 'xcast_data_t' as array type, if 'index' is larger
  * than array size, an empty data is returned. if you want to add new
  * element into the array, use the append methods instead.
  */
  xcast_data_t operator[](uint32_t index) {
    if (!value_ || type() != xc_vtype_array) return xcast_data_t();
    return at(index);
  }

  /* access 'xcast_data_t' as dictionary type, if 'key' is not exist
  * in the dictionary, an new data with type 'int32_t' is inserted into the 
  * dictionary with the given key and returned.
  */
  xcast_data_t operator[](const char *key) {
    if (!key || !*key) return xcast_data_t();
    if (!value_ || type() != xc_vtype_dict) init_vdict();
    if (!contains(key)) put(key, xcast_data_t(0));
    return get(key);
  }

  /* assign a none-copied buffer to 'xcast_data_t' and take ownership of it */
  xcast_data_t &assign(uint8_t *data, uint32_t len) {
    if (value_) xcast_variant_unref(value_);
    value_ = xcast_variant_buf_owned_new(data, len);

    return *this;
  }

  /* 'key-value' dictionary methods */
  void put(const char *key, xcast_data_t data) {
    if (!data.value_) return;

    init_vdict();
    xcast_variant_dict_set(value_, key, data.value_);
  }

  bool contains(const char *key) {
    if (!value_ || type() != xc_vtype_dict) return false;

    return xcast_variant_dict_contains(value_, key);
  }

  void put_bool(const char *key, bool value) {
    init_vdict();
    xcast_vdict_set_bool(value_, key, value);
  }

  void put_uint8(const char *key, uint8_t value) {
    init_vdict();
    xcast_vdict_set_uint8(value_, key, value);
  }

  void put_uint16(const char *key, uint16_t value) {
    init_vdict();
    xcast_vdict_set_uint16(value_, key, value);
  }

  void put_uint32(const char *key, uint32_t value) {
    init_vdict();
    xcast_vdict_set_uint32(value_, key, value);
  }

  void put_uint64(const char *key, uint64_t value) {
    init_vdict();
    xcast_vdict_set_uint64(value_, key, value);
  }

  void put_int16(const char *key, int16_t value) {
    init_vdict();
    xcast_vdict_set_int16(value_, key, value);
  }

  void put_int32(const char *key, int32_t value) {
    init_vdict();
    xcast_vdict_set_int32(value_, key, value);
  }

  void put_int64(const char *key, int64_t value) {
    init_vdict();
    xcast_vdict_set_int64(value_, key, value);
  }

  void put_str(const char *key, const char *value) {
    init_vdict();
    xcast_vdict_set_str(value_, key, value);
  }

  /* put a copied buffer into 'xcast_data_t' */
  void put_bytes(const char *key, const uint8_t *data, uint32_t len) {
    init_vdict();
    xcast_vdict_set_buf(value_, key, data, len);
  }

  /* put a none-copied buffer into 'xcast_data_t' and take ownership of it */
  void put_bytes_owned(const char *key, uint8_t *data, uint32_t len) {
    init_vdict();
    xcast_vdict_set_buf_owned(value_, key, data, len);
  }

  /* 'key-value' dictionary methods */
  xcast_data_t get(const char *key) {
    xcast_data_t data;
    if (value_) data.value_ = xcast_variant_dict_get(value_, key);
    return data;
  }

  bool get_bool(const char *key, bool def = false) {
    if (!value_) return def;
    return xcast_vdict_get_bool(value_, key, def);
  }

  uint8_t get_uint8(const char *key, uint8_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_uint8(value_, key, def);
  }

  uint16_t get_uint16(const char *key, uint16_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_uint16(value_, key, def);
  }

  uint32_t get_uint32(const char *key, uint32_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_uint32(value_, key, def);
  }

  uint64_t get_uint64(const char *key, uint64_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_uint64(value_, key, def);
  }

  int16_t get_int16(const char *key, int16_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_int16(value_, key, def);
  }

  int32_t get_int32(const char *key, int32_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_int32(value_, key, def);
  }

  int64_t get_int64(const char *key, int64_t def = 0) {
    if (!value_) return def;
    return xcast_vdict_get_int64(value_, key, def);
  }

  float get_float(const char *key, float def = 0.0f) {
    if (!value_) return def;
    return xcast_vdict_get_float(value_, key, def);
  }

  double get_double(const char *key, double def = 0.0) {
    if (!value_) return def;
    return xcast_vdict_get_double(value_, key, def);
  }

  const char *get_str(const char *key, const char *def = NULL) {
    if (!value_) return def;
    return xcast_vdict_get_str(value_, key, def);
  }

  const uint8_t *get_bytes(const char *key, const uint8_t *def = NULL) {
    if (!value_) return def;
    return xcast_vdict_get_buf(value_, key, def);
  }

  /* 'index' based array methods */
  void append(xcast_data_t data) {
    if (!data.value_) return;
    init_varray();
    xcast_variant_array_append(value_, data.value_);
  }

  void append_bool(bool value) {
    init_varray();
    xcast_varray_add_bool(value_, value);
  }

  void append_uint8(uint8_t value) {
    init_varray();
    xcast_varray_add_uint8(value_, value);
  }

  void append_uint16(uint16_t value) {
    init_varray();
    xcast_varray_add_uint16(value_, value);
  }

  void append_uint32(uint32_t value) {
    init_varray();
    xcast_varray_add_uint32(value_, value);
  }

  void append_uint64(uint64_t value) {
    init_varray();
    xcast_varray_add_uint64(value_, value);
  }

  void append_int16(int16_t value) {
    init_varray();
    xcast_varray_add_int16(value_, value);
  }

  void append_int32(int32_t value) {
    init_varray();
    xcast_varray_add_int32(value_, value);
  }

  void append_int64(int64_t value) {
    init_varray();
    xcast_varray_add_int64(value_, value);
  }

  void append_float(float value) {
    init_varray();
    xcast_varray_add_float(value_, value);
  }

  void append_double(double value) {
    init_varray();
    xcast_varray_add_double(value_, value);
  }

  void append_str(const char *value) {
    init_varray();
    xcast_varray_add_str(value_, value);
  }

  void append_bytes(const uint8_t *data, uint32_t len) {
    init_varray();
    xcast_varray_add_buf(value_, data, len);
  }

  /* 'index' based array methods */
  xcast_data_t at(uint32_t index) {
    xcast_data_t data;
    if (value_) data.value_ = xcast_variant_array_index(value_, index);
    return data;
  }

  bool bool_at(uint32_t index) {
    return value_ ? xcast_varray_get_bool(value_, index) : false;
  }

  uint8_t uint8_at(uint32_t index) {
    return value_ ? xcast_varray_get_uint8(value_, index) : 0;
  }

  uint16_t uint16_at(uint32_t index) {
    return value_ ? xcast_varray_get_uint16(value_, index) : 0;
  }

  uint32_t uint32_at(uint32_t index) {
    return value_ ? xcast_varray_get_uint32(value_, index) : 0;
  }

  uint64_t uint64_at(uint32_t index) {
    return value_ ? xcast_varray_get_uint64(value_, index) : 0;
  }

  int16_t int16_at(uint32_t index) {
    return value_ ? xcast_varray_get_int16(value_, index) : 0;
  }

  int32_t int32_at(uint32_t index) {
    return value_ ? xcast_varray_get_int32(value_, index) : 0;
  }

  int64_t int64_at(uint32_t index) {
    return value_ ? xcast_varray_get_int64(value_, index) : 0;
  }

  float float_at(uint32_t index) {
    return value_ ? xcast_varray_get_float(value_, index) : 0.0f;
  }

  double double_at(uint32_t index) {
    return value_ ? xcast_varray_get_double(value_, index) : 0.0;
  }

  const char* str_at(uint32_t index) {
    return value_ ? xcast_varray_get_str(value_, index) : NULL;
  }

  const uint8_t *bytes_at(uint32_t index) {
      return value_ ? xcast_varray_get_buf(value_, index) : NULL;
  }

  /* directly access to internal variant object. */
  operator xcast_variant_t *() const { return value_; }

  /* attach to a new variant ptr without ref. */
  void attach(xcast_variant_t *var) {
    if (value_) xcast_variant_unref(value_);

    if (var) value_ = var;
  }

  /* directly return the internal variant without ref. */
  xcast_variant_t *detach() {
    xcast_variant_t *var = value_;
    value_ = NULL;
    return var;
  }

protected:
  void init_vdict() {
    if (!value_) {
      value_ = xcast_variant_dict_new();
    } else {
      value_ = xcast_variant_change_type(value_, xc_vtype_dict);
    }
  }

  void init_varray() {
    if (!value_) {
      value_ = xcast_variant_array_new();
    } else {
      value_ = xcast_variant_change_type(value_, xc_vtype_array);
    }
  }

protected:

  xcast_variant_t   *value_;
};

#endif /* XCAST_DATA_H_ */
