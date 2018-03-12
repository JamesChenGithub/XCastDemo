#ifndef XCAST_UI_HANDLER_H_
#define XCAST_UI_HANDLER_H_

#include "xcast-dev.h"

/*
* 控制XCAST在UI中的表现，可以在不同的平台上采用native UI实现以下方法来简化APP界面封装。
*/

/* 启动/停止XCAST */
int32_t ui_init_xcast(bool start, void *user_data);
int32_t ui_start_stream(const char *stream, bool start, void* user_data);

/* 从APP中获取XCAST启动参数 */
xcast_data xcast_get_start_param(void* user_data);
/* 从APP中获取媒体流启动参数 */
xcast_data xcast_get_stream_param(void* user_data);
/* 刷新UI中XCAST错误信息 */
void ui_xcast_err(int32_t err, const char *err_msg, void* user_data);

/* 刷新UI上媒体流状态 */
void ui_stream_connecting(const char *stream, void* user_data);
void ui_stream_connected(const char *stream, void* user_data);
void ui_stream_closed(const char *stream, int32_t err, const char *err_msg, void* user_data);

/* 刷新UI上媒体流轨道状态 */
void ui_track_add(xcast_data &evt, bool add, void *user_data);
void ui_track_update(xcast_data &evt, void *user_data);

/* 在UI上绘制视频流媒体数据 */
int32_t ui_track_media(xcast_data &evt, void *user_data);

/* 刷新UI上设备状态 */
void ui_device_added(const char *dev, int32_t clazz, bool add, void* user_data);
void ui_device_update(const char *camera, int32_t clazz,
  int32_t state, int32_t err, const char *err_msg, void* user_data);
/* 设备预处理 */
int32_t ui_device_preprocess(xcast_data &evt, void *user_data);
/* 设备预览 */
int32_t ui_device_preview(xcast_data &evt, void *user_data);

/* 注入外部数据 */
int32_t xcast_inject_video(const uint8_t *frame_data, uint32_t frame_size, int32_t width, int32_t height);
int32_t xcast_inject_audio(const uint8_t *audio_data, uint32_t data_size, uint32_t sample_rate, uint32_t channels, uint32_t bits);

int32_t ui_start_lan_relay(bool start, const char* ip, uint16_t port);

#endif  /* #ifndef XCAST_UI_HANDLER_H_ */