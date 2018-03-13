#include "XCastHelper.h"
#include "include/xcast_variant.h"
#include "xcast.hh"


#if kForVipKidTest
#include "example/xcast-ui-handler.h"
#endif


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


int32_t XCastHelper::onXCastSystemEvent(void *contextinfo, tencent::xcast_data &data)
{
#if kForVipKidTest
	new_xcast_event(NULL, data);
#endif
	XCastHelper *instance = (XCastHelper *)contextinfo;

	if (instance && instance->m_global_handler.get())
	{
		instance->m_global_handler->onSystemEvent(contextinfo, data);
	}

	// TODO: 添加回调 
	return XCAST_OK;
}
int32_t XCastHelper::onXCastStreamEvent(void *contextinfo, tencent::xcast_data &data)
{
#if kForVipKidTest
	new_stream_event(NULL, data);
#endif

	XCastHelper *instance = (XCastHelper *)contextinfo;
	char * str = data.dump();
	if (str)
		printf("%s\n", str);
	switch ((int32_t)data["type"]) {
	case xc_stream_added:
	{
		if (instance && instance->m_room_handler)
		{
			instance->m_room_handler->onWillEnterRoom(0, NULL);
		}
	}
	break;
	case xc_stream_updated:
	{
		if (data["state"] == xc_stream_connected) {
			instance->stream_state = Room_Connectted;
			/* 流状态： 连接成功 */
			if (instance && instance->m_room_handler)
			{
				instance->m_room_handler->onDidEnterRoom(0, NULL);
			}

			if (instance->m_stream_param.get())
			{
				const XCastRoomOpera opera = instance->m_stream_param->roomOpera;

				instance->enableCamera(opera.defaultCamera.c_str(), opera.autoCameraCapture, opera.autoCameraPreview);
				instance->enableMic(opera.defaultMic.c_str(), opera.autoMic);
				instance->enableSpeaker(opera.defaultSpeaker.c_str(), opera.autoSpeaker);
			}
		}
	}
	break;
	case xc_stream_removed:
	{
		/* 流状态： 关闭, 移除媒体流 */
		//ui_stream_closed(e["src"], e["err"], e["err-msg"], user_data);

		if (instance)
		{
			if (instance->stream_state == Room_Connectted)
			{
				// 退房
				if (instance->m_room_handler)
				{
					instance->m_room_handler->onExitRoomComplete((int)(data["err"]), (const char *)(data["err-msg"]));
				}
				instance->clearAfterExitRoom();
			}
			else if (instance->stream_state == Room_Connecting)
			{
				// 进房出现问题
				if (instance->m_room_handler)
				{
					instance->m_room_handler->onDidEnterRoom((int)(data["err"]), (const char *)(data["err-msg"]));
				}
				instance->clearAfterExitRoom();
			}
		}
	}
	break;
	default:
		break;
	}
	return XCAST_OK;
}
int32_t XCastHelper::onXCastTrackEvent(void *contextinfo, tencent::xcast_data &data)
{
#if kForVipKidTest
	new_track_event(NULL, data);
#endif

	switch ((int32_t)data["type"])
	{
	case xc_track_added:
	{
		/* 新增轨道 */
		/*ui_track_add(e, true, user_data);*/
	}
	break;
	case xc_track_updated:
	{

	}
	break;
	case xc_track_capture_changed:
	{
	}
	/* 更新轨道 */
	/*ui_track_update(e, user_data);*/
	break;
	case xc_track_removed:
	{

	}
	//ui_track_add(e, false, user_data);
	break;
	case xc_track_media: {
		//ui_track_media(e, user_data);
	}
						 break;
	default:
		break;
	}

	return XCAST_OK;
}
int32_t XCastHelper::onXCastDeviceEvent(void *contextinfo, tencent::xcast_data &data)
{
#if kForVipKidTest
	new_device_event(NULL, data);
#endif
	XCastHelper *instance = (XCastHelper *)contextinfo;

	if (instance && instance->m_global_handler.get())
	{
		instance->m_global_handler->onDeviceEvent(contextinfo, data);
	}
	return XCAST_OK;
}
int32_t XCastHelper::onXCastTipsEvent(void *contextinfo, tencent::xcast_data &data)
{
#if kForVipKidTest
	new_stat_tips(NULL, data);
#endif
	return XCAST_OK;
}

XCastHelper* XCastHelper::getInstance()
{
	static std::once_flag oc;//用于call_once的局部静态变量
	std::call_once(oc, [&] { 
		m_instance = new XCastHelper();
	});
	return m_instance;
}
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

	if (is_startup_succ)
	{
		///* 注册事件通知回调 */
		tencent::xcast::handle_event(XC_EVENT_SYSTEM, XCastHelper::onXCastSystemEvent, m_instance);
		tencent::xcast::handle_event(XC_EVENT_STREAM, XCastHelper::onXCastStreamEvent, m_instance);
		tencent::xcast::handle_event(XC_EVENT_TRACK, XCastHelper::onXCastTrackEvent, m_instance);
		tencent::xcast::handle_event(XC_EVENT_DEVICE, XCastHelper::onXCastDeviceEvent, m_instance);
		tencent::xcast::handle_event(XC_EVENT_STATISTIC_TIPS, XCastHelper::onXCastTipsEvent, m_instance);
	}
	

	callback(avsdkErrorCode(rt), is_startup_succ ? "xcast_startup succ" : "xcast_startup failed");

}
void XCastHelper::stopContext(std::function<void(int32_t, char *)> callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (stream_state == Room_Connectted)
	{
		std::string sid = m_stream_param->streamID;
		int32_t ret = tencent::xcast::close_stream(sid.c_str());

		if (ret == XCAST_OK)
		{
			clearAfterExitRoom();
		}
		else
		{
			callback(avsdkErrorCode(ret), "xcast cant stop streaming");
			return;
		}
	}

	tencent::xcast::shutdown();
	is_startup_succ = false;
	callback(avsdkErrorCode(XCAST_OK), "xcast shutdown succ");
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
	if (stream_state != Room_Closed)
	{
		callback(1003, "xcast is streaming");
		return;
	}
	if (!(param.get())||!(param->isVaild()))
	{
		callback(1004, "xcast start stream param error");
		return;
	}

	m_stream_param.reset();
	m_stream_param = std::move(param);

	m_room_handler.reset();
	if (roomDelegate.get())
	{
		m_room_handler = std::move(roomDelegate);
	}
	


	// 参数配置
	tencent::xcast_data       params, auth_info, track;

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
		auth_info.put("auth_buffer", m_stream_param->auth_info.auth_buffer, m_stream_param->auth_info.auth_buffer_size);
	}
	else if (m_stream_param->auth_info.auth_type == xc_auth_manual)
	{
		auth_info["account_type"] = m_startup_param->accounttype;
		auth_info["expire_time"] = m_stream_param->auth_info.expire_time;
		const  char *secret_key = m_stream_param->auth_info.secret_key.c_str(); // 互动直播控制台上创建应该时生成的	
		auth_info.put("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
	}

	params.put("auth_info", auth_info);


	 //自定义采集（optional）,视频业务逻辑情况定
	track["ext-video-capture"] = m_stream_param->track.ext_video_capture;    /* allow video track to use external capture */
	track["ext-audio-capture"] = m_stream_param->track.ext_audio_capture;    /* allow audio track to use external capture */
	track["ext-audio-playback"] = m_stream_param->track.ext_audio_playback;   /* allow audio track to use external playback */
	params.put("track", track);

	////\"videomaxbps\":int32(3000)
	//params["videomaxbps"] = 3000;
	/*char *pstr = params.dump();*/

	int32_t  rt = tencent::xcast::start_stream(m_stream_param->streamID.c_str(), params);
	if (XCAST_OK != rt) {
		// TODO：进房失败；
		// ui_xcast_err(rt, xcast_err_msg(), user_data);

		callback(avsdkErrorCode(rt), (char *)xcast_err_msg());
		clearAfterExitRoom();
		return;
	}
	callback(avsdkErrorCode(rt), "xcast_start_stream succ");
	stream_state = Room_Connecting;

}
void XCastHelper::exitRoom(std::function<void(int32_t, char *)> callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (stream_state != Room_Closed)
	{
		callback(1004, "xcast isn't streaming");
		return;
	}

	int32_t ret = tencent::xcast::close_stream(m_stream_param->streamID.c_str());

	if (ret != XCAST_OK)
	{
		callback(avsdkErrorCode(ret), (char *)xcast_err_msg());
	}
	else
	{
		callback(avsdkErrorCode(ret), "xcast close stream succ");
	}
}

void XCastHelper::clearAfterExitRoom()
{
	stream_state = Room_Closed;
	m_room_handler.reset();
	m_stream_param.reset();
}
// Speaker操作
int XCastHelper::enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback)
{
	xcast_data defaultSpeaker = tencent::xcast::get_property(XC_SPEAKER_DEFAULT);
	return enableSpeaker(defaultSpeaker["return"], enable, callback);
}

// 指定默认的speaker 为 sid
// 同时进行enable操作
int XCastHelper::enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback)
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
	xcast_data defaultMic = tencent::xcast::get_property(XC_MIC_DEFAULT);
	return enableMic(defaultMic["return"], enable, callback);
}

// 对默认mic操作
int XCastHelper::enableMic(const char*micid, bool enable, std::function<void(int32_t, char *)> callback)
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
	xcast_data defaultCamera = tencent::xcast::get_property(XC_CAMERA_STATE);
	return XCAST_OK;
}

// 在房间内时：打开摄像头，并预览，同时上行；
// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；

int XCastHelper::enableCamera(bool preview, bool campture, std::function<void(int32_t, char *)> callback)
{
	xcast_data defaultCamera = tencent::xcast::get_property(XC_CAMERA_DEFAULT);
	enableCamera((const char *)(defaultCamera["return"]), preview, campture, callback);
	return XCAST_OK;
}
int XCastHelper::enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback)
{
	enableCamera(cameraid, preview, true, callback);
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