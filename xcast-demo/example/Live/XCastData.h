#pragma once

//#include "xcast_data.h"
#include "xcast.hh"
#include <string>

struct XCastDevice;
struct XCastVideoFrame;
struct XCastEndPoint;

class XCastGlobalHandler
{
public:
	virtual  ~XCastGlobalHandler() = 0;
public:

	// 设备回调
	virtual void onDeviceEvent_DeviceAdd(XCastDevice device) = 0;
	virtual void onDeviceEvent_DeviceUpdate(XCastDevice device) = 0;
	virtual void onDeviceEvent_DeviceRemoved(XCastDevice device) = 0;

	// XCAST系统回调
	virtual int32_t onSystemEvent(void *contextinfo, tencent::xcast_data &data) = 0;

	// 视频事件
	virtual bool needGlobalCallbackLocalVideo() = 0;
	virtual void onLocalVideoPreview(XCastVideoFrame *frame) = 0;
};

class  XCastRoomHandler
{
public:
	virtual  ~XCastRoomHandler() = 0;
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
	int32_t auth_type = 0;	
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
		if (auth_type == xc_auth_none)
		{
			return true;
		}
		else if (auth_type == xc_auth_manual)
		{
			return auth_buffer != nullptr && auth_buffer_size != 0;
		}
		else if (auth_type == xc_auth_auto)
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

typedef struct XCastVideoFrame
{
	uint64_t identifier = 0;
	int media_format = 1;
	uint8_t *data = nullptr;
	uint32_t size = 0;
	uint32_t width = 0;
	uint32_t height = 0;
	int rotate = 0;  //[0, 90, 180, 270]
	int direction;
	std::string deviceSrc;
}XCastVideoFrame;



typedef struct XCastDevice {

}XCastDevice;


typedef struct XCastEndPoint {

}XCastEndPoint;