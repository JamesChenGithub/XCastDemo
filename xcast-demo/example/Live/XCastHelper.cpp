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

	// TODO: ��ӻص� 
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
			/* ��״̬�� ���ӳɹ� */
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
		/* ��״̬�� �ر�, �Ƴ�ý���� */
		//ui_stream_closed(e["src"], e["err"], e["err-msg"], user_data);

		if (instance)
		{
			if (instance->stream_state == Room_Connectted)
			{
				// �˷�
				if (instance->m_room_handler)
				{
					instance->m_room_handler->onExitRoomComplete((int)(data["err"]), (const char *)(data["err-msg"]));
				}
				instance->clearAfterExitRoom();
			}
			else if (instance->stream_state == Room_Connecting)
			{
				// ������������
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
		/* ������� */
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
	/* ���¹�� */
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
	static std::once_flag oc;//����call_once�ľֲ���̬����
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
	// TODO��������� 

	tencent::xcast_data setparam;
	setparam["app_id"] = m_startup_param->sdkappid;
	setparam["identifier"] = m_startup_param->identifier;
	setparam["test_env"] =  m_startup_param->isTestEvn;
	int32_t rt = tencent::xcast::startup(setparam);

	is_startup_succ = (rt == XCAST_OK);

	if (is_startup_succ)
	{
		///* ע���¼�֪ͨ�ص� */
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
	


	// ��������
	tencent::xcast_data       params, auth_info, track;

	// ����ţ����շ�ʽ����ɫ
	params["relation_id"] = m_stream_param->roomid;
	params["auto_recv"] = m_stream_param->auto_recv;
	params["role"] = m_stream_param->role.c_str();

	params["auth_type"] = m_stream_param->auth_info.auth_type;
	auth_info["auth_bits"] = m_stream_param->auth_info.auth_bits;
	// ��Ȩ��ʽ�� ���ü�Ȩ��ʽ����������ѡ��һ���ɣ�
	if (m_stream_param->auth_info.auth_type == xc_auth_none)
	{
		// ��ʽ1. �޼�Ȩ����������ʽ
		// do nothing
	}
	else if (m_stream_param->auth_info.auth_type == xc_auth_manual)
	{
		// ��ʽ2. �ֶ���Ȩ
		auth_info.put("auth_buffer", m_stream_param->auth_info.auth_buffer, m_stream_param->auth_info.auth_buffer_size);
	}
	else if (m_stream_param->auth_info.auth_type == xc_auth_manual)
	{
		auth_info["account_type"] = m_startup_param->accounttype;
		auth_info["expire_time"] = m_stream_param->auth_info.expire_time;
		const  char *secret_key = m_stream_param->auth_info.secret_key.c_str(); // ����ֱ������̨�ϴ���Ӧ��ʱ���ɵ�	
		auth_info.put("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
	}

	params.put("auth_info", auth_info);


	 //�Զ���ɼ���optional��,��Ƶҵ���߼������
	track["ext-video-capture"] = m_stream_param->track.ext_video_capture;    /* allow video track to use external capture */
	track["ext-audio-capture"] = m_stream_param->track.ext_audio_capture;    /* allow audio track to use external capture */
	track["ext-audio-playback"] = m_stream_param->track.ext_audio_playback;   /* allow audio track to use external playback */
	params.put("track", track);

	////\"videomaxbps\":int32(3000)
	//params["videomaxbps"] = 3000;
	/*char *pstr = params.dump();*/

	int32_t  rt = tencent::xcast::start_stream(m_stream_param->streamID.c_str(), params);
	if (XCAST_OK != rt) {
		// TODO������ʧ�ܣ�
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
// Speaker����
int XCastHelper::enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback)
{
	xcast_data defaultSpeaker = tencent::xcast::get_property(XC_SPEAKER_DEFAULT);
	return enableSpeaker(defaultSpeaker["return"], enable, callback);
}

// ָ��Ĭ�ϵ�speaker Ϊ sid
// ͬʱ����enable����
int XCastHelper::enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}
// �л�������� , speaker : 1 ������ 0 ����
int XCastHelper::changeOutputMode(bool speaker)
{
	return XCAST_OK;
}

// mic����
// ��Ĭ��mic����

// ��ȡ��˷��б�, �����ͬ����ѯ���ⲿ��Ҫ������
// ���أ���˷��б�UTF-8��ʽ�����ⲿ����ת�룩
std::vector<std::string> XCastHelper::getMicList() const
{
	return getDeviceList(Device_Mic);
}

int XCastHelper::enableMic(bool enable, std::function<void(int32_t, char *)> callback)
{
	xcast_data defaultMic = tencent::xcast::get_property(XC_MIC_DEFAULT);
	return enableMic(defaultMic["return"], enable, callback);
}

// ��Ĭ��mic����
int XCastHelper::enableMic(const char*micid, bool enable, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}
// �л�mic
int XCastHelper::switchToMic(const char *micid, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}

// �ڷ�����ʱ����ȡ�ɼ�mic״̬��
// �ڷ�����ʱ����ȡdefault mic״̬;
int XCastHelper::getMicState()
{
	return XCAST_OK;
}

int XCastHelper::enableLoopBack(bool enable)
{
	return XCAST_OK;
}

// ����ͷ����

std::vector<std::string> XCastHelper::getCameraList() const
{
	return getDeviceList(Device_Camera);
}


// �ڷ�����ʱ����ȡ�ɼ�Camera״̬��
// �ڷ�����ʱ����ȡdefault Camera״̬;
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

// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��

int XCastHelper::enableCamera(bool preview, bool campture, std::function<void(int32_t, char *)> callback)
{
	enableCamera(nullptr, preview, campture, callback);
	return XCAST_OK;
}
int XCastHelper::enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback)
{
	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
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
	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
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
		// ������
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
		// ������
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
//		// ֱ���޸ĵ�ǰ����������ͷ
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
//		// �Ȼ�ȡ��ǰ����ͷ�������ֱͬ�Ӳ����������ͬ������л�
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
	// TODO : xcast err code ת��AVSDK������
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