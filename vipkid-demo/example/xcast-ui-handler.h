#ifndef XCAST_UI_HANDLER_H_
#define XCAST_UI_HANDLER_H_

#include "xcast-dev.h"

struct XCastVideoFrame;
struct XCastEndpoint;
enum XCastEndpointEvent;


 int32_t new_track_event(void *user_data, xcast_data &e);


 int32_t new_video_preview_event(void *user_data,const XCastVideoFrame *frame);

 extern void RenderBuffer();
 extern void InvalidVideoView(const RECT *rc = NULL);

/*
* 控制XCAST在UI中的表现，可以在不同的平台上采用native UI实现以下方法来简化APP界面封装。
*/

/* 启动/停止XCAST */
int32_t  new_ui_init_xcast(bool succ, void* user_data);

/* 刷新UI中XCAST错误信息 */
void ui_xcast_err(int32_t err, const char *err_msg, void* user_data);

/* 刷新UI上媒体流状态 */
void ui_stream_connecting(const char *stream, void* user_data);
void ui_stream_connected(const char *stream, void* user_data);
void ui_stream_closed(const char *stream, int32_t err, const char *err_msg, void* user_data);

void ui_track_update(const char *streamid, XCastEndpointEvent event,XCastEndpoint &endpoint, void *user_data);

/* 刷新UI上设备状态 */
void ui_device_added(const char *dev, int32_t clazz, bool add, void* user_data);
void ui_device_update(const char *camera, int32_t clazz,int32_t state, int32_t err, const char *err_msg, void* user_data);


#endif  /* #ifndef XCAST_UI_HANDLER_H_ */