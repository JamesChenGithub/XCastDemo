#include "xcast-ui-handler.h"
#include "include/xcast_variant.h"


extern void utf8_to_utf16(const char *str, uint32_t len, std::wstring& wstr);

static void    *user_data;
static int32_t on_xcast_event(void *user_data, xcast_data &e);
static int32_t on_stream_event(void *user_data, xcast_data &e);
static int32_t on_track_event(void *user_data, xcast_data &e);
static int32_t on_device_event(void *user_data, xcast_data &e);
static int32_t on_stat_tips(void *user_data, xcast_data &e);

//===========================================================
int32_t  new_ui_init_xcast(bool succ, void* user_data)
{
	if (succ) {
		///* 注册事件通知回调 */
		//xcast::handle_event(XC_EVENT_SYSTEM, on_xcast_event, user_data);
		//xcast::handle_event(XC_EVENT_STREAM, on_stream_event, user_data);
		//xcast::handle_event(XC_EVENT_TRACK, on_track_event, user_data);
		//xcast::handle_event(XC_EVENT_DEVICE, on_device_event, user_data);
		//xcast::handle_event(XC_EVENT_STATISTIC_TIPS, on_stat_tips, user_data);
	}
	else {
		ui_xcast_err(-1000, "startup fail", NULL);
	}
	return 0;
}

int32_t new_xcast_event(void *user_data, xcast_data &e)
{
	return on_xcast_event(&main_app, e);
}

int32_t new_stream_event(void *user_data, xcast_data &e)
{
	return on_stream_event(&main_app, e);
}
int32_t new_track_event(void *user_data, xcast_data &e)
{
	return on_track_event(&main_app, e);
}
int32_t new_device_event(void *user_data, xcast_data &e)
{
	return on_device_event(&main_app, e);
}
int32_t new_stat_tips(void *user_data, xcast_data &e)
{
	return on_stat_tips(&main_app, e);
}
//============================================
int32_t 
ui_init_xcast(bool start, void* user_data)
{
  int32_t     rt = XCAST_OK;

  if (start) {
    /* 准备启动参数 */
    xcast_data settings = xcast_get_start_param(user_data);

    /* 启动XCAST */
    rt = xcast::startup(settings);
    if (rt == XCAST_OK) {
      /* 注册事件通知回调 */
      xcast::handle_event(XC_EVENT_SYSTEM, on_xcast_event, user_data);
      xcast::handle_event(XC_EVENT_STREAM, on_stream_event, user_data);
      xcast::handle_event(XC_EVENT_TRACK, on_track_event, user_data);
      xcast::handle_event(XC_EVENT_DEVICE, on_device_event, user_data);
      xcast::handle_event(XC_EVENT_STATISTIC_TIPS, on_stat_tips, user_data);
    } else {
      ui_xcast_err(rt,"startup fail", NULL);
    }
  } else {
    /* 停止XCAST */
    xcast::shutdown();
  }

  return rt;
}

int32_t 
ui_start_stream(const char *stream, bool start, void* user_data)
{
  if (start) {
    int32_t     rt;
    xcast_data   params = xcast_get_stream_param(user_data);
	char *pstr = params.dump();
    rt = xcast::start_stream(stream, params);
    if (XCAST_OK != rt) {
      ui_xcast_err(rt, xcast_err_msg(), user_data);
    }
    return rt;
  } else {
    xcast::close_stream(stream);
  }
  return XCAST_OK;
}

/* xcast系统事件通知 */
int32_t 
on_xcast_event(void *user_data, xcast_data &e)
{
  const char             *err_msg;
  int32_t                 err;

  err = e["err"];
  err_msg = e["err-msg"];
  ui_xcast_err(err, err_msg, user_data);

  return XCAST_OK;
}

/* 流状态通知： 更新UI中媒体流状态 */
static int32_t
on_stream_event(void *user_data, xcast_data &e)
{
	char * str = e.dump();
	if (str)
		printf("%s\n", str);
  switch ((int32_t)e["type"]) {
  case xc_stream_added:
 {
   ///* set peer addr */
   // xcast_data_t  peer;
   // int32_t ret;

   // //peer["ip"] = "192.168.43.106";
   // //peer["ip"] = "192.168.43.124";
   // peer["ip"] = "192.168.43.106";
   // peer["port"] = 9000;
   // ret = xcast_set_property("lan-relay.peer-addr", peer);
   // if (ret != XCAST_OK) {
   //   /* process error */
   // }
  }
  
   /* 新增媒体流, 初始化流状态： 连接中 */
    ui_stream_connecting(e["src"], user_data);
    break;
  case xc_stream_updated:
    if (e["state"] == xc_stream_connected) {
      /* 流状态： 连接成功 */
		
		ui_stream_connected(e["src"], user_data);
    }

    break;
  case xc_stream_removed:
    /* 流状态： 关闭, 移除媒体流 */
    ui_stream_closed(e["src"], e["err"], e["err-msg"], user_data);
    break;
  default:
    break;
  }

  return XCAST_OK;
}

/* 流轨道状态通知： 更新UI中媒体流轨道状态 */
static int32_t
on_track_event(void *user_data, xcast_data &e)
{
  switch ((int32_t)e["type"]) {
  case xc_track_added:
  {
	  /* 新增轨道 */
	  ui_track_add(e, true, user_data);
	 
  }
    break;
  case xc_track_updated:
  case xc_track_capture_changed:
    /* 更新轨道 */
    ui_track_update(e, user_data);
    break;
  case xc_track_removed:
    ui_track_add(e, false, user_data);
    break;
  case xc_track_media: {
	  ui_track_media(e, user_data);
	  //xcast_data data = xcast::get_property("stream.stream1.track");
	  //char *str = data.dump();


  }
    

    break;
  default:
    break;
  }

  return XCAST_OK;
}

/* 设备事件通知： 更新UI中设备状态 */
static int32_t
on_device_event(void *user_data, xcast_data &e)
{
	//char *str = e.dump();
	//printf("%s", str);
	//static bool is_Setdefault = false;
  switch ((int32_t)e["type"]) {
  case xc_device_added:
    /* 设备插入 */
    ui_device_added(e["src"], e["class"], true, user_data);

	/*if (e["class"] == xc_device_camera && e["src"] == "11Webcam C170")
	{
		int ret = xcast::set_property(XC_CAMERA_DEFAULT, e["src"]);
		int i = 0;
	}*/

    break;
  case xc_device_updated:
    /* 设备更新 */
    ui_device_update(e["src"], e["class"], e["state"], e["err"], e["err_msg"], user_data);

#if defined(XCAST_EXTERNAL_VIDEO)
    dev = (const char *)evt["src"];
    state = evt["state"];
    if (dev == "ext1") {
      if (state == xc_device_running) {
        /* TODO: start external capture here */
        xcast_inject_video((const uint8_t *)0xFFFFFFFF, 4096, 480, 320);
      } else if (state == xc_device_stopped) {
        /* TODO: stop external capture here */

      }
    }
#endif

    break;
  case xc_device_removed:
    /* 设备拔出 */
    ui_device_added(e["src"], e["class"], false, user_data);
    break;
  case xc_device_preprocess:
    /* 设备预处理 */
    ui_device_preprocess(e, user_data);
    break;
  case xc_device_preview:
    /* 设备预览 */
    ui_device_preview(e, user_data);
    break;
  default:
    break;
  }

  return XCAST_OK;
}

extern void RenderBuffer();
extern void InvalidVideoView(const RECT *rc = NULL);
static int32_t
on_stat_tips(void *user_data, xcast_data &e)
{
  const char         *tips;

  tips = e["tips"];
  if (tips) {
    utf8_to_utf16(tips, strlen(tips), main_app.tips);
    InvalidVideoView();
  }

  return XCAST_OK;
}

/* TODO: inject external data here
* ATTENTION: this function must be called at main-ui-thread, so do not block here.
*/
int32_t 
xcast_inject_video(const uint8_t *frame_data, uint32_t frame_size, int32_t width, int32_t height)
{
#if defined(XCAST_EXTERNAL_VIDEO)
  xcast_data     data;


  data["width"] = width;
  data["height"] = height;
  data["format"] = xc_media_i420;
  data["size"] = frame_size;
  /* this will avoid copy of frame data */
  data["data"] = (uint64_t)frame_data;
  data["sub-frame-count"] = sub_frame_count;
  data["sub-frame"] = sub_frame;

  return xcast::set_property(XC_DEVICE_EXTERNAL_INPUT, data);
#else
  return 0;
#endif
}

int32_t
xcast_inject_audio(const uint8_t *frame_data, uint32_t frame_size,
  uint32_t sample, uint32_t channel, uint32_t bits)
{
  xcast_data      prop, data;

/*
  if (!xcast_passive || !frame_data || !frame_size) return XCAST_ERR_FAILED;

  data["format"] = xc_media_pcm;
  data.put_bytes("data", frame_data, frame_size);
  data["size"] = frame_size;
  data["sample-rate"] = sample;
  data["channel"] = channel;
  data["bits"] = bits;

  prop.format(XC_DEVICE_EXTERNAL_INPUT, AUDIO_EXTERNAL_CAPTURE);
  data.put_str("prop", prop.str_val());

  return xcast_execute_safe(xcast_passive, XCAST_SET_PROPERTY, data);
  */
return 0;
}


int32_t ui_start_lan_relay(bool start, const char* ip, uint16_t port)
{
  /* set peer addr */
   xcast_data  peer;
   int32_t ret;

   if (start) {
     peer["ip"] = ip;
     peer["port"] = port;
   } else {
     peer["ip"] = "";
     peer["port"] = 0;
   }
   ret = xcast::set_property("lan-relay.peer-addr", peer);
   return ret;
}