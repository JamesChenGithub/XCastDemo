#pragma once

//#include "xcast_data.h"
#include <string>
#include <vector>
#include <functional>


/*xcast auth type*/
typedef enum XCastAuthType {
	XCastAuth_None = 0,
	XCastAuth_Manual,
	XCastAuth_Auto,
} XCastAuthType;

/* xcast system events */
typedef enum XCastSystemEvent {
	XCastSystemEvent_Normal = 1,
	XCastSystemEvent_Warning,
	XCastSystemEvent_Error,
	XCastSystemEvent_Fatal,
} XCastSystemEvent;

/* xcast stream events */
typedef enum XCastStreamEvent {
	XCastStream_Added = 1,                /* new stream added */
	XCastStream_Updated,                  /* stream state updated */
	XCastStream_Removed,                  /* stream removed */
} XCastStreamEvent;

/* xcast stream events */
typedef enum  XCastStreamState {
	XCastStreamState_Connecting = 1,           /* stream connecting */
	XCastStreamState_Connected,                /* stream connected */
	XCastStreamState_Closed,                   /* stream connected */
} XCastStreamState;

/* xcast stream track events */
typedef enum XCastTrackEvent {
	XCastTrack_Added = 1,                 /* new track added */
	XCastTrack_Updated,                   /* track state updated */
	XCastTrack_Removed,                   /* track removed */
	XCastTrack_Capture_Changed,           /* track capture changed */
	XCastTrack_Media,                     /* track media data coming */
} XCastTrackEven;

/* xcast stream track running state */
typedef enum XCastTrackState {
	XCastTrackState_Stopped = 1,               /* track stopped */
	XCastTrackState_Running,                   /* track is running */
} XCastTrackState;

/* xcast track types */
typedef enum XCastTrackType {
	XCastTrackType_Msg = 1,                   /* msg track */
	XCastTrackType_Audio,                     /* audio track */
	XCastTrackType_Video,                     /* video track */
} XCastTrackType;

/* xcast track types */
typedef enum XCastTrackDirection {
	XCastTrackDirection_None = 0,			// Preview???????
	XCastTrackDirection_Out = 1,                   /* send media data to remote host */
	XCastTrackDirection_In,                        /* receive media data from remote host */
} XCastTrackDirection;

/* xcast media types */
typedef enum XCastMediaFormat {
	XCastMedia_argb32 = 0,                /* argb32 video format */
	XCastMedia_i420,                      /* i420 video format */
	XCastMedia_aac,                       /* aac audio format */
	XCastMedia_pcm,                       /* pcm audio format */
	XCastMedia_layer = 0xFF,              /* layer format*/
} XCastMediaFormat;

/* xcast media source types */
typedef enum XCastMediaSource {
	XCastMediaSource_Unknown = 0,        /* unknown source type */
	XCastMediaSource_Camera,             /* camera */
	XCastMediaSource_Screen_Capture,     /* screen capture */
	XCastMediaSource_Media_Player,       /* media player */
	XCastMediaSource_PPT,                /* ppt */
} XCastMediaSource;

/* xcast device running state */
typedef enum XCastDeviceType {
	XCastDeviceType_Unknown = 0,              /* unknown device type */
	XCastDeviceType_Camera,                   /* camera device */
	XCastDeviceType_Screen_Capture,           /* screnn capture */
	XCastDeviceType_Player,                   /* media player */
	XCastDeviceType_Mic,                      /* microphone device */
	XCastDeviceType_Speaker,                  /* speaker device */
	XCastDeviceType_Accompany,                /* audio acompany device */
	XCastDeviceType_External,                 /* external device type */
} XCastDeviceType;

/* xcast stream track events */
typedef enum XCastDeviceEvent {
	XCastDeviceEvent_Added = 1,                /* new device added */
	XCastDeviceEvent_Updated,                  /* device state updated */
	XCastDeviceEvent_Removed,                  /* device removed */
	XCastDeviceEvent_Preprocess,               /* device preprocess data coming */
	XCastDeviceEvent_Preview,                  /* device preview data coming */
} XCDeviceEvent;

/* xcast device running state */
typedef enum  XCastDeviceState {
	XCastDeviceState_NotFound = 0,
	XCastDeviceState_Stopped = 1,
	XCastDeviceState_Running,
} XCastDeviceState;

typedef enum XCastEndpointEvent {
	XCast_Endpoint_NONE = 0, ///< Ĭ��ֵ�������塣
	//XCast_Endpoint_Enter = 1, ///< ���뷿���¼���
	//XCast_Endpoint_Exit = 2, ///< �˳������¼���						 
	XCast_Endpoint_Has_Camera_Video = 3, ///< �з�����ͷ��Ƶ�¼���
	XCast_Endpoint_No_Camera_Video = 4, ///< �޷�����ͷ��Ƶ�¼���
	XCast_Endpoint_Has_Audio = 5, ///< �з������¼���
	XCast_Endpoint_No_Audio = 6, ///< �޷������¼���
	XCast_Endpoint_Has_Screen_Video = 7, ///< �з���Ļ��Ƶ�¼���
	XCast_Endpoint_No_Screen_Video = 8, ///< �޷���Ļ��Ƶ�¼���
	XCast_Endpoint_Has_Media_Video = 9, ///< �з��ļ���Ƶ�¼���
	XCast_Endpoint_No_Media_Video = 10, ///< �޷��ļ���Ƶ�¼���
}XCastEndpointEvent;

//===========================================================
struct XCastDeviceHotPlugItem;
struct XCastVideoFrame;
struct XCastEndpoint;

#define kSupportIMAccount 0
#ifdef kSupportIMAccount
class XCastAccountHandler
{
public:
	virtual void tinyid_to_identifier(uint64_t tinyid,std::function<void(std::string)> func) = 0;
	virtual void identifier_to_tinyid(std::string identifier, std::function<void(uint64_t)> func) = 0;
};
#endif

class XCastGlobalHandler
{
public:

	// �豸�Ȳ��
	virtual void onDeviceEvent_DeviceAdd(XCastDeviceHotPlugItem deviceItem) = 0;
	virtual void onDeviceEvent_DeviceUpdate(XCastDeviceHotPlugItem deviceItem) = 0;
	virtual void onDeviceEvent_DeviceRemoved(XCastDeviceHotPlugItem deviceItem) = 0;

	// XCASTϵͳ�ص�
	//virtual void onSystemEvent(void *contextinfo, tencent::xcast_data &data) = 0;
	virtual void onSystemEvent() = 0;
	// ��Ƶ�¼�
	virtual bool needGlobalCallbackLocalVideo() = 0;
	virtual void onGlobalLocalVideoPreview(XCastVideoFrame *frame) = 0;
};

class  XCastRoomHandler
{
public:
	//virtual void onWillEnterRoom(int result, const char *error) = 0;
	virtual void onDidEnterRoom(int result, const char *error) = 0;
	virtual void onExitRoomComplete(int result, const char *error) = 0;
	virtual void onRoomDisconnected(int result, const char *error) = 0;

	virtual void onEndpointsUpdateInfo(XCastEndpointEvent event,std::vector<XCastEndpoint> infos) = 0;
	virtual bool needRoomCallbackLocalVideo() = 0;
	virtual void onLocalVideoPreview(XCastVideoFrame *frame) = 0;

	virtual void onVideoPreview(XCastVideoFrame *frame) = 0;

	virtual bool needRoomCallbackTips() = 0;
	virtual void onStatTips() = 0;
};

typedef struct XCastStartParam {
#ifdef kSupportIMAccount
	std::string identifier;
#endif
	uint64_t tinyid = 0;
	bool isTestEvn = false;
	int32_t sdkappid = 0;
	uint32_t accounttype = 0;

	bool isVaild() const
	{
		if ((tinyid == 0 ) || sdkappid == 0 || accounttype == 0)
		{
			return false;
		}
		return true;
	}
} XCastStartParam;



typedef struct XCastStreamAuthInfo
{
	XCastAuthType auth_type = XCastAuth_None;
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
		if (auth_type == XCastAuth_None)
		{
			return true;
		}
		else if (auth_type == XCastAuth_Manual)
		{
			return auth_buffer != nullptr && auth_buffer_size != 0;
		}
		else if (auth_type == XCastAuth_Auto)
		{
			if (expire_time < 0 || secret_key.length() == 0)
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
	bool ext_audio_playback = false;
}XCastStreamTrackInfo;


typedef struct XCastRoomOpera {
	std::string defaultCamera = "";
	std::string defaultMic = "";
	std::string defaultSpeaker = "";

	bool autoCameraCapture = true;
	bool autoCameraPreview = true;
	bool autoMic = true;
	bool autoSpeaker = true;
	bool autoCloseCameraOnExit = true;

}XCastStreamDevice;

typedef struct XCastStreamParam {
	int32_t roomid;
	std::string role;
	bool auto_recv = true;
	std::string streamID;
	XCastStreamAuthInfo auth_info;
	XCastStreamTrackInfo track;

	// TODO:��Ͷ�����Լ�������ͨ���ֶ� 

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
//
//
//
//
//
//return XCAST_OK;

typedef struct XCastVideoFrame
{
	uint64_t			tinyid = 0;
	//std::string			identifier;
	XCastMediaFormat	media_format = XCastMedia_argb32;
	XCastMediaSource	media_source = XCastMediaSource_Unknown;
	uint8_t *data = nullptr;
	uint32_t size = 0;
	uint32_t width = 0;
	uint32_t height = 0;
	int rotate = 0;  // [0, 90, 180, 270]
	int direction;
	std::string deviceSrc;

	~XCastVideoFrame()
	{
		if (data)
		{
			free(data);
			data = nullptr;
		}
	}
}XCastVideoFrame;


// �Ȳ�λص��������¼�
typedef struct XCastDeviceHotPlugItem {

	XCastDeviceState state = XCastDeviceState_Stopped;				// ״̬
	XCastDeviceType deviceClass = XCastDeviceType_Unknown;			// ���� 
	std::string src = "";											// ����
	int32_t err = 0;												// ������
	std::string err_msg = "";									// ������Ϣ	
}XCastDeviceHotPlugItem;





///*
//* xcast֧�ֵ�ý��������¼�
//* "event.track":{
//*   // �¼�Դ: ����� ý��������
//*   "*src":"vstring",
//*   "*stream":"vstring",
//*   // �¼�����: ���������£�ɾ��,ý������
//*   "*type":[xc_track_event],
//*   // ������: xc_track_audio,xc_track_video
//*   "*class":[xc_track_type],
//*   // �������: ����,����
//*   "*direction":[xc_track_direction],
//*   // �û�uin
//*   "*uin":"vuint64",
//*   // ������
//*   "*index":"vuint32",
//*   // ���״̬
//*   "state":[xc_track_state],
//*   // �������
//*   "err":"vint32",
//*   // ������Ϣ
//*   "err-msg":"vstring",
//*   // ������ݸ�ʽ
//*   "format":[xc_media_format],
//*   // ������ݻ���
//*   "data":"vbytes",
//*   // ������ݻ��泤��
//*   "size":"vuint32",
//*   // ������ݿ��
//*   "width":"vuint32",
//*   // ������ݸ߶�
//*   "height":"vuint32",
//*   // ���������ת
//*   "rotate":[0,90,180,270]
//*   // ���������ƵԴ:����ͷ����Ļ����ý���ļ���PPT��δ֪Դ
//*   "media-src":[xc_media_source]
//* },
//*/
//
//+str	0x010cb1b8 "[\"src\":\"video-in-12345\",
//\"state\":int32(1),\"type\":int32(1),
//\"media_type\":int32(1),\"direction\":int32(2),\"stream\":\"stream1\",
//\"uin\":uint64(12345),\"err\":int32(0),\"class\":int32(3),\"index\":uint32(0)]"	const char *



typedef struct XCastEndpoint {
	uint64_t tinyid = 0;
	bool is_audio = false;
	bool is_camera_video = false;
	bool is_screen_video = false;
	bool is_media_video = false;
}XCastEndpoint;