#include "XCastHelper.h"
#include "include/xcast_variant.h"
#include "xcast.hh"


#if kForVipKidTest
#include "example/xcast-ui-handler.h"
#include <algorithm>
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
	//return XCAST_OK;
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
				//instance->enableMic(opera.defaultMic.c_str(), opera.autoMic);
				//instance->enableSpeaker(opera.defaultSpeaker.c_str(), opera.autoSpeaker);
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

	if (stream_state != Room_Connectted)
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

std::vector<std::string> XCastHelper::getSpeakerList() const
{
	return getDeviceList(Device_Speaker);
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

// mic操作
// 对默认mic操作

// 获取麦克风列表, 结果是同步查询，外部不要保存结果
// 返回：麦克风列表（UTF-8格式串，外部进行转码）
std::vector<std::string> XCastHelper::getMicList() const
{
	return getDeviceList(Device_Mic);
}

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

std::vector<std::string> XCastHelper::getCameraList() const
{
	return getDeviceList(Device_Camera);
}


// 在房间内时，获取采集Camera状态；
// 在房间外时，获取default Camera状态;
int XCastHelper::getCameraState(const char *cameraid) const
{
	std::string  operidstr = getOperaCamera(cameraid);
	const char *operid = operidstr.c_str();
	if (operid == nullptr)
	{
		return 0;
	}

	tencent::xcast_data state = tencent::xcast::get_property(XC_CAMERA_STATE, tencent::xcast_data(operid));
	int16_t statea = state.int16_val();
	return statea;
}

// 在房间内时：打开摄像头，并预览，同时上行；
// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；

int XCastHelper::enableCamera(bool preview, bool campture, std::function<void(int32_t, char *)> callback)
{
	enableCamera(nullptr, preview, campture, callback);
	return XCAST_OK;
}
int XCastHelper::enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback)
{
	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string oidstr = getOperaCamera(cameraid);
	const char *operdevID = oidstr.c_str();
	if (operdevID == nullptr || strlen(operdevID))
	{
		callback(avsdkErrorCode(-104), "not camera param");
		return avsdkErrorCode(-104);
	}
	int32_t preret = tencent::xcast::set_property(XC_CAMERA_PREVIEW, operdevID, tencent::xcast_data(preview));
	callback(avsdkErrorCode(preret), preret != XCAST_OK ? "set camera preview error" : "set camera preview succ");
	return avsdkErrorCode(preret);
}

int XCastHelper::enableCamera(const char *cameraid, bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback)
{
	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string str = getOperaCamera(cameraid);
	const char *operdevID = str.c_str();
	if (operdevID == nullptr || strlen(operdevID) == 0)
	{
		callback(avsdkErrorCode(-104), "not camera param");
		return avsdkErrorCode(-104);
	}
	
	int32_t preret = tencent::xcast::set_property(XC_CAMERA_PREVIEW, operdevID, tencent::xcast_data(preview));

	if (preret != XCAST_OK)
	{
		callback(avsdkErrorCode(preret), "set camera preview error");
		return avsdkErrorCode(preret);
	}

	if (stream_state == Room_Connectted)
	{
		// 房间内
		const char *streamid = m_stream_param->streamID.c_str();
		int32_t ret = tencent::xcast::set_property(XC_TRACK_CAPTURE, streamid,"video-out", operdevID);
		if (ret != XCAST_OK)
		{
			callback(avsdkErrorCode(ret), "set capture camera error");
			return avsdkErrorCode(ret);
		}
		tencent::xcast_data params;
		params["enable"] = enableVideoOut;
		int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, streamid, "video-out", params);
		
		callback(avsdkErrorCode(ret), ret != XCAST_OK ? "set enable video-out error" : "enable camera succ");
		return avsdkErrorCode(ret);
		
	}
	else
	{
		// 房间外
		int32_t ret = tencent::xcast::set_property(XC_CAMERA_DEFAULT, operdevID);
		callback(avsdkErrorCode(ret), ret != XCAST_OK ? "not in room ,set default camera failed" : "enable camera succ");
		return avsdkErrorCode(ret);
	}
	
}

//int XCastHelper::updateCameraMode(const char *cameraid, bool autoSending, std::function<void(int32_t, char *)> callback)
//{
//	if (stream_state != Room_Connectted)
//	{
//		callback(-1, "not in room");
//		return -1;
//	}
//
//	const char *streamid = m_stream_param->streamID.c_str();
//	if (cameraid == nullptr || strlen(cameraid) == 0)
//	{
//		// 直接修改当前操作的摄像头
//		tencent::xcast_data params;
//		params["enable"] = autoSending;
//		int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, streamid, "video-out", params);
//		if (enret != XCAST_OK)
//		{
//			callback(avsdkErrorCode(enret), "set enable video-out error");
//			return avsdkErrorCode(enret);
//		}
//	}
//	else
//	{
//		// 先获取当前摄像头，如果相同直接操作，如果不同则进行切换
//
//
//
//	}
//
//	
//
//
//	return XCAST_OK;
//}

//int XCastHelper::switchCamera(const char *cameraid, bool preview, bool campture, std::function<void(int32_t, char *)> callback)
//{
//	return XCAST_OK;
//}


std::string XCastHelper::getOperaDevice(DeviceType type, const char *cameraid = nullptr) const
{
	char operdevID[XCAST_MAX_PATH];
	bool isoperaDefaultCamera = false;
	if (cameraid == nullptr || strlen(cameraid) == 0)
	{
		const char *devicetyep = nullptr;

		switch (type)
		{
		case Device_Camera:
			devicetyep = XC_CAMERA_DEFAULT;
			break;
		case Device_Mic:
			devicetyep = XC_MIC_DEFAULT;
			break;
		case Device_Speaker:
			devicetyep = XC_SPEAKER_DEFAULT;
			break;
		}

		if (devicetyep == nullptr)
		{
			return "";
		}
		tencent::xcast_data data = tencent::xcast::get_property(devicetyep);
		const char *str_val = data.str_val();
		if (str_val == nullptr)
		{

			return "";
		}
		else
		{
			isoperaDefaultCamera = true;
			strcpy(operdevID, str_val);
			return std::string(operdevID);
		}
	}
	else
	{
		return std::string(cameraid);
	}
}

std::string XCastHelper::getOperaCamera(const char *cameraid) const
{
	return getOperaDevice(Device_Camera, cameraid);
}

std::string XCastHelper::getOperaSpeaker(const char *cameraid) const
{
	return getOperaDevice(Device_Speaker, cameraid);
}

std::string XCastHelper::getOperaMic(const char *cameraid) const
{
	return getOperaDevice(Device_Mic, cameraid);
}


int XCastHelper::avsdkErrorCode(int xcast_err_code)
{
	// TODO : xcast err code 转成AVSDK错误码
	return xcast_err_code;
}

std::vector<std::string> XCastHelper::getDeviceList(DeviceType type) const
{
	std::vector<std::string> camlist;
	if (is_startup_succ)
	{

		const char *devicetyep = nullptr;

		switch (type)
		{
		case Device_Camera:
			devicetyep = XC_CAMERA;
			break;
		case Device_Mic:
			devicetyep = XC_MIC;
			break;
		case Device_Speaker:
			devicetyep = XC_SPEAKER;
			break;
		}

		if (devicetyep == nullptr)
		{
			return camlist;
		}

		tencent::xcast_data captures = tencent::xcast::get_property(devicetyep);

		const char     *cap;
		for (uint32_t n = 0; n < captures.size(); n++) {
			cap = captures[n].str_val();
			camlist.push_back(std::string(cap));
		}
		std::for_each(camlist.begin(), camlist.end(), [](auto item) {
			int i = 0;
		});

		return camlist;
	}
	else
	{
		return camlist;
	}
}


int XCastHelper::getDeviceState(DeviceType type, const char *device = nullptr) const
{
	if (type == Device_Speaker)
	{
		return 0;
	}

	std::string  operidstr = getOperaDevice(type, device);
	const char *operid = operidstr.c_str();
	if (operid == nullptr)
	{
		return 0;
	}

	const char *devicetyep = nullptr;

	switch (type)
	{
	case Device_Camera:
		devicetyep = XC_CAMERA_STATE;
		break;
	case Device_Mic:
		devicetyep = XC_MIC_STATE;
		break;
	case Device_Speaker:
		devicetyep = nullptr;
		break;
	}

	tencent::xcast_data state = tencent::xcast::get_property(devicetyep, tencent::xcast_data(operid));
	int16_t statea = state.int16_val();
	return statea;
}