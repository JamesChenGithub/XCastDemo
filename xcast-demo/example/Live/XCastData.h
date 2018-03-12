#pragma once

#include "xcast_data.h"
#include "xcast_define.h"
#include <string>

class XCastGlobalHandler
{
public:
	virtual  ~XCastGlobalHandler() = 0;
};

class  XCastRoomHandler
{
public:
	virtual  ~XCastRoomHandler() = 0;
public:
	virtual void onWillEnterRoom(int result, const char *error) = 0;
	virtual void onDidEnterRoom(int result, const char *error) = 0;
	virtual void onExitRoomComplete() = 0;
	virtual void onRoomDisconnected(int result, const char *error) = 0;

	//virtual void onEndpointsUpdateInfo(int eventid, std::vector<>)

	virtual void onLocalVideoPreview() = 0;
	virtual void onVideoPreview() = 0;
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


typedef struct XCastStreamDevice {
	std::string defaultCamera = "";
	std::string defaultMic = "";
	std::string defaultSpeaker = "";

	bool autoCamera = false;
	bool autoMic = false;
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