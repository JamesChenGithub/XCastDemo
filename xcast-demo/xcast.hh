/*
** Copyright (c) 2014-2017 The xCast project. All rights reserved.
*/
#ifndef XCAST_HH_
#define XCAST_HH_

#include "xcast.h"

namespace tencent {

/*
* data type used by xcast for property operations.
* you can store basic types, such as 'bool', 'int', 'float', 'string' in
* a 'xcast_data' type.
* container types of 'array' and 'dictionary' are also supported.
* when 'xcast_data' is a container, all its elements have a type of 'xcast_data'.
*/
class xcast_data {
public:
  /* xcast_data constructors */
  explicit xcast_data() : value_(NULL) {}
  xcast_data(const xcast_data &other) : value_(NULL) {
    /* set new one if any */
    if (other.value_) {
      value_ = (xcast_variant_t *)other.value_;
      xcast_variant_ref(value_);
    }
  }

#define XCAST_DATA_TYPE(T, VT, VAL)  \
  xcast_data(T value) { value_ = xcast_variant_##VT##_new(value); } \
  operator T() { return value_ ? xcast_variant_##VT##_get(value_) : VAL; } \
  T VT##_val() { return value_ ? xcast_variant_##VT##_get(value_) : VAL; } \
  xcast_data &operator = (T value) { \
    if (!value_) {  \
      value_ = xcast_variant_##VT##_new(value); \
    } else {  \
      value_ = xcast_variant_##VT##_set(value_, value); \
    } \
    return *this; \
  } \
  bool operator == (T value) { \
    if (!value_ || type() != xc_vtype_##VT) return false; \
    return VT##_val() == value; \
  }

  /* xcast_data from basic types */
  XCAST_DATA_TYPE(bool, bool, false)
  XCAST_DATA_TYPE(uint8_t, uint8, 0)
  XCAST_DATA_TYPE(uint16_t, uint16, 0)
  XCAST_DATA_TYPE(int16_t, int16, 0)
  XCAST_DATA_TYPE(uint32_t, uint32, 0)
  XCAST_DATA_TYPE(int32_t, int32, 0)
  XCAST_DATA_TYPE(uint64_t, uint64, 0)
  XCAST_DATA_TYPE(float, float, 0.0f)
  XCAST_DATA_TYPE(double, double, 0.0)
  XCAST_DATA_TYPE(void *, ptr, NULL)

  xcast_data(const char* value) { value_ = xcast_variant_str_new(value); }
  explicit xcast_data(const uint8_t* value, uint32_t len) { 
    value_ = xcast_variant_buf_new(value, len);
  }

  ~xcast_data() { if (value_) xcast_variant_unref(value_); }

  /* container: whether 'xcast_data' is empty */
  bool empty() { return value_ ? xcast_variant_empty(value_) : true; }

  /* container: elements count in 'xcast_data' */
  uint32_t size() { return value_ ? xcast_variant_get_size(value_) : 0; }

  /* current 'xcast_data' type */
  char type() { return value_ ? (char)xcast_variant_get_type(value_) : 0; }
  /* dump data of xcast_variant, should free after use */
  char *dump() { return value_ ? xcast_variant_dump(value_) : NULL; }
  /* get basic type value from 'xcast_data' */
  operator const char *() { return value_ ? xcast_variant_str_get(value_) : NULL; }
  operator const uint8_t *() { return value_ ? xcast_variant_buf_get(value_) : NULL; }

  const char *str_val() { return value_ ? xcast_variant_str_get(value_) : NULL; }
  const uint8_t *bytes_val() { return value_ ? xcast_variant_buf_get(value_) : NULL; }

  /* assignment from other data types */
  xcast_data &operator = (const xcast_data &other) {
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

  xcast_data &operator = (const char *value) {
    if (!value_) {
      value_ = xcast_variant_str_new(value);
    } else {
      value_ = xcast_variant_str_set(value_, value);
    }

    return *this;
  }

  bool operator == (const char *value) {
    if (!value_ || type() != xc_vtype_string) return false;
    return xcast_variant_str_cmp(value_, value) ? false : true;
  }

  /* access 'xcast_data' as array type, if 'index' is larger
  * than array size, an empty data is returned. if you want to add new
  * element into the array, use the append methods instead.
  */
  xcast_data operator[](uint32_t index) {
    if (!value_ || type() != xc_vtype_array) return xcast_data();
    return at(index);
  }

  /* access 'xcast_data' as dictionary type, if 'key' is not exist
  * in the dictionary, an new data with type 'int32_t' is inserted into the
  * dictionary with the given key and returned.
  */
  xcast_data operator[](const char *key) {
    if (!key || !*key) return xcast_data();
    if (!value_ || type() != xc_vtype_dict) init_vdict();
    if (!contains(key)) put(key, xcast_data(0));
    return get(key);
  }

  /* assign a none-copied buffer to 'xcast_data' and take ownership of it */
  xcast_data &assign(uint8_t *data, uint32_t len) {
    if (value_) xcast_variant_unref(value_);
    value_ = xcast_variant_buf_owned_new(data, len);
    return *this;
  }

  /* 'key-value' dictionary methods */
  void put(const char *key, xcast_data data) {
    if (!data.value_) return;
    init_vdict();
    xcast_variant_dict_set(value_, key, data.value_);
  }

  bool contains(const char *key) {
    if (!value_ || type() != xc_vtype_dict) return false;
    return xcast_variant_dict_contains(value_, key);
  }

  /* put a copied buffer into 'xcast_data' */
  void put(const char *key, const uint8_t *data, uint32_t len) {
    init_vdict();
    xcast_vdict_set_buf(value_, key, data, len);
  }

  /* put a none-copied buffer into 'xcast_data' and take ownership of it */
  void put_owned(const char *key, uint8_t *data, uint32_t len) {
    init_vdict();
    xcast_vdict_set_buf_owned(value_, key, data, len);
  }

  /* 'key-value' dictionary methods */
  xcast_data get(const char *key) {
    xcast_data data;
    if (value_) data.value_ = xcast_variant_dict_get(value_, key);
    return data;
  }

  /* 'index' based array methods */
  void append(xcast_data data) {
    if (!data.value_) return;
    init_varray();
    xcast_variant_array_append(value_, data.value_);
  }

  void append(const uint8_t *data, uint32_t len) {
    init_varray();
    xcast_varray_add_buf(value_, data, len);
  }

  /* 'index' based array methods */
  xcast_data at(uint32_t index) {
    xcast_data data;
    if (value_) data.value_ = xcast_variant_array_index(value_, index);
    return data;
  }

protected:
  friend class xcast;
  explicit xcast_data(xcast_variant_t *val) : value_(val) {}

  /* directly access to internal variant object. */
  operator xcast_variant_t *() const { return value_; }

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

#define XCAST_MAX_PATH      260

/*
* xcast wrapper class for c++.
*/
class xcast {
 public:
  static const char *version(void) { return xcast_version(); }
  static const char *err_msg(void) { return xcast_err_msg(); }
  static int32_t startup(xcast_data params) { 
    params["event-fire"] = (void *)&xcast::fire_event;
    return xcast_startup(params);
  }

  static void shutdown(void) { return xcast_shutdown(); }

  static xcast_data get_property(const char *prop) {
    return xcast_data(xcast_get_property(prop));
  }

  static xcast_data get_property(const char *prop, const char *id) {
    return xcast_data(xcast_get_property(format(prop, id)));
  }

  static xcast_data get_property(const char *prop, const char *id1, const char *id2) {
    return xcast_data(xcast_get_property(format(prop, id1, id2)));
  }

  static xcast_data get_property(
    const char *prop, const char *id1, const char *id2, const char *id3) {
    return xcast_data(xcast_get_property(format(prop, id1, id2, id3)));
  }

  static int32_t set_property(const char *prop, xcast_data val) {
    return xcast_set_property(prop, val);
  }

  static int32_t set_property(const char *prop, const char *id, xcast_data val) {
    return xcast_set_property(format(prop, id), val);
  }

  static int32_t set_property(
    const char *prop, const char *id1, const char *id2, xcast_data val) {
    return xcast_set_property(format(prop, id1, id2), val);
  }

  static int32_t set_property(
    const char *prop, const char *id1, const char *id2, const char *id3, xcast_data val) {
    return xcast_set_property(format(prop, id1, id2, id3), val);
  }

  typedef int32_t(*xcast_event_handler)(void *user_data, xcast_data &e);

  static int32_t handle_event(
    const char *event_path, xcast_event_handler handler, void *user_data) {
    return xcast_handle_event(event_path, (xcast_func_pt)handler, user_data);
  }
  
  static int32_t start_stream(const char *id, xcast_data params) {
    return xcast_start_stream(id, params);
  }

  static int32_t close_stream(const char *id) { return xcast_close_stream(id); }

  static void cycle(bool block) { return xcast_cycle(block); }

  static void cycle_break() { return xcast_cycle_break(); }

protected:
  static int32_t fire_event(xcast_event_handler h, xcast_variant_t *e, void *user_data) {
    xcast_data        data;

    data.value_ = e;
    h(user_data, data);
    data.value_ = NULL;

    return XCAST_OK;
  }

  /* format null terminated string with a max size of '260' */
  static const char *format(const char *fmt, ...) {
    static char       path[XCAST_MAX_PATH] = {0};
    va_list           arg;
    int32_t           len;

    if (!fmt || !*fmt) return NULL;

    va_start(arg, fmt);
    len = vsnprintf(path, XCAST_MAX_PATH, fmt, arg);
    va_end(arg);
    if (len <= 0) return NULL;

    return path;
  }

};

} /* namespace tencent */

#endif /* XCAST_HH_ */
