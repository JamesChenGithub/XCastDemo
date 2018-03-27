#ifndef	XCAST_DATA_H
#define XCAST_DATA_H

#include <string>
#include <vector>
#include <functional>

// 鉴权类型
typedef enum XCastAuthType {
	XCastAuth_None = 0,
	XCastAuth_Manual,
	XCastAuth_Auto,
} XCastAuthType;

// 内部事件类型
typedef enum XCastSystemEvent {
	XCastSystemEvent_Normal = 1,
	XCastSystemEvent_Warning,
	XCastSystemEvent_Error,
	XCastSystemEvent_Fatal,
} XCastSystemEvent;

// 多媒体格式
typedef enum XCastMediaFormat {
	XCastMedia_argb32 = 0,                /* argb32 video format */
	XCastMedia_i420,                      /* i420 video format */
	XCastMedia_aac,                       /* aac audio format */
	XCastMedia_pcm,                       /* pcm audio format */
	XCastMedia_layer = 0xFF,              /* layer format*/
} XCastMediaFormat;

// 多媒体来源
typedef enum XCastMediaSource {
	XCastMediaSource_Unknown = 0,        /* unknown source type */
	XCastMediaSource_Camera,             /* camera */
	XCastMediaSource_Screen_Capture,     /* screen capture */
	XCastMediaSource_Media_Player,       /* media player */
	// XCastMediaSource_PPT,                /* ppt */
} XCastMediaSource;

// 设备类型 
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

/* xcast device running state */
typedef enum  XCastDeviceState {
	XCastDeviceState_NotFound = 0,
	XCastDeviceState_Stopped = 1,
	XCastDeviceState_Running,
} XCastDeviceState;

typedef enum XCastEndpointEvent {
	XCast_Endpoint_NONE = 0, ///< 默认值，无意义。
	//XCast_Endpoint_Enter = 1, ///< 进入房间事件。
	XCast_Endpoint_Removed = 2, ///< 上行用户退出房间事件。						 
	XCast_Endpoint_Has_Camera_Video = 3, ///< 有发摄像头视频事件。
	XCast_Endpoint_No_Camera_Video = 4, ///< 无发摄像头视频事件。
	XCast_Endpoint_Has_Audio = 5, ///< 有发语音事件。
	XCast_Endpoint_No_Audio = 6, ///< 无发语音事件。
	XCast_Endpoint_Has_Screen_Video = 7, ///< 有发屏幕视频事件。
	XCast_Endpoint_No_Screen_Video = 8, ///< 无发屏幕视频事件。
	XCast_Endpoint_Has_Media_Video = 9, ///< 有发文件视频事件。
	XCast_Endpoint_No_Media_Video = 10, ///< 无发文件视频事件。
}XCastEndpointEvent;

//===========================================================
struct XCastDeviceHotPlugItem;
struct XCastVideoFrame;
struct XCastEndpoint;

#define kSupportIMAccount 1

#ifdef kSupportIMAccount
class XCastAccountHandler
{
public:

	typedef std::function<void(std::vector<uint64_t>, std::vector<std::string>, int, std::string)> XCastAccountCallBack;
	virtual bool useIMSDKasAccount() const = 0;
	virtual uint64_t getTinyId() = 0;
	virtual void tinyid_to_identifier(std::vector<uint64_t> tinyidlist, XCastAccountCallBack func) = 0;
	virtual void identifier_to_tinyid(std::vector<std::string> identifiedlist, XCastAccountCallBack func) = 0;
};
#endif

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
	virtual void onGlobalLocalVideoPreview(const XCastVideoFrame *frame) = 0;
};

class  XCastRoomHandler
{
public:
	virtual void onWillEnterRoom(int result, const char *error) = 0;
	virtual void onDidEnterRoom(int result, const char *error) = 0;
	virtual void onExitRoomComplete(int result, const char *error) = 0;
	virtual void onRoomDisconnected(int result, const char *error) = 0;

	virtual void onEndpointUpdateInfo(XCastEndpointEvent event,XCastEndpoint infos) = 0;
	virtual void onEndpointCaptureUpdate(XCastEndpointEvent event, XCastEndpoint infos) = 0;
	virtual void onEndpointRemoved(XCastEndpointEvent event, XCastEndpoint infos) = 0;
	virtual bool needRoomCallbackLocalVideo() = 0;
	virtual void onLocalVideoPreview(const XCastVideoFrame *frame) = 0;

	virtual void onVideoPreview(const XCastVideoFrame *frame) = 0;

	virtual bool needRoomCallbackTips() = 0;
	virtual void onStatTips() = 0;
};

typedef struct XCastStartParam {
#ifdef kSupportIMAccount
	std::string identifier="";  
#endif
	uint64_t tinyid = 0;
	bool isTestEvn = false;
	int32_t sdkappid = 0;
	uint32_t accounttype = 0;

	bool isVaild() const
	{
		if ((tinyid == 0) || sdkappid == 0 || accounttype == 0)
		{
			return false;
		}
		return true;
	}
#ifdef kSupportIMAccount
	bool isVaildIM() const
	{
		if ((identifier.length() == 0) || sdkappid == 0 || accounttype == 0)
		{
			return false;
		}
		return true;
	}
#endif
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
			if (expire_time < 0 || secret_key.length() == 0)
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
	bool ext_audio_playback = false;
}XCastStreamTrackInfo;


typedef struct XCastRoomOption {
	std::string captureCamera = "";
	std::string captureMic = "";
	std::string outputSpeaker = "";

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

	XCastRoomOption roomOpera;

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

typedef struct XCastVideoFrame
{
	uint64_t			tinyid = 0;
	std::string			identifier;
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


// 热插拔回调出来的事件
typedef struct XCastDeviceHotPlugItem {
	XCastDeviceState state = XCastDeviceState_Stopped;				// 状态
	XCastDeviceType deviceClass = XCastDeviceType_Unknown;			// 类型 
	std::string src = "";											// 名称
	int32_t err = 0;												// 出错码
	std::string err_msg = "";										// 出错信息	
}XCastDeviceHotPlugItem;

typedef struct XCastEndpoint {
	uint64_t tinyid = 0;
	std::string identifier;
	bool is_audio = false;
	bool is_camera_video = false;
	bool is_screen_video = false;
	bool is_media_video = false;
	

	bool isHas() const {
		return is_audio || is_camera_video || is_screen_video || is_media_video;
	}

}XCastEndpoint;


typedef struct XCastRequestViewItem
{
	uint64_t tinyid = 0;
	std::string identifer = "";
	XCastMediaSource video_src = XCastMediaSource_Unknown;

	bool isVaild() const
	{
		if (tinyid == 0 || video_src == XCastMediaSource_Unknown)
		{
			return false;
		}
		return true;
	}
}XCastRequestViewItem;

typedef std::function<void(int32_t, const char *)> XCHCallBack;
typedef std::function<void(XCastRequestViewItem, int32_t, const char *)> XCHReqViewListCallBack;
#define XCHNilCallBack NULL

#endif // XCAST_DATA_H