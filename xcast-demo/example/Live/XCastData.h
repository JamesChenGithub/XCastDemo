#pragma once

//#include "xcast_data.h"
#include <string>


/*xcast auth type*/
typedef enum XCastAuthType {
	XCastAuth_None = 0,
	XCastAuth_Manual,
	XCastAuth_Auto,
} XCastAuthType;

/* xcast system events */
typedef enum XCastSystemEvent {
	XCastSystemEvent_Normal = 1,
	XCastSystemEvent_Warning,
	XCastSystemEvent_Error,
	XCastSystemEvent_Fatal,
} XCastSystemEvent;

/* xcast stream events */
typedef enum XCastStreamEvent {
	XCastStream_Added = 1,                /* new stream added */
	XCastStream_Updated,                  /* stream state updated */
	XCastStream_Removed,                  /* stream removed */
} XCastStreamEvent;

/* xcast stream events */
typedef enum  XCastStreamState {
	XCastStreamState_Connecting = 1,           /* stream connecting */
	XCastStreamState_Connected,                /* stream connected */
	XCastStreamState_Closed,                   /* stream connected */
} XCastStreamState;

/* xcast stream track events */
typedef enum XCastTrackEvent {
	XCastTrack_Added = 1,                 /* new track added */
	XCastTrack_Updated,                   /* track state updated */
	XCastTrack_Removed,                   /* track removed */
	XCastTrack_Capture_Changed,           /* track capture changed */
	XCastTrack_Media,                     /* track media data coming */
} XCastTrackEven;

/* xcast stream track running state */
typedef enum XCastTrackState {
	XCastTrackState_Stopped = 1,               /* track stopped */
	XCastTrackState_Running,                   /* track is running */
} XCastTrackState;

/* xcast track types */
typedef enum XCastTrackType {
	XCastTrackType_Msg = 1,                   /* msg track */
	XCastTrackType_Audio,                     /* audio track */
	XCastTrackType_Video,                     /* video track */
} XCastTrackType;

/* xcast track types */
typedef enum XCastTrackDirection {
	XCastTrackDirection_None = 0,			// Preview???????
	XCastTrackDirection_Out = 1,                   /* send media data to remote host */
	XCastTrackDirection_In,                        /* receive media data from remote host */
} XCastTrackDirection;

/* xcast media types */
typedef enum XCastMediaFormat {
	XCastMedia_argb32 = 0,                /* argb32 video format */
	XCastMedia_i420,                      /* i420 video format */
	XCastMedia_aac,                       /* aac audio format */
	XCastMedia_pcm,                       /* pcm audio format */
	XCastMedia_layer = 0xFF,              /* layer format*/
} XCastMediaFormat;

/* xcast media source types */
typedef enum XCastMediaSource {
	XCastMediaSource_Unknown = 0,        /* unknown source type */
	XCastMediaSource_Camera,             /* camera */
	XCastMediaSource_Screen_Capture,     /* screen capture */
	XCastMediaSource_Media_Player,       /* media player */
	XCastMediaSource_PPT,                /* ppt */
} XCastMediaSource;

/* xcast device running state */
typedef enum XCastDeviceType {
	XCastDeviceType_Unknown = 0,              /* unknown device type */
	XCastDeviceType_Camera,                   /* camera device */
	XCastDeviceType_Screen_Capture,           /* screnn capture */
	XCastDeviceType_Player,                   /* media player */
	XCastDeviceType_Mic,                      /* microphone device */
	XCastDeviceType_Speaker,                  /* speaker device */
	XCastDeviceType_Accompany,                /* audio acompany device */
	XCastDeviceType_External,                 /* external device type */
} XCastDeviceType;

/* xcast stream track events */
typedef enum XCastDeviceEvent {
	XCastDeviceEvent_Added = 1,                /* new device added */
	XCastDeviceEvent_Updated,                  /* device state updated */
	XCastDeviceEvent_Removed,                  /* device removed */
	XCastDeviceEvent_Preprocess,               /* device preprocess data coming */
	XCastDeviceEvent_Preview,                  /* device preview data coming */
} XCDeviceEvent;

/* xcast device running state */
typedef enum  XCastDeviceState {
	XCastDeviceState_NotFound = 0,
	XCastDeviceState_Stopped = 1,
	XCastDeviceState_Running,
} XCastDeviceState;



//===========================================================
struct XCastDeviceHotPlugItem;
struct XCastVideoFrame;
struct XCastEndPoint;

class XCastGlobalHandler
{
public:

	// 设备热插拔
	virtual void onDeviceEvent_DeviceAdd(XCastDeviceHotPlugItem deviceItem) = 0;
	virtual void onDeviceEvent_DeviceUpdate(XCastDeviceHotPlugItem deviceItem) = 0;
	virtual void onDeviceEvent_DeviceRemoved(XCastDeviceHotPlugItem deviceItem) = 0;

	// XCAST系统回调
	//virtual void onSystemEvent(void *contextinfo, tencent::xcast_data &data) = 0;
	virtual void onSystemEvent() = 0;
	// 视频事件
	virtual bool needGlobalCallbackLocalVideo() = 0;
	virtual void onGlobalLocalVideoPreview(XCastVideoFrame *frame) = 0;
};

class  XCastRoomHandler
{
public:
	//virtual void onWillEnterRoom(int result, const char *error) = 0;
	virtual void onDidEnterRoom(int result, const char *error) = 0;
	virtual void onExitRoomComplete(int result, const char *error) = 0;
	virtual void onRoomDisconnected(int result, const char *error) = 0;

	virtual void onEndpointsUpdateInfo(XCastEndPoint info) = 0;
	virtual bool needRoomCallbackLocalVideo() = 0;
	virtual void onLocalVideoPreview(XCastVideoFrame *frame) = 0;

	virtual void onVideoPreview(XCastVideoFrame *frame) = 0;

	virtual bool needRoomCallbackTips() = 0;
	virtual void onStatTips() = 0;
};

typedef struct XCastStartParam {
	uint64_t identifier = 0;
	bool isTestEvn = false;
	int32_t sdkappid = 0;
	uint32_t accounttype = 0;

	bool isVaild() const
	{
		if (identifier == 0 || sdkappid == 0 || accounttype == 0)
		{
			return false;
		}
		return true;
	}
} XCastStartParam;



typedef struct XCastStreamAuthInfo
{
	XCastAuthType auth_type = XCastAuth_None;
	uint8_t *auth_buffer = nullptr;
	uint8_t auth_buffer_size = 0;
	int32_t auth_bits = -1;
	//uint32_t account_type;
	uint32_t expire_time = 1800;
	std::string secret_key = "";

	bool isVaild() const
	{
		if (auth_bits == 0)
		{
			return false;
		}
		//xc_auth_none = 0,
		//	xc_auth_manual,
		//	xc_auth_auto
		if (auth_type == XCastAuth_None)
		{
			return true;
		}
		else if (auth_type == XCastAuth_Manual)
		{
			return auth_buffer != nullptr && auth_buffer_size != 0;
		}
		else if (auth_type == XCastAuth_Auto)
		{
			if (expire_time < 0 || secret_key.length() == 0 )
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}XCastStreamAuthInfo;


typedef struct XCastStreamTrackInfo
{
	bool ext_video_capture = false;
	bool ext_audio_capture = false;
	bool ext_audio_playback = true;
}XCastStreamTrackInfo;


typedef struct XCastRoomOpera {
	std::string defaultCamera = "";
	std::string defaultMic = "";
	std::string defaultSpeaker = "";

	bool autoCameraCapture = true;
	bool autoCameraPreview = true;
	bool autoMic = true;
	bool autoSpeaker = true;
	bool autoCloseCameraOnExit = true;

}XCastStreamDevice;

typedef struct XCastStreamParam {
	int32_t roomid;
	std::string role;
	bool auto_recv = true;
	std::string streamID;
	XCastStreamAuthInfo auth_info;
	XCastStreamTrackInfo track;

	// TODO:补投屏，以及局域网通信字段 

	XCastRoomOpera roomOpera;

	bool isVaild() const
	{
		if (roomid == 0 || role.length() == 0 || streamID.length() == 0)
		{
			return false;
		}

		bool isv = auth_info.isVaild();
		return isv;
	}

} XCastStreamParam;


///* xcast media types */
//typedef enum xc_media_format_e {
//	xc_media_argb32 = 0,                /* argb32 video format */
//	xc_media_i420,                      /* i420 video format */
//	xc_media_aac,                       /* aac audio format */
//	xc_media_pcm,                       /* pcm audio format */
//	xc_media_layer = 0xFF,              /* layer format*/
//} xc_media_format;

/* xcast track types */
//typedef enum xc_track_direction_e {
//	xc_track_out = 1,                   /* send media data to remote host */
//	xc_track_in,                        /* receive media data from remote host */
//} xc_track_direction;
//
//
//
//
//
//return XCAST_OK;

typedef struct XCastVideoFrame
{
	uint64_t			tinyid = 0;
	//std::string			identifier;
	XCastMediaFormat	media_format = XCastMedia_argb32;
	XCastMediaSource	media_source = XCastMediaSource_Unknown;
	uint8_t *data = nullptr;
	uint32_t size = 0;
	uint32_t width = 0;
	uint32_t height = 0;
	int rotate = 0;  // [0, 90, 180, 270]
	int direction;
	std::string deviceSrc;

	~XCastVideoFrame()
	{
		if (data)
		{
			free(data);
			data = nullptr;
		}
	}
}XCastVideoFrame;

/*
* xcast支持的设备事件
* "event.device":{
*   // 事件源: 设备名
*   "*src":"vstring",
*   // 事件类型: 新增，更新，删除,预处理数据,预览数据
*   "*type":[xc_device_added,xc_device_updated,
xc_device_removed,xc_device_preprocess,xc_device_preview],
*   // 设备类型: xc_device_type
*   "*class":[xc_device_camera,xc_device_screen_capture,xc_device_player
*     xc_device_mic,xc_device_speaker,xc_device_external],
*   // 事件状态: 运行，停止
*   "state":[xc_device_running,xc_device_stopped],
*   // 错误代码
*   "err":"vint32",
*   // 错误信息
*   "err-msg":"vstring",
*   // 数据格式
*   "format":[xc_media_argb32,xc_media_i420,xc_media_aac],
*   // 数据缓存
*   "data":"vbytes",
*   // 数据缓存长度
*   "size":"vuint32",
*   // 数据宽度
*   "width":"vint32",
*   // 数据高度
*   "height":"vint32",
*   // 数据旋转
*   "rotate":[0,90,180,270],
*   // 音频设备：音量值，取值范围[0,100]
*   "volume":"vuint32",
*   // 音频设备：动态音量值，取值范围[0,100]
*   "dynamic-volume":"vuint32",
*   // 播放器路径
*   "player-path":"vstring",
*   // 音频文件路径
*   "file-path":"vstring",
*   // 播放进度
*   "current-pos":"vint64",
*   // 最大进度
*   "max-pos":"vint64",
*   // 伴奏源: 默认值,系统伴奏,应用伴奏(播放器进程)
*   "accompany-source":["none","system","process"],
*   // 左边界
*   "screen-left":"vint32",
*   // 上边界
*   "screen-top":"vint32",
*   // 右边界
*   "screen-right":"vint32",
*   // 下边界
*   "screen-bottom":"vint32",
*   // 捕获帧率
*   "screen-fps":"vuint32",
*   // 窗口句柄, 仅支持win
*   "screen-hwnd":"vint32"
* },
*/
 //"[\"state\":int32(1),\"err\":int32(0),\"type\":int32(1),\"class\":int32(7),\"src\":\"ext1\"]"	char *

// 热插拔回调出来的事件

typedef struct XCastDeviceHotPlugItem {

	XCastDeviceState state = XCastDeviceState_Stopped;				// 状态
	XCastDeviceType deviceClass = XCastDeviceType_Unknown;			// 类型 
	std::string src = "";											// 名称
	int32_t err = 0;												// 出错码
	std::string err_msg = "";									// 出错信息	
}XCastDeviceHotPlugItem;


typedef struct XCastEndPoint {

}XCastEndPoint;