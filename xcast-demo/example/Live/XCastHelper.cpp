

#include "XCastHelper.h"
#include <locale.h>
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
		/*if (instance && instance->m_room_handler)
		{
			instance->m_room_handler->onWillEnterRoom(0, NULL);
		}*/
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

				// 打开摄像头
				instance->enableCamera(opera.autoCameraPreview, opera.autoCameraCapture, opera.defaultCamera.c_str());
				
				// 打开扬声器
				std::vector<std::string> slist;
				instance->getSpeakerList(slist);
				if (slist.size() > 0)
				{
					instance->speakerVolume(slist[0].c_str());
					instance->enableSpeaker(slist[0].c_str(), opera.autoSpeaker);
				}

				// 打开Mic
				instance->enableMic(opera.autoMic);
				
				
			
				
				
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
	XCastHelper *instance = (XCastHelper *)contextinfo;
	if (instance && instance->m_room_handler.get())
	{
		switch ((int32_t)data["type"])
		{
		case xc_track_added:
		{
			/* 新增轨道 */
			/*ui_track_add(e, true, user_data);*/
			XCastEndPoint info;
			instance->m_room_handler->onEndpointsUpdateInfo(info);
		}
		break;
		case xc_track_updated:
		{
			XCastEndPoint info;
			instance->m_room_handler->onEndpointsUpdateInfo(info);
		}
		break;
		case xc_track_capture_changed:
		{
			XCastEndPoint info;
			instance->m_room_handler->onEndpointsUpdateInfo(info);
		}
		/* 更新轨道 */
		/*ui_track_update(e, user_data);*/
		break;
		case xc_track_removed:
		{
			XCastEndPoint info;
			instance->m_room_handler->onEndpointsUpdateInfo(info);
		}
		//ui_track_add(e, false, user_data);
		break;
		case xc_track_media: {
			//ui_track_media(e, user_data);
			XCastVideoFrame *videoFrame;
			instance->m_room_handler->onVideoPreview(videoFrame);
		}
							 break;
		default:
			break;
		}
	}
	

	return XCAST_OK;
}
int32_t XCastHelper::onXCastDeviceEvent(void *contextinfo, tencent::xcast_data &e)
{
#if kForVipKidTest
	new_device_event(NULL, e);
#endif
	XCastHelper *instance = (XCastHelper *)contextinfo;
	if (instance && instance->m_global_handler.get())
	{
		switch ((int32_t)e["type"]) {
		case xc_device_added:
			/* 设备插入 */
		{
			XCastDevice device;
			instance->m_global_handler->onDeviceEvent_DeviceAdd(device);
		}

			/*if (e["class"] == xc_device_camera && e["src"] == "11Webcam C170")
			{
			int ret = xcast::set_property(XC_CAMERA_DEFAULT, e["src"]);
			int i = 0;
			}*/

			break;
		case xc_device_updated:
		{
			XCastDevice device;
			instance->m_global_handler->onDeviceEvent_DeviceUpdate(device);
		}
//			/* 设备更新 */
//			ui_device_update(e["src"], e["class"], e["state"], e["err"], e["err_msg"], user_data);
//
//#if defined(XCAST_EXTERNAL_VIDEO)
//			dev = (const char *)evt["src"];
//			state = evt["state"];
//			if (dev == "ext1") {
//				if (state == xc_device_running) {
//					/* TODO: start external capture here */
//					xcast_inject_video((const uint8_t *)0xFFFFFFFF, 4096, 480, 320);
//				}
//				else if (state == xc_device_stopped) {
//					/* TODO: stop external capture here */
//
//				}
//			}
//#endif
//
			break;
		case xc_device_removed:
			/* 设备拔出 */
			/*ui_device_added(e["src"], e["class"], false, user_data);*/
		{
			XCastDevice device;
			instance->m_global_handler->onDeviceEvent_DeviceRemoved(device);
		}
			break;
		case xc_device_preprocess:
			/* 设备预处理 */
		// 	ui_device_preprocess(e, user_data);
			// TODO:
			break;
		case xc_device_preview:
			// TODO:
			/* 设备预览 */
			if (instance->m_global_handler->needGlobalCallbackLocalVideo())
			{
				XCastVideoFrame *frame;
				instance->m_global_handler->onLocalVideoPreview(frame);
			}

			if (instance->m_room_handler->needRoomCallbackLocalVideo())
			{
				XCastVideoFrame *frame;
				instance->m_room_handler->onLocalVideoPreview(frame);
			}
			break;
		default:
			break;
		}

		return XCAST_OK;
	}
	return XCAST_OK;
}
int32_t XCastHelper::onXCastTipsEvent(void *contextinfo, tencent::xcast_data &data)
{
#if kForVipKidTest
	new_stat_tips(NULL, data);
#endif

	XCastHelper *instance = (XCastHelper *)contextinfo;
	if (instance->m_room_handler->needRoomCallbackTips())
	{
		instance->m_room_handler->onStatTips();
	}
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
void XCastHelper::startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback)
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
void XCastHelper::stopContext(XCHCallBack callback)
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


void XCastHelper::enterRoom(std::unique_ptr<XCastStreamParam> param, std::unique_ptr<XCastRoomHandler>	roomDelegate, XCHCallBack callback)
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
void XCastHelper::exitRoom(XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (stream_state != Room_Connectted)
	{
		callback(1004, "xcast isn't streaming");
		return;
	}


	// 添加退房时，关摄像头的操作

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

void XCastHelper::getSpeakerList(std::vector<std::string> &vec) const
{
	 getDeviceList(Device_Speaker, vec);
}
// Speaker操作
int XCastHelper::enableSpeaker(bool enable, XCHCallBack callback)
{
	return enableSpeaker(nullptr, enable, callback);
}

// 指定默认的speaker 为 sid
// 同时进行enable操作
int XCastHelper::enableSpeaker(const char *sid, bool enable, XCHCallBack callback)
{
	std::string oidstr;
	getOperaSpeaker(oidstr, sid);
	if (oidstr.length() == 0)
	{
		callback(avsdkErrorCode(-104), "not camera param");
		return avsdkErrorCode(-104);
	}
	const char *operdevID = oidstr.c_str();
	// 房间外
	tencent::xcast_data data;
	data["params"] = enable;
	int32_t ret = tencent::xcast::set_property(XC_SPEAKER_PREVIEW, operdevID, data);
	callback(avsdkErrorCode(ret), ret != XCAST_OK ? tencent::xcast::err_msg() : "enable speaker succ");
	return avsdkErrorCode(ret);
}
// 切换输出类型 , speaker : 1 扬声器 0 耳机
int XCastHelper::changeOutputMode(const char *sid, bool headphone)
{
	std::string oidstr;
	getOperaSpeaker(oidstr, sid);
	
	if (oidstr.length() == 0)
	{
		return avsdkErrorCode(-104);
	}
	const char *operdevID = oidstr.c_str();
	int32_t ret = tencent::xcast::set_property(XC_SPEAKER_EARPHONE_MODE, operdevID, tencent::xcast_data(headphone));
	return avsdkErrorCode(ret);
}

int XCastHelper::speakerVolume(const char *sid) const
{
	std::string oidstr;
	getOperaSpeaker(oidstr,sid);
	
	if (oidstr.length() == 0)
	{
		return avsdkErrorCode(-104);
	}
	const char *operdevID = oidstr.c_str();
	tencent::xcast_data data = tencent::xcast::get_property(XC_SPEAKER_VOLUME, tencent::xcast_data(operdevID));
	uint32_t volume = data.uint32_val();
	return volume;
}
int XCastHelper::setSpeakerVolume(int volume, const char *sid)
{
	std::string oidstr;
	getOperaSpeaker(oidstr, sid);
	if (oidstr.length() == 0)
	{
		return avsdkErrorCode(-104);
	}
	const char *operdevID = oidstr.c_str();
	uint32_t setvol = volume;
	if (volume < 0)
	{
		setvol = 0;
	}
	else if (volume > 100)
	{
		setvol = 100;
	}

	tencent::xcast_data vol(setvol);
	int32_t ret = tencent::xcast::set_property(XC_SPEAKER_VOLUME, sid, setvol);
	return ret;
}

// mic操作
// 对默认mic操作

// 获取麦克风列表, 结果是同步查询，外部不要保存结果
// 返回：麦克风列表（UTF-8格式串，外部进行转码）
void XCastHelper::getMicList(std::vector<std::string> &vec) const
{
	getDeviceList(Device_Mic, vec);
}


// 在房间内时，获取采集mic状态；
// 在房间外时，获取default mic状态;
int XCastHelper::getMicState(const char *micid) const
{
	std::string  operidstr;
	getOperaMic(operidstr,micid);
	const char *operid = operidstr.c_str();
	if (operid == nullptr)
	{
		return 0;
	}

	tencent::xcast_data state = tencent::xcast::get_property(XC_MIC_STATE, tencent::xcast_data(operid));
	int16_t statea = state.int16_val();
	return statea;
}


int XCastHelper::enableMic(bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, false, false, enableAudioOut,true, false, callback);
}

int XCastHelper::enableMicPreview( bool preview, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, preview, true, false, false, false, callback);
}

int XCastHelper::enableMic( bool preview, bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, preview, true, enableAudioOut, true, false, callback);
}

// 摄像头操作

void XCastHelper::getCameraList(std::vector<std::string> &vec) const
{
	return getDeviceList(Device_Camera, vec);
}


// 在房间内时，获取采集Camera状态；
// 在房间外时，获取default Camera状态;
int XCastHelper::getCameraState(const char *cameraid) const
{
	std::string  operidstr;
	getOperaCamera(operidstr,cameraid);
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

int XCastHelper::enableCamera(bool campture, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, false, false, campture, true, false, callback);
}
int XCastHelper::enableCameraPreview(bool preview, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, preview, true, false, false, false, callback);
}

int XCastHelper::enableCamera(bool preview, bool enableVideoOut, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, preview, true, enableVideoOut, true, false, callback);
}

void XCastHelper::getOperaDevice(DeviceType type, std::string &retstr, const char *cameraid ) const
{
	char operdevID[XCAST_MAX_PATH];
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
			retstr = "";
			return;
		}
		tencent::xcast_data data = tencent::xcast::get_property(devicetyep);
		const char *dstr = data.dump();
		const char *str_val = data.str_val();
		char *str  = strdup(str_val);
		if (str_val == nullptr)
		{
			retstr = "";
			return;
		}
		else
		{
			strcpy(operdevID, str_val);
			retstr = std::string(operdevID);
		}
	}
	else
	{
		retstr = std::string(cameraid);
	}
}

void XCastHelper::getOperaCamera(std::string &retstr, const char *cameraid) const
{
	return getOperaDevice(Device_Camera, retstr, cameraid);
}

void XCastHelper::getOperaSpeaker(std::string &retstr, const char *cameraid) const
{
	return getOperaDevice(Device_Speaker, retstr, cameraid);
}

void XCastHelper::getOperaMic(std::string &retstr, const char *cameraid) const
{
	return getOperaDevice(Device_Mic, retstr, cameraid);
}


int XCastHelper::avsdkErrorCode(int xcast_err_code) const
{
	// TODO : xcast err code 转成AVSDK错误码
	return xcast_err_code;
}

void XCastHelper::getDeviceList(DeviceType type, std::vector<std::string> &devlist) const
{
	devlist.clear();
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
			return;
		}

		tencent::xcast_data captures = tencent::xcast::get_property(devicetyep);
		const char *datat = captures.dump();
		const char     *cap;
		for (uint32_t n = 0; n < captures.size(); n++) {
			cap = captures[n].str_val();
			devlist.push_back(std::string(cap));
		}
		std::for_each(devlist.begin(), devlist.end(), [](auto item) {
			int i = 0;
		});
	}
}


int XCastHelper::getDeviceState(DeviceType type, const char *device) const
{
	if (type == Device_Speaker)
	{
		return 0;
	}

	std::string  operidstr;
	getOperaDevice(type, operidstr, device);
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


int XCastHelper::operaMic(const char *micid, bool preview, bool needExePreview, bool audioout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback)
{

	if (!needExePreview && !needExeAudioOut && !needSetDefault)
	{
		int ret = avsdkErrorCode(XCAST_OK);
		callback(ret, "do nothing");
		return ret;
	}

	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string str;
	getOperaMic(str, micid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		callback(erc, "not found opera mic device");
		return erc;
	}
	const char *operdevID = str.c_str();
	if (needExePreview)
	{
		int32_t preret = tencent::xcast::set_property(XC_MIC_PREVIEW, operdevID, tencent::xcast_data(preview));

		if (needExeAudioOut || needSetDefault)
		{
			if (preret != XCAST_OK)
			{
				int erc = avsdkErrorCode(-preret);
				callback(erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			callback(erc, tencent::xcast::err_msg());
			return erc;
		}
		
	}

	if (needExeAudioOut)
	{
		const char *streamid = m_stream_param->streamID.c_str();
		{	// 房间内
			
			int32_t ret = tencent::xcast::set_property(XC_TRACK_CAPTURE, streamid, "audio-out", operdevID);
			if (ret != XCAST_OK)
			{
				int erc = avsdkErrorCode(ret);
				callback(erc, tencent::xcast::err_msg());
				return erc;
			}
		}

		if (stream_state == Room_Connectted)
		{
			tencent::xcast_data data,  params;
			params["enable"] = audioout;
			//data["params"] = params;
			int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, streamid, "audio-out", params);

			if (needSetDefault)
			{
				if (enret != XCAST_OK)
				{
					int erc = avsdkErrorCode(enret);
					callback(erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				callback(erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
				return erc;
			}
		}
	}

	if (needSetDefault)
	{
		// 房间外
		int32_t ret = tencent::xcast::set_property(XC_MIC_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		callback(erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	callback(erc, "opera mic succ");
	return erc;
	
}

int XCastHelper::operaCamera(const char *cameraid, bool preview, bool needExePreview, bool videoout, bool needExeVideoOut, bool needSetDefault, XCHCallBack callback)
{
	if (!needExePreview && !needExeVideoOut && !needSetDefault)
	{
		int erc = avsdkErrorCode(-104);
		callback(erc, "not found opera camera device");
		return erc;
	}

	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string str;
	getOperaCamera(str, cameraid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		callback(erc, "not found opera camera device");
		return erc;
	}
	const char *operdevID = str.c_str();
	if (needExePreview)
	{
		int32_t preret = tencent::xcast::set_property(XC_CAMERA_PREVIEW, operdevID, tencent::xcast_data(preview));

		if (needExeVideoOut || needSetDefault)
		{
			if (preret != XCAST_OK)
			{
				int erc = avsdkErrorCode(-preret);
				callback(erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			callback(erc, tencent::xcast::err_msg());
			return erc;
		}
	}
	
	if (needExeVideoOut)
	{
		// 房间内
		const char *streamid = m_stream_param->streamID.c_str();
		{
			int32_t ret = tencent::xcast::set_property(XC_TRACK_CAPTURE, streamid, "video-out", operdevID);
			if (ret != XCAST_OK)
			{
				int erc = avsdkErrorCode(ret);
				callback(erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		

		if (stream_state == Room_Connectted)
		{
			tencent::xcast_data params;
			params["enable"] = videoout;
			int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, streamid, "video-out", params);

			if (needSetDefault)
			{
				if (enret != XCAST_OK)
				{
					int erc = avsdkErrorCode(enret);
					callback(erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				callback(erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
				return erc;
			}

		}
	}

	if (needSetDefault)
	{
		// 房间外
		int32_t ret = tencent::xcast::set_property(XC_CAMERA_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		callback(erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	callback(erc, "opera mic succ");
	return erc;
}