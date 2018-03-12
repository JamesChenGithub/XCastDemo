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
	// TODO:�ȼ�д
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
	// TODO��������� 

	tencent::xcast_data setparam;
	setparam["app_id"] = m_startup_param->sdkappid;
	setparam["identifier"] = m_startup_param->identifier;
	setparam["test_env"] =  m_startup_param->isTestEvn;
	int32_t rt = tencent::xcast::startup(setparam);

	is_startup_succ = (rt == XCAST_OK);
	///* ע���¼�֪ͨ�ص� */
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
	


	// ��������
	xcast_data_t       params, auth_info, track;

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
		auth_info.put_bytes("auth_buffer", m_stream_param->auth_info.auth_buffer, m_stream_param->auth_info.auth_buffer_size);  // "xxxx" Ϊҵ������auth_buffer
	}
	else if (m_stream_param->auth_info.auth_type == xc_auth_manual)
	{
		auth_info["account_type"] = m_startup_param->accounttype;
		auth_info["expire_time"] = m_stream_param->auth_info.expire_time;
		const  char *secret_key = m_stream_param->auth_info.secret_key.c_str(); // ����ֱ������̨�ϴ���Ӧ��ʱ���ɵ�	
		auth_info.put_bytes("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
	}

	params.put("auth_info", auth_info);
	// �Զ���ɼ���optional��,��Ƶҵ���߼������
	track["ext-video-capture"] = m_stream_param->track.ext_video_capture;    /* allow video track to use external capture */
	track["ext-audio-capture"] = m_stream_param->track.ext_audio_capture;    /* allow audio track to use external capture */
	track["ext-audio-playback"] = m_stream_param->track.ext_audio_playback;   /* allow audio track to use external playback */
	params["track"] = track;

	int32_t  rt = xcast_start_stream(m_stream_param->streamID.c_str(), params);
	if (XCAST_OK != rt) {
		// TODO������ʧ�ܣ�
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


// Speaker����
int XCastHelper::enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback)
{

	/*xcast_variant_t defaultSpeaker = xcast_get_property(XC_SPEAKER_DEFAULT);
	xcast_data_t ds(defaultSpeaker);

	return enableSpeaker(ds["return"], enable, callback);*/

	return XCAST_OK;
}

// ָ��Ĭ�ϵ�speaker Ϊ sid
// ͬʱ����enable����
int enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback)
{
	return XCAST_OK;
}
// �л�������� , speaker : 1 ������ 0 ����
int XCastHelper::changeOutputMode(bool speaker)
{
	return XCAST_OK;
}

// ��ȡ��ǰSpeaker״̬��
int XCastHelper::getSpeakerState()
{
	return XCAST_OK;
}

// mic����
// ��Ĭ��mic����
int XCastHelper::enableMic(bool enable, std::function<void(int32_t, char *)> callback)
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

// �ڷ�����ʱ����ȡ�ɼ�Camera״̬��
// �ڷ�����ʱ����ȡdefault Camera״̬;
int XCastHelper::getCameraState()
{
	return XCAST_OK;
}

// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
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
	// TODO : xcast err code ת��AVSDK������
	return xcast_err_code;
}