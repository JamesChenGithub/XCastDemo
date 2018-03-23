#include "XCastHelper.h"
#include "xcast.hh"
#include <algorithm>
#include <thread>
#include <future>
#include <mutex>

#if kForVipKidTest
#include "example/xcast-ui-handler.h"
#endif

//====================================================================

#define kTRACK_CAMERA_IN "video-in"
#define kTRACK_AUDIO_IN "audio-in"
#define kTRACK_SCREEN_CAPTURE_IN "screen-video-in"
#define kTRACK_MEDIA_IN "media-file-in"

#define kTRACK_CAMERA_OUT "video-out"
#define kTRACK_AUDIO_OUT "audio-out"
#define kTRACK_SCREEN_CAPTURE_OUT "screen-video-out"
#define kTRACK_MEDIA_OUT "media-file-out"


#define kAccountMutexTimeout 100
#define kEndpointMutexTimeout 10000

//====================================================================
inline void XCastHelperCallBack(XCHCallBack callback, int32_t errcode, const char *err)
{
	if (callback)
		callback(errcode, err);
}

bool xcast_data_to_deviceitem(tencent::xcast_data data, XCastDeviceHotPlugItem &item)
{
	const char *tstr = data.dump();
	const char *srcstr = data["src"];
	if (srcstr == nullptr)
	{
		// 没查到src，直接返回出错
		return false;
	}

	// 获取其他信息
	item.deviceClass = (XCastDeviceType)((int32_t)(data["class"]));
	item.src = std::string(srcstr);
	item.state = (XCastDeviceState)((int32_t)(data["state"]));
	item.err = data["err"];
	const char *str = data["err_msg"].str_val();
	item.err_msg = str ? std::string(str) : "";
	return true;
}

bool xcast_data_to_videoframe(tencent::xcast_data data, XCastVideoFrame &frame, bool isLocal)
{
	const char *dev = data["src"];
	const uint8_t *bytedata = data["data"].bytes_val();
	if (!dev || !bytedata)
	{
		// 没查到src，直接返回出错
		// 或数据有问题
		return false;
	}


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
		// 数据有变化，重新分配置内存
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

//=========================================================================

XCastHelper * XCastHelper::m_instance = nullptr;

XCastHelper::XCastHelper()
{

}


XCastHelper::~XCastHelper()
{
	// 清理资源
	clearAfterExitRoom();

	m_startup_param.reset();
	m_stream_param.reset();

	m_account_handler.reset();
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

	// TODO: 添加回调 
	return XCAST_OK;
}
int32_t XCastHelper::onXCastStreamEvent(void *contextinfo, tencent::xcast_data &data)
{
	//return XCAST_OK;
	XCastHelper *instance = (XCastHelper *)contextinfo;
#if kForVipKidTest
	char * str = data.dump();
	if (str)
		printf("%s\n", str);
#endif
	switch ((int32_t)data["type"]) {
	case xc_stream_added:
	{
		// 主要用于通知界面添加提逻辑，本质上没有太大的用处
		if (instance && instance->m_room_handler)
		{
			instance->m_room_handler->onWillEnterRoom(0, NULL);
		}
	}
	break;
	case xc_stream_updated:
	{
		if (data["state"] == xc_stream_connected)
		{
			instance->stream_state = Room_Connectted;
			/* 流状态： 连接成功 */
			if (instance && instance->m_room_handler)
			{
				instance->m_room_handler->onDidEnterRoom(0, NULL);
			}

			if (instance->m_stream_param.get())
			{
				const XCastRoomOption opera = instance->m_stream_param->roomOpera;

				// 打开摄像头
				instance->enableCamera(opera.autoCameraPreview, opera.autoCameraCapture, opera.defaultCamera.c_str());
				// 打开麦克风
				instance->enableMic(opera.autoMic);
				// 打开扬声器
				instance->enableSpeaker(opera.autoSpeaker);

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
			int errcode = (int)(data["err"]);
			int averrcode = avsdkErrorCode(errcode);
			const char *msg = (const char *)(data["err-msg"]);

			if (instance->stream_state == Room_Connectted)
			{
				// 退房
				if (instance->m_room_handler)
				{
					instance->m_room_handler->onExitRoomComplete(averrcode, msg);
				}
				instance->clearAfterExitRoom();
			}
			else if (instance->stream_state == Room_Connecting)
			{
				// 进房出现问题
				if (instance->m_room_handler)
				{
					instance->m_room_handler->onDidEnterRoom(errcode, msg);
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
	if (instance && instance->m_room_handler.get() && uin != 0)
	{
		xc_track_event type = (xc_track_event)((int32_t)data["type"]);

		switch (type)
		{
		case xc_track_added:
		{
		}
		break;
		case xc_track_updated:
		case xc_track_capture_changed:
		{
			const char *str = data.dump();
			XCastMediaSource source = (XCastMediaSource)((int32_t)data["media-src"]);
			xc_track_type tracktype = (xc_track_type)((int32_t)data["class"]);
			xc_track_state state = (xc_track_state)((int32_t)data["state"]);
			bool has = (state == xc_track_running);

			// 获取事件类型
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
					//case XCastMediaSource_PPT:               /* ppt */
				default:
					break;
				}
			}

			if (event != XCast_Endpoint_NONE)
			{
				if (instance->isSupportIMAccount())
				{
					// imsdk帐号模式下，先通过tinyid查到帐号，然后再通通知
					instance->getUserIDWithTinyid(uin, [=](std::string identifer, int errcode, std::string errmsg) {
						if (errcode == 0)
						{
							instance->notifyTrackEndpointEvent(uin, identifer, event, has);
						}
						else
						{
							// TODO: 获取userid失败情况下，如何处理
						}
					});
				}
				else
				{
					// tinyid模式下直接通知
					instance->notifyTrackEndpointEvent(uin, "", event, has);
				}
			}
		}
		break;
		case xc_track_removed:
		{

		}
		break;
		case xc_track_media:
		{
#if kForVipKidTest
			instance->logtoFile("xc_track_media", data.dump());
#endif
			// 获取远程视频 ，然后交给上层渲染
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
	const char *srcstr = e["src"];

	if (srcstr == nullptr)
	{
		return XCAST_OK;
	}

	XCastHelper *instance = (XCastHelper *)contextinfo;
	if (instance && instance->m_global_handler.get())
	{
		xc_device_event eventtype = (xc_device_event)((int32_t)(e["type"]));
		switch (eventtype)
		{
		case xc_device_added:
		{
			/* 设备插入 */
			XCastDeviceHotPlugItem device;
			if (xcast_data_to_deviceitem(e, device))
			{
				instance->m_global_handler->onDeviceEvent_DeviceAdd(device);
			}
		}
		break;
		case xc_device_updated:
		{
			// 设备更新
			XCastDeviceHotPlugItem device;
			if (xcast_data_to_deviceitem(e, device))
			{
				instance->m_global_handler->onDeviceEvent_DeviceUpdate(device);
			}

			// 移除本地的视频缓存
			if (device.state == XCastDeviceState_Stopped && (device.deviceClass == XCastDeviceType_Camera || device.deviceClass == XCastDeviceType_Screen_Capture || device.deviceClass == XCastDeviceType_Player))
			{
				uint64_t tinyid = instance->m_startup_param->tinyid;
				instance->earseVideoFrameBuffer(tinyid);
			}

		}
		break;
		case xc_device_removed:
		{
			/* 设备拔出 */
			XCastDeviceHotPlugItem device;
			if (xcast_data_to_deviceitem(e, device))
			{
				instance->m_global_handler->onDeviceEvent_DeviceRemoved(device);
			}

			// 移除本地的视频缓存
			if (device.deviceClass == XCastDeviceType_Camera || device.deviceClass == XCastDeviceType_Screen_Capture || device.deviceClass == XCastDeviceType_Player)
			{
				uint64_t tinyid = instance->m_startup_param->tinyid;
				instance->earseVideoFrameBuffer(tinyid);
			}
		}
		break;
		case xc_device_preprocess:
			/* 设备预处理 */
			// XCAST目前只有该类型，但无实质
			// 	ui_device_preprocess(e, user_data);
			// TODO:
			break;
		case xc_device_preview:
		{
			/* 设备预览 */
#if kForVipKidTest
			const char *cs = e.str_val();
			const char *str = e.dump();
#endif
			XCastDeviceType deviceType = (XCastDeviceType)((int32_t)(e["class"]));
			if (deviceType == XCastDeviceType_Camera || deviceType == XCastDeviceType_Screen_Capture || deviceType == XCastDeviceType_Player)
			{
				bool ng = instance->m_global_handler->needGlobalCallbackLocalVideo();
				bool nr = instance->m_room_handler && instance->m_room_handler->needRoomCallbackLocalVideo();
				if (ng || nr)
				{
					XCastMediaSource mediaTye = instance->getDeviceVideoSourceType(deviceType);
					std::shared_ptr<XCastVideoFrame> frameptr = instance->getVideoFrameBuffer(instance->m_startup_param->tinyid, mediaTye);
					XCastVideoFrame *frame = frameptr.get();
					frame->identifier = instance->m_startup_param->identifier;
					if (xcast_data_to_videoframe(e, *frame, true))
					{
						// 分批转发给上层
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
			else
			{
				// TODO:处理其他非视频类数据
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
#if kForVipKidTest
	instance->logtoFile("onXCastTipsEvent", data.dump());
#endif
	// TODO:待xcast提供json接口，返回json数据到业务上层
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

#if kForVipKidTest
void XCastHelper::logtoFile(const char *tag, const char * info)
{
	fprintf(logFile, "%s : %s \n", tag, info);
	fflush(logFile);
}
#endif
std::string XCastHelper::version()
{
	const char *ver = tencent::xcast::version();
	return std::string(ver);
}

bool XCastHelper::setAccountHandler(std::shared_ptr<XCastAccountHandler> handler)
{
	// 设置帐号体系，必须在startContext之前
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	if (!is_startup_succ)
	{
		m_account_handler.reset();
		m_account_handler = std::move(handler);
		return true;
	}
	return false;
}

const std::string XCastHelper::getStreamID() const
{
	if (stream_state != Room_Closed)
	{
		if (m_stream_param.get())
		{
			return std::string(m_stream_param->streamID);
		}
	}
	return "";
}

int XCastHelper::startContextWithout(std::unique_ptr<XCastStartParam> param, XCHCallBack callback)
{
#if kForVipKidTest
	logFile = fopen("log.txt", "w+");
#endif
	if (is_startup_succ)
	{
		XCastHelperCallBack(callback, 1003, "xcast has started");
		return 1003;
	}

	if (!param.get() || !param->isVaild())
	{
		XCastHelperCallBack(callback, 1004, "param error");
		return 1004;
	}
	m_startup_param.reset();
	m_startup_param = std::move(param);
	// TODO：参数检查 

	tencent::xcast_data setparam;
	setparam["app_id"] = m_startup_param->sdkappid;
	setparam["identifier"] = m_startup_param->tinyid;
	setparam["test_env"] = m_startup_param->isTestEvn;
	int32_t rt = tencent::xcast::startup(setparam);

	is_startup_succ = (rt == XCAST_OK);

	if (is_startup_succ && isSupportIMAccount())
	{
		// 登录成功后，直接可以拿到缓存列表
		atomicAccountCache([&] {
			m_account_cache.insert(std::make_pair(m_startup_param->tinyid, m_startup_param->identifier));
		});
	}


	if (is_startup_succ)
	{
		///* 注册事件通知回调 */
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

int XCastHelper::startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (is_startup_succ)
	{
		XCastHelperCallBack(callback, 1003, "xcast has started");
		return 1003;
	}
	bool useim = isSupportIMAccount();
	if (useim)
	{
		if (!param.get() || !param->isVaildIM())
		{
			XCastHelperCallBack(callback, 1004, "param error");
			return 1004;
		}

		if (m_account_handler.get())
		{
			// IMSDK 登录成功后，getTinyId > 0，否则 < 0
			uint64_t tinyid = m_account_handler->getTinyId();
			if (tinyid == 0)
			{
				XCastHelperCallBack(callback, 6014, "IMSK not login");
				return 6014;
			}
			else
			{
				param->tinyid = tinyid;
				return startContextWithout(std::move(param), callback);
			}
		}
		else
		{
			XCastHelperCallBack(callback, 1004, "param error");
			return 1004;
		}
	}
	else
	{
		return startContextWithout(std::move(param), callback);
	}

}
int XCastHelper::stopContext(XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
#if kForVipKidTest
	fflush(logFile);
	fclose(logFile);
	logFile = nullptr;
#endif
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
		XCastHelperCallBack(callback, 1003, "xcast is streaming");
		return 1003;
	}
	if (!(param.get()) || !(param->isVaild()))
	{
		XCastHelperCallBack(callback, 1004, "xcast start stream param error");
		return 1004;
	}

	clearAfterExitRoom();

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
	int code = avsdkErrorCode(rt);
	if (XCAST_OK != rt) {
		// TODO：进房失败；
		// ui_xcast_err(rt, xcast_err_msg(), user_data);

		XCastHelperCallBack(callback, code, (char *)xcast_err_msg());
		clearAfterExitRoom();
		return code;
	}

	// 因每次退房都会清除掉帐号缓存，进房间时，再把自己的重新加上
	atomicAccountCache([&] {
		m_account_cache.insert(std::make_pair(m_startup_param->tinyid, m_startup_param->identifier));
	});

	XCastHelperCallBack(callback, code, "xcast_start_stream succ");
	stream_state = Room_Connecting;
	return code;

}
int XCastHelper::exitRoom(XCHCallBack callback)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

	if (stream_state != Room_Connectted)
	{
		XCastHelperCallBack(callback, 1004, "xcast isn't streaming");
		return 1004;
	}

	if (m_stream_param->roomOpera.autoCloseCameraOnExit)
	{
		enableCameraPreview(false);
	}

	// 添加退房时，关摄像头的操作

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

	std::for_each(video_frame_map.begin(), video_frame_map.end(), [&](std::pair<std::string, std::shared_ptr<XCastVideoFrame>> pair) {
		auto vfp = pair.second;
		vfp.reset();
	});
	video_frame_map.clear();

	// 清理缓存的endpoint数据
	{
		std::lock_guard<std::mutex> el(m_endpoint_mutex);
		m_endpoint_map.clear();
	}
	// 清理缓存的帐号数据
	if (isSupportIMAccount())
	{
		atomicAccountCache([&] {
			m_account_cache.clear();
		});
	}
}

std::vector<std::string> XCastHelper::getSpeakerList() const
{
	return getDeviceList(XCastDeviceType_Speaker);
}

// 设置默认扬声器
int XCastHelper::setDefaultSpeaker(const char *sid, XCHCallBack callback)
{
	return operaSpeaker(sid, false, false, false, false, true, callback);
}

std::string XCastHelper::getDefaultSpeaker() const
{
	return getOperaSpeaker();
}

// 指定默认的speaker 为 sid
// 同时进行enable操作
int XCastHelper::enableSpeaker(bool enable, const char *sid, XCHCallBack callback)
{
	return operaSpeaker(sid, false, false, enable, stream_state == Room_Connectted, false, callback);
}


int XCastHelper::enableSpeakerPreview(bool preview, const char *sid, XCHCallBack callback)
{
	return operaSpeaker(sid, preview, true, false, false, false, callback);
}


int XCastHelper::enableSpeaker(bool preview, bool enable, const char *sid, XCHCallBack callback)
{
	return operaSpeaker(sid, preview, true, enable, stream_state == Room_Connectted, false, callback);
}
int XCastHelper::switchSpeaker(bool preview, bool enable, bool setSidAsDefault, const char *sid, XCHCallBack callback)
{
	return operaSpeaker(sid, preview, true, enable, stream_state == Room_Connectted, setSidAsDefault, callback);
}
// 切换输出类型 , speaker : 1 扬声器 0 耳机
int XCastHelper::changeOutputMode(bool headphone, const char *sid)
{
	std::string oidstr = getOperaSpeaker(sid);;
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
	std::string oidstr = getOperaSpeaker(sid);

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
	std::string oidstr = getOperaSpeaker(sid);
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
std::vector<std::string> XCastHelper::getMicList() const
{
	return getDeviceList(XCastDeviceType_Mic);
}


int XCastHelper::setDefaultMic(const char *sid, XCHCallBack callback)
{
	return operaMic(sid, false, false, false, false, true, callback);
}

/*
* 功能：获取默认麦克风
* 返回：默认麦克风（UTF-8格式串），为空即没有麦克风设备
*/
std::string XCastHelper::getDefaultMic() const
{
	return getOperaMic();
}


// 在房间内时，获取采集mic状态；
// 在房间外时，获取default mic状态;
XCastDeviceState XCastHelper::getMicState(const char *micid) const
{
	std::string  operidstr = getOperaMic(micid);
	const char *operid = operidstr.c_str();
	if (operid == nullptr)
	{
		return XCastDeviceState_NotFound;
	}

	tencent::xcast_data state = tencent::xcast::get_property(XC_MIC_STATE, tencent::xcast_data(operid));
	XCastDeviceState  statea = (XCastDeviceState)(state.int16_val());
	return statea;
}


int XCastHelper::enableMic(bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, false, false, enableAudioOut, stream_state == Room_Connectted, false, callback);
}

int XCastHelper::enableMicPreview(bool preview, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, preview, true, false, false, false, callback);
}

int XCastHelper::enableMic(bool preview, bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, preview, true, enableAudioOut, stream_state == Room_Connectted, false, callback);
}

int XCastHelper::switchMic(bool preview, bool enableAudioOut, bool setDefault, const char *micid, XCHCallBack callback)
{
	return operaMic(micid, preview, true, enableAudioOut, stream_state == Room_Connectted, setDefault, callback);
}

// 摄像头操作

std::vector<std::string> XCastHelper::getCameraList() const
{
	return getDeviceList(XCastDeviceType_Camera);
}

/*
* 功能：设置默认摄像头
* sid ：为空或为错误的，默认摄像头不会变更，业务上层保证传入值的合法性
* 返回值 : 0 成功，非0失败
*/
int XCastHelper::setDefaultCamera(const char *sid, XCHCallBack callback)
{
	return operaCamera(sid, false, false, false, false, true, callback);
}

/*
* 功能：获取默认摄像头
* 返回：默认麦克风（UTF-8格式串），为空即没有麦克风设备
*/
std::string XCastHelper::getDefaultCamera() const
{
	return getOperaCamera();
}


// 在房间内时，获取采集Camera状态；
// 在房间外时，获取default Camera状态;
XCastDeviceState XCastHelper::getCameraState(const char *cameraid) const
{
	std::string  operidstr = getOperaCamera(cameraid);
	const char *operid = operidstr.c_str();
	if (operid == nullptr)
	{
		return XCastDeviceState_NotFound;
	}

	tencent::xcast_data state = tencent::xcast::get_property(XC_CAMERA_STATE, tencent::xcast_data(operid));
	XCastDeviceState statea = (XCastDeviceState)(state.int16_val());
	return statea;
}

// 在房间内时：打开摄像头，并预览，同时上行；
// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；

int XCastHelper::enableCamera(bool campture, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, false, false, campture, stream_state == Room_Connectted, false, callback);
}
int XCastHelper::enableCameraPreview(bool preview, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, preview, true, false, false, false, callback);
}

int XCastHelper::enableCamera(bool preview, bool enableVideoOut, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, preview, true, enableVideoOut, stream_state == Room_Connectted, false, callback);
}

int XCastHelper::switchCamera(bool preview, bool enableVideoOut, bool setDefault, const char *cameraid, XCHCallBack callback)
{
	return operaCamera(cameraid, preview, true, enableVideoOut, stream_state == Room_Connectted, setDefault, callback);
}

void XCastHelper::requestView(XCastRequestViewItem item, XCHReqViewListCallBack callback)
{
	remoteView(item, true, callback, true);
}

void XCastHelper::requestAudio(XCastRequestViewItem item, XCHReqViewListCallBack callback)
{
	remoteView(item, true, callback, false);
}


void XCastHelper::requestViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback)
{
	std::for_each(itemList.begin(), itemList.end(), [&](XCastRequestViewItem item) {
		requestView(item, callback);
	});
}

void XCastHelper::requestAllView(XCHReqViewListCallBack callback)
{
	remoteAllView(true, callback);
}


void XCastHelper::cancelView(XCastRequestViewItem item, XCHReqViewListCallBack callback)
{
	remoteView(item, false, callback,true);
}

void XCastHelper::cancelAudio(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack)
{
	remoteView(item, false, callback, false);
}

void XCastHelper::cancelViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback)
{
	std::for_each(itemList.begin(), itemList.end(), [&](XCastRequestViewItem item) {
		cancelView(item, callback);
	});
}


void XCastHelper::cancelAllView(XCHReqViewListCallBack callback)
{
	remoteAllView(false, callback);
}


int XCastHelper::getSpeakerDynamicVolume(std::string trackid) const
{
	tencent::xcast_data data = tencent::xcast::get_property(XC_SPEAKER_DYNAMIC_VOLUME, trackid.c_str());
	uint32_t volume = data.uint32_val();
	return volume;
}
int XCastHelper::getSpeakerDynamicVolume(uint64_t tinyid) const
{
	char trackid[XCAST_MAX_PATH];
	sprintf(trackid, "%s-%llu", kTRACK_AUDIO_IN, tinyid);
	std::string trackstr = trackid;
	return getSpeakerDynamicVolume(trackid);
}

std::string XCastHelper::getOperaDevice(XCastDeviceType type, const char *cameraid) const
{
	char operdevID[XCAST_MAX_PATH];
	if (cameraid == nullptr || strlen(cameraid) == 0)
	{
		const char *devicetyep = nullptr;

		switch (type)
		{

		case XCastDeviceType_Camera:
			devicetyep = XC_CAMERA_DEFAULT;
			break;
		case XCastDeviceType_Screen_Capture:
			break;
		case XCastDeviceType_Player:
			break;
		case XCastDeviceType_Mic:
			devicetyep = XC_MIC_DEFAULT;
			break;
		case XCastDeviceType_Speaker:
			devicetyep = XC_SPEAKER_DEFAULT;
			break;
		case XCastDeviceType_Accompany:
			break;
		case XCastDeviceType_External:
			break;
		default:
			break;
		}

		if (devicetyep == nullptr)
		{
			return "";
		}
		tencent::xcast_data data = tencent::xcast::get_property(devicetyep);
#if kForVipKidTest
		const char *dstr = data.dump();
#endif
		const char *str_val = data.str_val();
		if (str_val == nullptr)
		{
			return "";
		}
		else
		{
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
	return getOperaDevice(XCastDeviceType_Camera, cameraid);
}

std::string XCastHelper::getOperaSpeaker(const char *cameraid) const
{
	return getOperaDevice(XCastDeviceType_Speaker, cameraid);
}

std::string XCastHelper::getOperaMic(const char *cameraid) const
{
	return getOperaDevice(XCastDeviceType_Mic, cameraid);
}


int XCastHelper::avsdkErrorCode(int xcast_err_code)
{
	// TODO : xcast err code 转成AVSDK错误码
	return xcast_err_code;
}

std::vector<std::string> XCastHelper::getDeviceList(XCastDeviceType type) const
{
	// 查询设备列表
	std::vector<std::string> devlist;
	if (is_startup_succ)
	{
		const char *devicetyep = nullptr;

		switch (type)
		{
		case XCastDeviceType_Camera:
			devicetyep = XC_CAMERA;
			break;
		case XCastDeviceType_Screen_Capture:
			break;
		case XCastDeviceType_Player:
			break;
		case XCastDeviceType_Mic:
			devicetyep = XC_MIC;
			break;
		case XCastDeviceType_Speaker:
			devicetyep = XC_SPEAKER;
			break;
		case XCastDeviceType_Accompany:
			break;
		case XCastDeviceType_External:
			devicetyep = XC_DEVICE_EXTERNAL;
			break;
		default:
			break;
		}

		if (devicetyep == nullptr)
		{
			return devlist;
		}

		tencent::xcast_data captures = tencent::xcast::get_property(devicetyep);
		/*	const char *datat = captures.dump();*/
		const char     *cap = nullptr;
		for (uint32_t n = 0; n < captures.size(); n++) {
			cap = captures[n].str_val();
			if (cap)
			{
				devlist.push_back(std::string(cap));
			}
		}
	}
	return devlist;
}


XCastDeviceState XCastHelper::getDeviceState(XCastDeviceType type, const char *device) const
{
	// 查询设备状态
	if (type == XCastDeviceType_Speaker)
	{
		return XCastDeviceState_NotFound;
	}

	std::string  operidstr = getOperaDevice(type, device);
	const char *operid = operidstr.c_str();
	if (operid == nullptr)
	{
		return XCastDeviceState_NotFound;
	}

	const char *devicetyep = nullptr;


	switch (type)
	{
	case XCastDeviceType_Camera:
		devicetyep = XC_CAMERA_STATE;
		break;
	case XCastDeviceType_Screen_Capture:
		break;
	case XCastDeviceType_Player:
		break;
	case XCastDeviceType_Mic:
		devicetyep = XC_MIC_STATE;
		break;
	case XCastDeviceType_Speaker:
		break;
	case XCastDeviceType_Accompany:
		break;
	case XCastDeviceType_External:
		devicetyep = XC_DEVICE_EXTERNAL_STATE;
		break;
	default:
		break;
	}
	if (devicetyep == nullptr)
	{
		return XCastDeviceState_NotFound;
	}

	tencent::xcast_data state = tencent::xcast::get_property(devicetyep, tencent::xcast_data(operid));
	XCastDeviceState statea = (XCastDeviceState)(state.int16_val());
	return statea;
}


int XCastHelper::operaMic(const char *micid, bool preview, bool needExePreview, bool audioout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback)
{
	// 操作mic
	if (!needExePreview && !needExeAudioOut && !needSetDefault)
	{
		int ret = avsdkErrorCode(XCAST_OK);
		XCastHelperCallBack(callback, ret, "do nothing");
		return ret;
	}

	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string str = getOperaMic(micid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback, erc, "not found opera mic device");
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
				int erc = avsdkErrorCode(preret);
				XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
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
				XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
				return erc;
			}
		}

		if (stream_state == Room_Connectted)
		{
			tencent::xcast_data data, params;
			params["enable"] = audioout;
			//data["params"] = params;
			int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, streamid, "audio-out", params);

			if (needSetDefault)
			{
				if (enret != XCAST_OK)
				{
					int erc = avsdkErrorCode(enret);
					XCastHelperCallBack(callback, erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				XCastHelperCallBack(callback, erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
				return erc;
			}
		}
	}

	if (needSetDefault)
	{
		// 房间外
		int32_t ret = tencent::xcast::set_property(XC_MIC_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		XCastHelperCallBack(callback, erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	XCastHelperCallBack(callback, erc, "opera mic succ");
	return erc;

}

int XCastHelper::operaCamera(const char *cameraid, bool preview, bool needExePreview, bool videoout, bool needExeVideoOut, bool needSetDefault, XCHCallBack callback)
{
	if (!needExePreview && !needExeVideoOut && !needSetDefault)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback, erc, "not found opera camera device");
		return erc;
	}

	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string str = getOperaCamera(cameraid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback, erc, "not found opera camera device");
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
				int erc = avsdkErrorCode(preret);
				XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
			return erc;
		}
	}

	if (needExeVideoOut)
	{
		// 房间内
		const char *streamid = m_stream_param->streamID.c_str();
		
		{
			int32_t ret = tencent::xcast::set_property(XC_TRACK_CAPTURE, streamid, kTRACK_CAMERA_OUT, operdevID);
			if (ret != XCAST_OK)
			{
				int erc = avsdkErrorCode(ret);
				XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
				return erc;
			}
		}


		if (stream_state == Room_Connectted)
		{
			tencent::xcast_data params;
			params["enable"] = videoout;
			int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, streamid, kTRACK_CAMERA_OUT, params);

			if (needSetDefault)
			{
				if (enret != XCAST_OK)
				{
					int erc = avsdkErrorCode(enret);
					XCastHelperCallBack(callback, erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				XCastHelperCallBack(callback, erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable audio-out succ");
				return erc;
			}

		}
	}

	if (needSetDefault)
	{
		// 房间外
		int32_t ret = tencent::xcast::set_property(XC_CAMERA_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		XCastHelperCallBack(callback, erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	XCastHelperCallBack(callback, erc, "opera mic succ");
	return erc;
}

int XCastHelper::operaSpeaker(const char *micid, bool preview, bool needExePreview, bool audioout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback)
{
	// 操作mic
	if (!needExePreview && !needExeAudioOut && !needSetDefault)
	{
		int ret = avsdkErrorCode(XCAST_OK);
		XCastHelperCallBack(callback, ret, "do nothing");
		return ret;
	}

	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	std::string str = getOperaSpeaker(micid);
	if (str.length() == 0)
	{
		int erc = avsdkErrorCode(-104);
		XCastHelperCallBack(callback, erc, "not found opera speaker device");
		return erc;
	}
	const char *operdevID = str.c_str();
	if (needExePreview)
	{
		int32_t preret = tencent::xcast::set_property(XC_SPEAKER_PREVIEW, operdevID, tencent::xcast_data(preview));

		if (needExeAudioOut || needSetDefault)
		{
			if (preret != XCAST_OK)
			{
				int erc = avsdkErrorCode(preret);
				XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
				return erc;
			}
		}
		else
		{
			int erc = avsdkErrorCode(preret);
			XCastHelperCallBack(callback, erc, tencent::xcast::err_msg());
			return erc;
		}

	}

	if (needExeAudioOut)
	{
		if (stream_state == Room_Connectted)
		{
			tencent::xcast_data data, params;
			params["enable"] = audioout;
			//data["params"] = params;
			int32_t enret = tencent::xcast::set_property(XC_SPEAKER_ENABLE, params);

			if (needSetDefault)
			{
				if (enret != XCAST_OK)
				{
					int erc = avsdkErrorCode(enret);
					XCastHelperCallBack(callback, erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable speaker succ");
					return erc;
				}
			}
			else
			{
				int erc = avsdkErrorCode(enret);
				XCastHelperCallBack(callback, erc, enret != XCAST_OK ? tencent::xcast::err_msg() : "enable speaker succ");
				return erc;
			}
		}
	}

	if (needSetDefault)
	{
		// 房间外
		int32_t ret = tencent::xcast::set_property(XC_SPEAKER_DEFAULT, operdevID);
		int erc = avsdkErrorCode(ret);
		XCastHelperCallBack(callback, erc, ret != XCAST_OK ? tencent::xcast::err_msg() : "set defaul mic succ");
		return erc;
	}

	int erc = avsdkErrorCode(XCAST_OK);
	XCastHelperCallBack(callback, erc, "opera mic succ");
	return erc;

}

void XCastHelper::earseVideoFrameBuffer(uint64_t tinyid, XCastMediaSource source)
{
	if (tinyid != 0)
	{
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
		char tinyid_src[256];
		sprintf(tinyid_src, "%llu_%d", tinyid, source);
		std::string key = tinyid_src;
		video_frame_map.erase(key);
	}
}

void XCastHelper::earseVideoFrameBuffer(uint64_t tinyid)
{
	if (tinyid != 0)
	{
		earseVideoFrameBuffer(tinyid, XCastMediaSource_Camera);
		earseVideoFrameBuffer(tinyid, XCastMediaSource_Screen_Capture);
		earseVideoFrameBuffer(tinyid, XCastMediaSource_Media_Player);
	}
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
	default:
		// TODO:添加其他支持
		return XCastMediaSource_Unknown;
		break;
	}
}

const std::shared_ptr<XCastVideoFrame> XCastHelper::getVideoFrameBuffer(const uint64_t tinyid, XCastMediaSource source)
{
	std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
	char tinyid_src[256];
	sprintf(tinyid_src, "%llu_%d", tinyid, source);
	std::string key = tinyid_src;
	auto it = video_frame_map.find(key);
	if (it == video_frame_map.end())
	{
		// 重新生成一个
		std::shared_ptr<XCastVideoFrame> xvf(new XCastVideoFrame);
		xvf->tinyid = tinyid;
		xvf->identifier = syncGetUserid(tinyid);
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
	std::lock_guard<std::mutex> lock(m_endpoint_mutex);
	auto it = m_endpoint_map.find(tinyid);
	if (it == m_endpoint_map.end())
	{
		// 重新生成一个
		std::shared_ptr<XCastEndpoint> endptr(new XCastEndpoint);
		endptr->tinyid = tinyid;
		m_endpoint_map.insert(std::make_pair(tinyid, endptr));


		getUserIDWithTinyid(tinyid, [=](std::string userid, int err, std::string errmsg) {
			if (err == 0 && userid.length() != 0)
			{
				endptr->identifier = userid;
				atomicAccountCache([&] {
					m_account_cache.insert(std::make_pair(tinyid, userid));
				});
			}
		});
		return endptr;
	}
	else
	{
		return it->second;
	}
}
void XCastHelper::updateEndpointMap(uint64_t tinyid, XCastEndpointEvent event)
{
	std::lock_guard<std::mutex> lock(m_endpoint_mutex);
	auto it = m_endpoint_map.find(tinyid);
	if (it != m_endpoint_map.end())
	{
		std::shared_ptr<XCastEndpoint> endptr = it->second;
		XCastEndpoint *end = endptr.get();
		if (end)
		{
			switch (event)
			{
			case XCast_Endpoint_No_Camera_Video:
				earseVideoFrameBuffer(end->tinyid, XCastMediaSource_Camera);
				break;
			case XCast_Endpoint_No_Screen_Video:
				earseVideoFrameBuffer(end->tinyid, XCastMediaSource_Screen_Capture);
				break;
			case XCast_Endpoint_No_Media_Video:
				earseVideoFrameBuffer(end->tinyid, XCastMediaSource_Media_Player);
				break;
			default:
				break;
			}

			if (!end->is_audio && !end->is_camera_video && !end->is_screen_video && !end->is_media_video)
			{
				m_endpoint_map.erase(tinyid);
				// 同时清除frame_map中对应的数据
			}
		}
	}
}

void XCastHelper::deleteEndpoint(uint64_t tinyid)
{
	std::lock_guard<std::mutex> lock(m_endpoint_mutex);
	auto it = m_endpoint_map.find(tinyid);
	if (it != m_endpoint_map.end())
	{
		m_endpoint_map.erase(tinyid);
	}
}

XCastRequestViewItem XCastHelper::getFromTrackID(std::string track)
{
	const std::string videostr = kTRACK_CAMERA_IN;
	const std::string subvideostr = kTRACK_SCREEN_CAPTURE_IN;
	const std::string mediavideostr = kTRACK_MEDIA_IN;

	XCastRequestViewItem item;

	if (track.find(videostr) == 0)
	{
		std::string uinstr = track.substr(videostr.length() + 1);
		uint64_t uin = strtoull(uinstr.c_str(), 0, 10);
		item.tinyid = uin;
		item.identifer = syncGetUserid(uin);
		item.video_src = XCastMediaSource_Camera;
		return item;
	}
	else if (track.find(subvideostr) == 0)
	{
		std::string uinstr = track.substr(subvideostr.length() + 1);
		uint64_t uin = strtoull(uinstr.c_str(), 0, 10);
		item.tinyid = uin;
		item.video_src = XCastMediaSource_Screen_Capture;
		return item;
	}
	else if (track.find(mediavideostr) == 0)
	{
		std::string uinstr = track.substr(mediavideostr.length() + 1);
		uint64_t uin = strtoull(uinstr.c_str(), 0, 10);
		item.tinyid = uin;
		item.video_src = XCastMediaSource_Media_Player;
		return item;
	}
	return item;
}
void XCastHelper::remoteViewWithTinyid(XCastRequestViewItem item, bool enable, XCHReqViewListCallBack callback, bool isrequestVideo)
{
	if ((isrequestVideo && !item.isVaild()) || (!isrequestVideo && item.tinyid == 0))
	{
		if (callback)
		{
			callback(item, 1004, "item is invaild");
		}

		return;
	}

	tencent::xcast_data data, params;
	params["enable"] = enable;
	
	std::string trackid = "";
	if (isrequestVideo)
	{
		switch (item.video_src)
		{
		case XCastMediaSource_Camera:
			trackid = kTRACK_CAMERA_IN;
			break;
		case XCastMediaSource_Screen_Capture:
			trackid = kTRACK_SCREEN_CAPTURE_IN;
			break;
		case XCastMediaSource_Media_Player:
			trackid = kTRACK_MEDIA_IN;
			break;
		default:
			break;
		}
	}
	else
	{
		trackid = kTRACK_AUDIO_IN;
	}
	

	if (trackid.length() == 0)
	{
		if (callback)
		{
			callback(item, 1004, "item is invaild");
		}
		return;
	}

	
	trackid += "-";
	trackid += std::to_string(item.tinyid);
	int32_t enret = tencent::xcast::set_property(XC_TRACK_ENABLE, m_stream_param->streamID.c_str(), trackid.c_str(), params);

	if (callback)
	{
		int code = avsdkErrorCode(enret);
		callback(item, code, code == XCAST_OK ? "" : enable ? "request view failed" : "cancel view failed");
	}

}
void XCastHelper::remoteView(XCastRequestViewItem item, bool enable, XCHReqViewListCallBack callback, bool isrequestVideo)
{
	if (isSupportIMAccount() && callback)
	{
		if ((isrequestVideo && item.video_src == XCastMediaSource_Unknown) || item.identifer.length() == 0)
		{
			if (callback)
			{
				callback(item, 1004, "item is invaild");
			}
			return;
		}

		if (item.identifer == m_startup_param->identifier)
		{
			if (callback)
			{
				callback(item, 1004, "can't request self track");
			}
			return;
		}

		getTinyIDWithUserID(item.identifer, [&](uint64_t uin, int code, std::string msg) {
			if (code == 0 && uin != 0)
			{
				item.tinyid = uin;
				remoteViewWithTinyid(item, enable, callback, isrequestVideo);
			}
			else
			{
				if (callback)
				{
					callback(item, 1004, "get tinyid failed");
				}
			}

		});
	}
	else
	{

		if (isrequestVideo && !item.isVaild())
		{
			if (callback)
			{
				callback(item, 1004, "item is invaild");
			}
			return;
		}

		if (item.tinyid == m_startup_param->tinyid)
		{
			if (callback)
			{
				callback(item, 1004, "can't request self track");
			}
			return;
		}
		remoteViewWithTinyid(item, enable, callback, isrequestVideo);
	}

}

void XCastHelper::remoteAllView(bool enable, XCHReqViewListCallBack callback)
{
	const char *streamid = m_stream_param->streamID.c_str();
	tencent::xcast_data tracklist = tencent::xcast::get_property(XC_STREAM_TRACK, streamid);
	uint32_t tlsize = tracklist.size();
	const std::string videostr = kTRACK_CAMERA_IN;
	const std::string subvideostr = kTRACK_SCREEN_CAPTURE_IN;
	const std::string mediavideostr = kTRACK_MEDIA_IN;
	for (uint32_t n = 0; n < tlsize; n++)
	{
		std::string track = tracklist[n].str_val();
		XCastRequestViewItem item = getFromTrackID(track);
		if (item.isVaild())
		{
			remoteView(item, enable, callback,true);
		}
	}
}

inline bool XCastHelper::isSupportIMAccount() const
{
	return m_account_handler.get() ? m_account_handler->useIMSDKasAccount() : false;
}

std::string XCastHelper::syncGetUserid(uint64_t tinyid)
{
	std::string ep = "";
	atomicAccountCache([=, &ep] {
		auto endptr = m_account_cache.find(tinyid);
		if (endptr != m_account_cache.end())
		{
			ep = endptr->second;
		}
	});
	return ep;
}

void XCastHelper::getUserIDWithTinyid(uint64_t tinyid, std::function<void(std::string, int, std::string)> callback)
{
	if (callback)
	{
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
		if (tinyid == 0)
		{
			callback("", 1004, "tinyid is wrong");
			return;
		}
		else
		{

			std::string ep = "";
			atomicAccountCache([=, &ep] {
				auto endptr = m_account_cache.find(tinyid);
				if (endptr != m_account_cache.end())
				{
					ep = endptr->second;
				}
			});

			if (ep.length() > 0)
			{
				callback(ep, 0, "");
			}
			else
			{
				getUserIDWithTinyidFromIMSDK(tinyid, callback);
			}
		}
	}
}

void XCastHelper::getUserIDWithTinyid(std::vector<uint64_t> tinyidlist, std::function<void(std::vector<std::string>, int, std::string)> callback)
{
	std::vector<std::string> userid;
	// 找到第一个不在列表里的元素
	atomicAccountCache([=, &userid] {
		std::find_if_not(tinyidlist.begin(), tinyidlist.end(), [&](uint64_t tinyid)->bool {
			auto endptr = m_account_cache.find(tinyid);
			if (endptr != m_account_cache.end())
			{
				std::string ep = endptr->second;
				userid.push_back(ep);
				return true;
			}
			return false;
		});
	});


	if (userid.size() == tinyidlist.size())
	{
		if (callback)
		{
			callback(userid, 0, "");
		}
	}
	else
	{
		getUserIDWithTinyidFromIMSDK(tinyidlist, callback);
	}
}

void XCastHelper::getUserIDWithTinyidFromIMSDK(uint64_t tinyid, std::function<void(std::string, int, std::string)> callback)
{
	if (m_account_handler.get() && m_account_handler->useIMSDKasAccount() && callback)
	{
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

		std::vector<uint64_t> tinyidvec;
		tinyidvec.push_back(tinyid);
		m_account_handler->tinyid_to_identifier(tinyidvec, [=](std::vector<uint64_t> tinyidlist, std::vector<std::string> identifierlist, int errcode, std::string errmsg) {

			if (errcode == 0 && tinyidlist.size() == identifierlist.size())
			{
				uint64_t uin = tinyidlist[0];
				std::string identifier = identifierlist[0];
				if (identifier.length() > 0)
				{
					atomicAccountCache([&] {
						m_account_cache.insert(std::make_pair(uin, identifier));
					});
					if (callback)
					{
						callback(identifier, 0, "");
					}

					return;
				}
			}

			if (callback)
			{
				callback("", 1004, "get the  identifier of tinyid  failed");
			}
		});
	}
}

void XCastHelper::getUserIDWithTinyidFromIMSDK(std::vector<uint64_t> tinyidlist, std::function<void(std::vector<std::string>, int, std::string)> callback)
{
	if (isSupportIMAccount() && callback)
	{
		if (tinyidlist.empty())
		{
			callback(std::vector<std::string>(), 1004, "tinyidlist is empty");
			return;
		}
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

		m_account_handler->tinyid_to_identifier(tinyidlist, [=](std::vector<uint64_t> tlist, std::vector<std::string> list, int errcode, std::string errmsg) {

			if (errcode == 0 && list.size() != 0 && list.size() == tlist.size())
			{

				std::vector<std::pair<uint64_t, std::string>> pairs;

				for (uint32_t i = 0; i < list.size(); i++)
				{
					uint64_t tinyid = tlist[i];
					std::string identifier = list[i];
					if (tinyid != 0 && identifier.length() > 0)
					{
						pairs.push_back(std::make_pair(tinyid, identifier));
					}
				}
				if (!pairs.empty())
				{
					atomicAccountCache([&] {
						m_account_cache.insert(pairs.begin(), pairs.end());
					});
				}

				if (callback)
				{
					callback(list, 0, "");
				}
			}
			else
			{
				if (callback)
				{
					callback(std::vector<std::string>(), errcode, "get tinyid failed");
				}
			}
		});
	}
}

uint64_t XCastHelper::syncGetTinyid(std::string userid)
{
	typedef std::map<uint64_t, std::string> AccoutCache;

	auto func = [&](std::pair<uint64_t, std::string> pair)->bool {
		return pair.second == userid;
	};

	if (m_cache_mutex.try_lock_for(std::chrono::milliseconds(kAccountMutexTimeout)))
	{
		const AccoutCache::const_iterator it = std::find_if(m_account_cache.begin(), m_account_cache.end(), func);

		if (it != m_account_cache.end() && it->first != 0)
		{
			m_cache_mutex.unlock();
			return it->first;
		}
		else
		{
			m_cache_mutex.unlock();
			return 0;
		}

	}
	return 0;
}
void XCastHelper::getTinyIDWithUserID(std::string userid, std::function<void(uint64_t, int, std::string)> callback)
{
	if (callback)
	{
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);
		if (userid.length() == 0)
		{
			callback(0, 1004, "userid is empty");
			return;
		}
		else
		{
			uint64_t uin = syncGetTinyid(userid);
			if (uin != 0)
			{
				callback(uin, 0, "");
			}
			else
			{
				getTinyIDWithUserIDFromIMSDK(userid, callback);
			}
		}
	}
}
void XCastHelper::getTinyIDWithUserIDFromIMSDK(std::string userid, std::function<void(uint64_t, int, std::string)> callback)
{
	if (isSupportIMAccount() && callback)
	{
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

		std::vector<std::string> useridlist;
		useridlist.push_back(userid);
		m_account_handler->identifier_to_tinyid(useridlist, [=](std::vector<uint64_t> list, std::vector<std::string> idlist, int errcode, std::string errmsg) {

			if (errcode == 0 && !list.empty() && list.size() == idlist.size())
			{
				uint64_t  tinyid = list[0];
				if (tinyid != 0)
				{
					if (callback)
					{
						callback(tinyid, 0, "");
					}
					atomicAccountCache([&] {
						m_account_cache.insert(std::make_pair(tinyid, userid));
					});
					return;
				}
				errcode = 1004;
			}
			callback(0, errcode, "get tinyid failed");
		});
	}
}

void XCastHelper::getTinyIDWithUserID(std::vector<std::string> useridlist, std::function<void(std::vector<uint64_t>, int, std::string)> callback)
{
	std::vector<uint64_t> tinyidlist;
	// 找到第一个不在列表里的元素
	std::find_if_not(useridlist.begin(), useridlist.end(), [&](std::string userid)->bool {
		uint64_t  uin = syncGetTinyid(userid);
		if (uin == 0)
		{
			return true;
		}
		else
		{
			tinyidlist.push_back(uin);
			return false;
		}


	});

	if (useridlist.size() == tinyidlist.size())
	{
		if (callback)
		{
			callback(tinyidlist, 0, "");
		}
	}
	else
	{
		getTinyIDWithUserIDFromIMSDK(useridlist, callback);
	}
}
void XCastHelper::getTinyIDWithUserIDFromIMSDK(std::vector<std::string> useridlist, std::function<void(std::vector<uint64_t>, int, std::string)> callback)
{
	if (isSupportIMAccount() && callback)
	{
		std::lock_guard<std::recursive_mutex> lock(m_func_mutex);

		m_account_handler->identifier_to_tinyid(useridlist, [=](std::vector<uint64_t> list, std::vector<std::string> idlist, int errcode, std::string errmsg) {
			if (errcode == 0 && list.size() == idlist.size() && !list.empty())
			{
				std::vector<std::pair<uint64_t, std::string>> pairs;
				for (uint32_t i = 0; i < list.size(); i++)
				{
					uint64_t tinyid = list[i];
					std::string identifier = idlist[i];
					if (tinyid != 0 && identifier.length() > 0)
					{
						pairs.push_back(std::make_pair(tinyid, identifier));
					}
				}
				if (callback)
				{
					callback(list, 0, "");
				}
				atomicAccountCache([&] {
					m_account_cache.insert(pairs.begin(), pairs.end());
				});
			}
			else
			{
				if (callback)
				{
					callback(std::vector<std::uint64_t>(), 1004, "get tinyid failed");
				}
			}
		});
	}

}

void XCastHelper::notifyTrackEndpointEvent(uint64_t uin, std::string userid, XCastEndpointEvent event, const bool has)
{
	bool notify = false;
	std::shared_ptr<XCastEndpoint> end = getEndpoint(uin);
	if (end.get())
	{
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
			ep.identifier = userid;
			ep.is_audio = end->is_audio;
			ep.is_camera_video = end->is_camera_video;
			ep.is_screen_video = end->is_screen_video;
			ep.is_media_video = end->is_media_video;

			if (m_room_handler.get())
			{
				m_room_handler->onEndpointsUpdateInfo(event, ep);
			}
			updateEndpointMap(uin, event);
		}
	}

}


void XCastHelper::atomicAccountCache(std::function<void()> func)
{
	using MS = std::chrono::milliseconds;
	if (m_cache_mutex.try_lock_for(MS(kAccountMutexTimeout)))
	{
		func();
		m_cache_mutex.unlock();
	}
}
