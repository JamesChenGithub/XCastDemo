

#include "XCastHelper.h"
#include <locale.h>
#include "xcast.hh"


#if kForVipKidTest
#include "example/xcast-ui-handler.h"
#include <algorithm>
#endif


inline void XCastHelperCallBack(XCHCallBack callback, int32_t errcode, const char *err)
{
	if (callback)
		callback(errcode, err);
}

bool xcast_data_to_deviceitem(tencent::xcast_data data, XCastDeviceHotPlugItem &item)
{
	const char *srcstr = data["src"];

	if (srcstr == nullptr)
		return false;

	XCastDeviceHotPlugItem device;
	device.deviceClass = (XCastDeviceType)((int32_t)(data["class"]));
	device.src = std::string(srcstr);
	device.state = (XCastDeviceState)((int32_t)(data["state"]));
	device.err = data["err"];
	const char *str = data["err_msg"].str_val();
	device.err_msg = str ? std::string(str) : "";
	return true;
}

bool xcast_data_to_videoframe(tencent::xcast_data data, XCastVideoFrame &frame, bool isLocal)
{
	const char *dev = data["src"];
	const uint8_t *bytedata = data["data"].bytes_val();
	if (!dev || !bytedata)
		return false;

	frame.deviceSrc = dev;

	int32_t width = data["width"];
	int32_t height = data["height"];
	int32_t format = data["format"];
	int32_t	rotate = data["rotate"];
	int32_t	size = data["size"];
	int32_t	direction = isLocal ? 1 : data["direction"];

	if (frame.data == nullptr)
	{
		frame.data = (uint8_t *)malloc(size);
		if (frame.data == nullptr)
			return false;
	}
	else if (frame.size != size || frame.width != width || frame.height != height)
	{
		free(frame.data);
		frame.data = (uint8_t *)malloc(size);
		if (frame.data == nullptr)
			return false;
	}
	memcpy(frame.data, bytedata, size);
	frame.media_format = (XCastMediaFormat)format;
	frame.rotate = rotate;
	frame.direction = (xc_track_direction)direction;
	frame.width = width;
	frame.height = height;
	frame.size = size;
	return true;
}



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
		instance->m_global_handler->onSystemEvent();
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
			/* ��״̬�� ���ӳɹ� */
			if (instance && instance->m_room_handler)
			{
				instance->m_room_handler->onDidEnterRoom(0, NULL);
			}

			if (instance->m_stream_param.get())
			{
				const XCastRoomOption opera = instance->m_stream_param->roomOpera;

				// ������ͷ
				instance->enableCamera(opera.autoCameraPreview, opera.autoCameraCapture, opera.defaultCamera.c_str());
			
				// ��������
				instance->enableSpeaker(opera.autoSpeaker);
				// ��������
				//std::vector<std::string> slist;
				//instance->getSpeakerList(slist);
				//if (slist.size() > 0)
				//{
				//	instance->speakerVolume(slist[0].c_str());
				
				//}

				// ��Mic
				//instance->enableMic(opera.autoMic);
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
	XCastHelper *instance = (XCastHelper *)contextinfo;
	uint64_t          uin = data["uin"]; 
	if (instance && instance->m_room_handler.get())
	{
		xc_track_event type = (xc_track_event)((int32_t)data["type"]);

		switch (type)
		{
		case xc_track_added:
		{
			/* ������� */
			/*ui_track_add(e, true, user_data);*/
			//instance->logtoFile("xc_track_added", data.dump());

			/*XCastEndPoint info;
			instance->m_room_handler->onEndpointsUpdateInfo(info);*/
		}
		break;
		case xc_track_updated:
		case xc_track_capture_changed:
		{
			XCastMediaSource source = (XCastMediaSource)((int32_t)data["media_type"]);
			xc_track_type tracktype = (xc_track_type)((int32_t)data["class"]);
			xc_track_state state = (xc_track_state)((int32_t)data["state"]);
			bool has = (state == xc_track_running);

			XCastEndpointEvent event = XCast_Endpoint_NONE;
			if (tracktype == xc_track_audio)
			{
				event = has ? XCast_Endpoint_Has_Audio : XCast_Endpoint_No_Audio;
			}
			else if (tracktype == xc_track_video)
			{
				switch (source)
				{
				case XCastMediaSource_Camera:            /* camera */
					event = has ? XCast_Endpoint_Has_Camera_Video : XCast_Endpoint_No_Camera_Video;
					break;
				case XCastMediaSource_Screen_Capture:     /* screen capture */
					event = has ? XCast_Endpoint_Has_Screen_Video : XCast_Endpoint_No_Screen_Video;
					break;
				case XCastMediaSource_Media_Player:      /* media player */
					event = has ? XCast_Endpoint_Has_Media_Video : XCast_Endpoint_No_Media_Video;
				case XCastMediaSource_PPT:               /* ppt */
				default:
					break;
				}
			}

			if (event != XCast_Endpoint_NONE)
			{
				bool notify = false;
				std::shared_ptr<XCastEndpoint> end = instance->getEndpoint(uin);
				switch (event)
				{
				case XCast_Endpoint_Has_Camera_Video:
				case XCast_Endpoint_No_Camera_Video:
					if (end->is_camera_video != has)
					{
						end->is_camera_video = has;
						notify = true;
					}
					
					break;
				case XCast_Endpoint_Has_Audio:
				case XCast_Endpoint_No_Audio:
					if (end->is_audio != has)
					{
						end->is_audio = has;
						notify = true;
					}
					
					break;
				case XCast_Endpoint_Has_Screen_Video:
				case XCast_Endpoint_No_Screen_Video:
					if (end->is_screen_video != has)
					{
						end->is_screen_video = has;
						notify = true;
					}
					
					break;
				case XCast_Endpoint_Has_Media_Video:
				case XCast_Endpoint_No_Media_Video:
					if (end->is_media_video != has)
					{
						end->is_media_video = has;
						notify = true;
					}
					break;
				default:
					break;
				}

				// TODO: dump endpoint and callback
				if (notify)
				{
					XCastEndpoint ep;
					ep.tinyid = end->tinyid;
					ep.is_audio = end->is_audio;
					ep.is_camera_video = end->is_camera_video;
					ep.is_screen_video = end->is_screen_video;
					ep.is_media_video = end->is_media_video;
					instance->m_room_handler->onEndpointsUpdateInfo(event, ep);
					instance->updateEndpointMap(uin);
				}
				
			}
		}
		/* ���¹�� */
		/*ui_track_update(e, user_data);*/
		break;
		case xc_track_removed:
		{
			std::shared_ptr<XCastEndpoint> end = instance->getEndpoint(uin);
			XCastEndpoint ep;
			ep.tinyid = end->tinyid;
			ep.is_audio = end->is_audio;
			ep.is_camera_video = end->is_camera_video;
			ep.is_screen_video = end->is_screen_video;
			ep.is_media_video = end->is_media_video;
			instance->m_room_handler->onEndpointsUpdateInfo(XCast_Endpoint_Removed, ep);

			instance->deleteEndpoint(uin);

			instance->logtoFile("xc_track_removed", data.dump());
			/*	XCastEndPoint info;
			instance->m_room_handler->onEndpointsUpdateInfo(info);*/
		}
		//ui_track_add(e, false, user_data);
		break;
		case xc_track_media: 
		{
			instance->logtoFile("xc_track_media", data.dump());

			XCastMediaSource mediaTye = (XCastMediaSource)((int32_t)data["media-src"]);
			std::shared_ptr<XCastVideoFrame> frameptr = instance->getVideoFrameBuffer(uin, mediaTye);
			XCastVideoFrame *frame = frameptr.get();
			if (xcast_data_to_videoframe(data, *frame, false))
			{
				instance->m_room_handler->onVideoPreview(frameptr.get());
			}
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

	const char *srcstr = e["src"];

	if (srcstr == nullptr)
		return XCAST_OK;

	XCastHelper *instance = (XCastHelper *)contextinfo;
	if (instance && instance->m_global_handler.get())
	{
		xc_device_event eventtype = (xc_device_event)((int32_t)(e["type"]));
		switch (eventtype)
		{
		case xc_device_added:
		{
			/* �豸���� */
			XCastDeviceHotPlugItem device;
			if (xcast_data_to_deviceitem(e, device))
			{
				instance->m_global_handler->onDeviceEvent_DeviceAdd(device);
			}
			
		}
		break;
		case xc_device_updated:
		{
			XCastDeviceHotPlugItem device;
			if (xcast_data_to_deviceitem(e, device))
			{
				instance->m_global_handler->onDeviceEvent_DeviceUpdate(device);
			}
			
		}
		break;
		case xc_device_removed:
		{
			/* �豸�γ� */
			XCastDeviceHotPlugItem device;
			if (xcast_data_to_deviceitem(e, device))
			{
				instance->m_global_handler->onDeviceEvent_DeviceRemoved(device);
			}
		}
		break;
		case xc_device_preprocess:
			/* �豸Ԥ���� */
		// 	ui_device_preprocess(e, user_data);
			// TODO:
			break;
		case xc_device_preview:
		{
			/* �豸Ԥ�� */

			const char *cs = e.str_val();
			const char *str = e.dump();
			bool ng = instance->m_global_handler->needGlobalCallbackLocalVideo();
			bool nr = instance->m_room_handler && instance->m_room_handler->needRoomCallbackLocalVideo();
			if (ng || nr)
			{
				XCastDeviceType deviceType = (XCastDeviceType)((int32_t)(e["class"]));
				XCastMediaSource mediaTye = instance->getDeviceVideoSourceType(deviceType);

				std::shared_ptr<XCastVideoFrame> frameptr = instance->getVideoFrameBuffer(instance->m_startup_param->tinyid, mediaTye);
				XCastVideoFrame *frame = frameptr.get();
				if (xcast_data_to_videoframe(e, *frame, true))
				{
					if (ng)
					{
						instance->m_global_handler->onGlobalLocalVideoPreview(frameptr.get());
					}

					if (nr)
					{
						instance->m_room_handler->onLocalVideoPreview(frameptr.get());
					}
				}
			}
			
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
	instance->logtoFile("onXCastTipsEvent", data.dump());
	if (instance->m_room_handler->needRoomCallbackTips())
	{
		instance->m_room_handler->onStatTips();
	}
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

void XCastHelper::logtoFile(const char *tag, const char * info)
{
	fprintf(logFile, "%s : %s \n", tag, info);
	fflush(logFile);
}


int XCastHelper::startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	logFile = fopen("log.txt", "w+");
	
	if (is_startup_succ)
	{
		XCastHelperCallBack(callback, 1003, "xcast has started");
		return 1003;
	}

	if (!param.get() || !param->isVaild())
	{
		XCastHelperCallBack(callback,1004, "param error");
		return 1004;
	}
	m_startup_param.reset();
	m_startup_param = std::move(param);
	// TODO��������� 

	tencent::xcast_data setparam;
	setparam["app_id"] = m_startup_param->sdkappid;
	setparam["identifier"] = m_startup_param->tinyid;
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
	int code = avsdkErrorCode(rt);
	XCastHelperCallBack(callback, code, is_startup_succ ? "xcast_startup succ" : "xcast_startup failed");
	return code;
}
int XCastHelper::stopContext(XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	fflush(logFile);
	fclose(logFile);
	logFile = nullptr;
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
			int code = avsdkErrorCode(ret);
			XCastHelperCallBack(callback, code, "xcast cant stop streaming");
			return code;
		}
	}

	tencent::xcast::shutdown();
	is_startup_succ = false;
	m_global_handler.reset();
	int code = avsdkErrorCode(XCAST_OK);
	XCastHelperCallBack(callback, code, "xcast shutdown succ");
	return code;
}

void XCastHelper::setGlobalHandler(std::shared_ptr<XCastGlobalHandler>  handler)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	m_global_handler.reset();
	m_global_handler = std::move(handler);
}


int XCastHelper::enterRoom(std::unique_ptr<XCastStreamParam> param, std::shared_ptr<XCastRoomHandler>	roomDelegate, XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	if (stream_state != Room_Closed)
	{
		XCastHelperCallBack(callback,1003, "xcast is streaming");
		return 1003;
	}
	if (!(param.get())||!(param->isVaild()))
	{
		XCastHelperCallBack(callback,1004, "xcast start stream param error");
		return 1004;
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
	int code = avsdkErrorCode(rt);
	if (XCAST_OK != rt) {
		// TODO������ʧ�ܣ�
		// ui_xcast_err(rt, xcast_err_msg(), user_data);
		
		XCastHelperCallBack(callback, code, (char *)xcast_err_msg());
		clearAfterExitRoom();
		return code;
	}
	XCastHelperCallBack(callback, code, "xcast_start_stream succ");
	stream_state = Room_Connecting;
	return code;

}
int XCastHelper::exitRoom(XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (stream_state != Room_Connectted)
	{
		XCastHelperCallBack(callback,1004, "xcast isn't streaming");
		return 1004;
	}

	if (m_stream_param->roomOpera.autoCloseCameraOnExit)
	{
		enableCameraPreview(false);
	}
	
	// ����˷�ʱ��������ͷ�Ĳ���

	int32_t ret = tencent::xcast::close_stream(m_stream_param->streamID.c_str());

	int code = avsdkErrorCode(ret);
	if (ret != XCAST_OK)
	{
		XCastHelperCallBack(callback, code, (char *)xcast_err_msg());
	}
	else
	{
		XCastHelperCallBack(callback, code, "xcast close stream succ");
	}

	return code;
}

void XCastHelper::clearAfterExitRoom()
{
	stream_state = Room_Closed;
	m_room_handler.reset();
	m_stream_param.reset();

	std::for_each(video_frame_map.begin(), video_frame_map.end(), [&] (std::pair<std::string, std::shared_ptr<XCastVideoFrame>> pair){
		auto vfp = pair.second;
		vfp.reset();
	});
	
	video_frame_map.clear();

}

void XCastHelper::getSpeakerList(std::vector<std::string> &vec) const
{
	 getDeviceList(Device_Speaker, vec);
}
// Speaker����
int XCastHelper::enableSpeaker(bool enable, XCHCallBack callback)
{
	return enableSpeakerByID(enable, nullptr, callback);
}

// ָ��Ĭ�ϵ�speaker Ϊ sid
// ͬʱ����enable����
int XCastHelper::enableSpeakerByID(bool enable, const char *sid, XCHCallBack callback)
{
	std::string oidstr;
	getOperaSpeaker(oidstr, sid);
	if (oidstr.length() == 0)
	{
		XCastHelperCallBack(callback,avsdkErrorCode(-104), "not camera param");
		return avsdkErrorCode(-104);
	}
	const char *operdevID = oidstr.c_str();
	// ������
	tencent::xcast_data data;
	data["params"] = enable;
	int32_t ret = tencent::xcast::set_property(XC_SPEAKER_PREVIEW, operdevID, data);
	XCastHelperCallBack(callback,avsdkErrorCode(ret), ret != XCAST_OK ? tencent::xcast::err_msg() : "enable speaker succ");
	return avsdkErrorCode(ret);
}
// �л�������� , speaker : 1 ������ 0 ����
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

// mic����
// ��Ĭ��mic����

// ��ȡ��˷��б�, �����ͬ����ѯ���ⲿ��Ҫ������
// ���أ���˷��б�UTF-8��ʽ�����ⲿ����ת�룩
void XCastHelper::getMicList(std::vector<std::string> &vec) const
{
	getDeviceList(Device_Mic, vec);
}


// �ڷ�����ʱ����ȡ�ɼ�mic״̬��
// �ڷ�����ʱ����ȡdefault mic״̬;
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

// ����ͷ����

void XCastHelper::getCameraList(std::vector<std::string> &vec) const
{
	return getDeviceList(Device_Camera, vec);
}


// �ڷ�����ʱ����ȡ�ɼ�Camera״̬��
// �ڷ�����ʱ����ȡdefault Camera״̬;
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

// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��

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
	// TODO : xcast err code ת��AVSDK������
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
		XCastHelperCallBack(callback,ret, "do nothing");
		return ret;
	}

	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
	std::string str;
	getOperaMic(str, micid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback,erc, "not found opera mic device");
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
				XCastHelperCallBack(callback,erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			XCastHelperCallBack(callback,erc, tencent::xcast::err_msg());
			return erc;
		}
		
	}

	if (needExeAudioOut)
	{
		const char *streamid = m_stream_param->streamID.c_str();

		{	// ������
			
			int32_t ret = tencent::xcast::set_property(XC_TRACK_CAPTURE, streamid, "audio-out", operdevID);
			if (ret != XCAST_OK)
			{
				int erc = avsdkErrorCode(ret);
				XCastHelperCallBack(callback,erc, tencent::xcast::err_msg());
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
					XCastHelperCallBack(callback,erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				XCastHelperCallBack(callback,erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
				return erc;
			}
		}
	}

	if (needSetDefault)
	{
		// ������
		int32_t ret = tencent::xcast::set_property(XC_MIC_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		XCastHelperCallBack(callback,erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	XCastHelperCallBack(callback,erc, "opera mic succ");
	return erc;
	
}

int XCastHelper::operaCamera(const char *cameraid, bool preview, bool needExePreview, bool videoout, bool needExeVideoOut, bool needSetDefault, XCHCallBack callback)
{
	if (!needExePreview && !needExeVideoOut && !needSetDefault)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback,erc, "not found opera camera device");
		return erc;
	}

	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
	std::string str;
	getOperaCamera(str, cameraid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback,erc, "not found opera camera device");
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
				XCastHelperCallBack(callback,erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			XCastHelperCallBack(callback,erc, tencent::xcast::err_msg());
			return erc;
		}
	}
	
	if (needExeVideoOut)
	{
		// ������
		const char *streamid = m_stream_param->streamID.c_str();
		{
			int32_t ret = tencent::xcast::set_property(XC_TRACK_CAPTURE, streamid, "video-out", operdevID);
			if (ret != XCAST_OK)
			{
				int erc = avsdkErrorCode(ret);
				XCastHelperCallBack(callback,erc, tencent::xcast::err_msg());
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
					XCastHelperCallBack(callback,erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				XCastHelperCallBack(callback,erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
				return erc;
			}

		}
	}

	if (needSetDefault)
	{
		// ������
		int32_t ret = tencent::xcast::set_property(XC_CAMERA_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		XCastHelperCallBack(callback,erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	XCastHelperCallBack(callback,erc, "opera mic succ");
	return erc;
}

void XCastHelper::earseVideoFrameBuffer(uint64_t tinyid, XCastMediaSource source)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	char tinyid_src[256];
	sprintf(tinyid_src, "%lld_%d", tinyid, source);
	std::string key = tinyid_src;
	video_frame_map.erase(key);
}

XCastMediaSource XCastHelper::getDeviceVideoSourceType(XCastDeviceType type) const
{
	switch (type)
	{
	case XCastDeviceType_Camera:
		return XCastMediaSource_Camera;
	case XCastDeviceType_Screen_Capture:
		return XCastMediaSource_Screen_Capture;
	case XCastDeviceType_Player:
		return XCastMediaSource_Media_Player;
		// TODO:�������֧��
	default:
		return XCastMediaSource_Unknown;
		break;
	}
}

const std::shared_ptr<XCastVideoFrame> XCastHelper::getVideoFrameBuffer(const uint64_t tinyid, XCastMediaSource source)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	char tinyid_src[256];
	sprintf(tinyid_src, "%lld_%d", tinyid, source);
	std::string key = tinyid_src;
	auto it = video_frame_map.find(key);
	if (it == video_frame_map.end())
	{
		// ��������һ��
		std::shared_ptr<XCastVideoFrame> xvf(new XCastVideoFrame);
		xvf->tinyid = tinyid;
		xvf->media_source = source;
		video_frame_map.insert(std::make_pair(key, xvf));
		return xvf;
	}
	else
	{
		return it->second;
	}
}

std::shared_ptr<XCastEndpoint> XCastHelper::getEndpoint(uint64_t tinyid)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	auto it = endpoint_map.find(tinyid);
	if (it == endpoint_map.end())
	{
		// ��������һ��
		std::shared_ptr<XCastEndpoint> endptr(new XCastEndpoint);
		endptr->tinyid = tinyid;
		endpoint_map.insert(std::make_pair(tinyid, endptr));
		return endptr;
	}
	else
	{
		return it->second;
	}
}
void XCastHelper::updateEndpointMap(uint64_t tinyid)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	auto it = endpoint_map.find(tinyid);
	if (it != endpoint_map.end())
	{
		std::shared_ptr<XCastEndpoint> endptr = it->second;
		XCastEndpoint *end = endptr.get();
		if (end)
		{
			if (!end->is_audio || !end->is_camera_video || !end->is_screen_video || !end->is_media_video)
			{
				endpoint_map.erase(tinyid);
			}
		}
		
	}
}

void XCastHelper::deleteEndpoint(uint64_t tinyid)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	auto it = endpoint_map.find(tinyid);
	if (it != endpoint_map.end())
	{
		endpoint_map.erase(tinyid);
	}
}