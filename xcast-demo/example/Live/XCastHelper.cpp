#include "XCastHelper.h"
#include "include/xcast_variant.h"
#include "xcast.hh"
#include "include/xcast_variant.h"
#pragma comment(lib, "comctl32.lib")
XCastHelper * XCastHelper::m_instance = nullptr;

XCastHelper::XCastHelper()
{

}


XCastHelper::~XCastHelper()
{
	m_startup_param.reset();
	m_stream_param.reset();
	m_global_handler.reset();
	m_room_handler.reset();
}


int32_t XCastHelper::onXCastSystemEvent(void *contextinfo, void * data)
{

	return XCAST_OK;
}
int32_t XCastHelper::onXCastStreamEvent(void *contextinfo, void * data)
{
	return XCAST_OK;
}
int32_t XCastHelper::onXCastTrackEvent(void *contextinfo, void * data)
{
	return XCAST_OK;
}
int32_t XCastHelper::onXCastDeviceEvent(void *contextinfo, void * data)
{
	return XCAST_OK;
}
int32_t XCastHelper::onXCastTipsEvent(void *contextinfo, void * data)
{
	return XCAST_OK;
}

XCastHelper* XCastHelper::getInstance()
{
	/*static std::once_flag xcast_instance_flag;
	std::call_once(xcast_instance_flag, [](XCastHelper *instance) {
		m_instance = new XCastHelper;
	});


	return m_instance;*/
	// TODO:先简单写
	static bool once_flag = false;
	if (!once_flag)
	{
		m_instance = new XCastHelper;
		once_flag = true;
	}
	return m_instance;
	
}



//static void XCastHelper::unInit()
//{
//
//}


void XCastHelper::startContext(std::unique_ptr<XCastStartParam> param, std::function<void(int32_t, char *)> callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (is_startup_succ)
	{
		callback(1003, "xcast has started");
		return;
	}

	if (!param.get() || !param->isVaild())
	{
		callback(1004, "param error");
		return;
	}
	m_startup_param.reset();
	m_startup_param = std::move(param);
	// TODO：参数检查 

	tencent::xcast_data setparam;
	setparam["app_id"] = m_startup_param->sdkappid;
	setparam["identifier"] = m_startup_param->identifier;
	setparam["test_env"] =  m_startup_param->isTestEvn;
	int32_t rt = tencent::xcast::startup(setparam);

	is_startup_succ = (rt == XCAST_OK);
	///* 注册事件通知回调 */
	//xcast_handle_event(XC_EVENT_SYSTEM, XCastHelper::onXCastSystemEvent, (void *)m_instance);
	//xcast_handle_event(XC_EVENT_STREAM, XCastHelper::onXCastStreamEvent, m_instance);
	//xcast_handle_event(XC_EVENT_TRACK, XCastHelper::onXCastTrackEvent, m_instance);
	//xcast_handle_event(XC_EVENT_DEVICE, XCastHelper::onXCastDeviceEvent, m_instance);
	//xcast_handle_event(XC_EVENT_STATISTIC_TIPS, XCastHelper::onXCastTipsEvent, m_instance);


	callback(avsdkErrorCode(rt), is_startup_succ ? "xcast_startup succ" : "xcast_startup failed");

}
void XCastHelper::stopContext(std::function<void(int32_t, char *)> callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (is_stream_succ)
	{
		std::string sid = m_stream_param->streamID;
		int32_t ret = xcast_close_stream(sid.c_str());

		if (ret == XCAST_OK)
		{
			is_stream_succ = false;
		}
		else
		{
			callback(ret, "xcast cant stop streaming");
			return;
		}
	}

	xcast_shutdown();
	is_startup_succ = false;
	callback(XCAST_OK, "xcast shutdown succ");
}

void XCastHelper::setGlobalHandler(std::unique_ptr<XCastGlobalHandler>  handler)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	m_global_handler.reset();
	m_global_handler = std::move(handler);
}


void XCastHelper::enterRoom(std::unique_ptr<XCastStreamParam> param, std::unique_ptr<XCastRoomHandler>	roomDelegate, std::function<void(int32_t, char *)> callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	if (is_stream_succ)
	{
		callback(1003, "xcast is streaming");
		return;
	}

	if (!param.get() || !param->isVaild());
	{
		callback(1004, "xcast start stream param error");
		return;
	}

	m_stream_param.reset();
	m_stream_param = std::move(param);

	m_room_handler.reset();
	if (roomDelegate)
	{
		m_room_handler = std::move(roomDelegate);
	}
	


	// 参数配置
	xcast_data_t       params, auth_info, track;

	// 房间号，接收方式，角色
	params["relation_id"] = m_stream_param->roomid;
	params["auto_recv"] = m_stream_param->auto_recv;
	params["role"] = m_stream_param->role.c_str();

	params["auth_type"] = m_stream_param->auth_info.auth_type;
	auth_info["auth_bits"] = m_stream_param->auth_info.auth_bits;
	// 监权方式： 设置鉴权方式，以下三种选其一即可：
	if (m_stream_param->auth_info.auth_type == xc_auth_none)
	{
		// 方式1. 无鉴权，白名单方式
		// do nothing
	}
	else if (m_stream_param->auth_info.auth_type == xc_auth_manual)
	{
		// 方式2. 手动鉴权
		auth_info.put_bytes("auth_buffer", m_stream_param->auth_info.auth_buffer, m_stream_param->auth_info.auth_buffer_size);  // "xxxx" 为业务生成auth_buffer
	}
	else if (m_stream_param->auth_info.auth_type == xc_auth_manual)
	{
		auth_info["account_type"] = m_startup_param->accounttype;
		auth_info["expire_time"] = m_stream_param->auth_info.expire_time;
		const  char *secret_key = m_stream_param->auth_info.secret_key.c_str(); // 互动直播控制台上创建应该时生成的	
		auth_info.put_bytes("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
	}

	params.put("auth_info", auth_info);
	// 自定义采集（optional）,视频业务逻辑情况定
	track["ext-video-capture"] = m_stream_param->track.ext_video_capture;    /* allow video track to use external capture */
	track["ext-audio-capture"] = m_stream_param->track.ext_audio_capture;    /* allow audio track to use external capture */
	track["ext-audio-playback"] = m_stream_param->track.ext_audio_playback;   /* allow audio track to use external playback */
	params["track"] = track;

	int32_t  rt = xcast_start_stream(m_stream_param->streamID.c_str(), params);
	if (XCAST_OK != rt) {
		// TODO：进房失败；
		// ui_xcast_err(rt, xcast_err_msg(), user_data);

		callback(rt, (char *)xcast_err_msg());
		is_stream_succ = false;
		return;
	}
	callback(rt, "xcast_start_stream succ");
	is_stream_succ = true;

}
void XCastHelper::exitRoom(std::function<void(int32_t, char *)> callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (!is_stream_succ)
	{
		callback(1004, "xcast isn't streaming");
	}


	int32_t ret = xcast_close_stream(m_stream_param->streamID.c_str());

	if (ret != XCAST_OK)
	{
		callback(ret, (char *)xcast_err_msg());
	}
	else
	{
		is_stream_succ = true;
		callback(ret, "xcast close stream succ");
	}
}


// Speaker操作
int XCastHelper::enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback)
{

	/*xcast_variant_t defaultSpeaker = xcast_get_property(XC_SPEAKER_DEFAULT);
	xcast_data_t ds(defaultSpeaker);

	return enableSpeaker(ds["return"], enable, callback);*/

	return XCAST_OK;
}

// 指定默认的speaker 为 sid
// 同时进行enable操作
int enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}
// 切换输出类型 , speaker : 1 扬声器 0 耳机
int XCastHelper::changeOutputMode(bool speaker)
{
	return XCAST_OK;
}

// 获取当前Speaker状态，
int XCastHelper::getSpeakerState()
{
	return XCAST_OK;
}

// mic操作
// 对默认mic操作
int XCastHelper::enableMic(bool enable, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}

// 切换mic
int XCastHelper::switchToMic(const char *micid, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}

// 在房间内时，获取采集mic状态；
// 在房间外时，获取default mic状态;
int XCastHelper::getMicState()
{
	return XCAST_OK;
}

int XCastHelper::enableLoopBack(bool enable)
{
	return XCAST_OK;
}

// 摄像头操作

// 在房间内时，获取采集Camera状态；
// 在房间外时，获取default Camera状态;
int XCastHelper::getCameraState()
{
	return XCAST_OK;
}

// 在房间内时：打开摄像头，并预览，同时上行；
// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
int XCastHelper::enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}

int XCastHelper::enableCamera(const char *cameraid, bool preview, bool campture, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}

int XCastHelper::updateCameraMode(const char *cameraid, bool autoSending, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}

int XCastHelper::switchCamera(const char *cameraid, bool preview, bool campture, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}


int XCastHelper::avsdkErrorCode(int xcast_err_code)
{
	// TODO : xcast err code 转成AVSDK错误码
	return xcast_err_code;
}