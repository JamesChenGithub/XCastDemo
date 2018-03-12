/*
** Copyright (c) 2014-2017 The xcast project. All rights reserved.
*/
#ifndef XCAST_H_
#define XCAST_H_

#include "./include/xcast_define.h"
#include "./include/xcast_variant.h"
#include "./include/xcast_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* TODO(element): !!!重要!!!
* 1、增加接口返回值错误代码上报
* 2、分析接口错误用法，日志自动化分析
* 2、所有的错误都要上报
*/

/*
* query current xcast string version.
*/
xcast_export const char *xcast_version(void);

/*
* query current xcast error message.
*/
xcast_export const char *xcast_err_msg(void);

/*
* start xcast before xcast streaming.
*/
xcast_export int32_t xcast_startup(xcast_variant_t *settings);

/*
* shutdown xcast after streaming.
*/
xcast_export void xcast_shutdown(void);

/*
* get property with the given property path.
*/
xcast_export xcast_variant_t *xcast_get_property(const char *prop);

/*
* set property with the given property path.
*/
xcast_export int32_t xcast_set_property(const char *prop, xcast_variant_t *val);

/*
* execute with the given path.
*/
xcast_export int32_t xcast_execute(const char *path, xcast_variant_t *val);

/*
* register/unregister event callback into xcast to receive event notify.
* if the 'handler' is null, the callback is unregistered from xcast.
*/
xcast_export int32_t xcast_handle_event(
  const char *event_path, xcast_func_pt handler, void *user_data);

/* 
* start xcast streaming with params. 
*/
xcast_export int32_t xcast_start_stream(const char *id, xcast_variant_t *params);

/* 
* close xcast streaming. 
*/
xcast_export int32_t xcast_close_stream(const char *id);

/* 
* only for passive mode, drive xcast cycle.
* if block set false, you should call xcast_cycle some where to drive xcast.
*/
xcast_export void xcast_cycle(bool block);

/* 
* only for passive mode, break xcast cycle while xcast_cycle block param set to true.
*/
xcast_export void xcast_cycle_break();
    
#ifdef __cplusplus
}
#endif

#endif /* XCAST_H_ */
