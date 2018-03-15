#ifndef XCAST_OBSERVET_H_
#define XCAST_OBSERVET_H_

#include "XCastData.h"

class XCastObserver : public XCastGlobalHandler, public XCastRoomHandler
{

private:
	bool has_enter_room = false;
public:
	XCastObserver();
	virtual ~XCastObserver();

	//==========================================================
	// 设备回调
	virtual void onDeviceEvent_DeviceAdd(XCastDeviceHotPlugItem device);
	virtual void onDeviceEvent_DeviceUpdate(XCastDeviceHotPlugItem device);
	virtual void onDeviceEvent_DeviceRemoved(XCastDeviceHotPlugItem device);

	// XCAST系统回调
	//virtual void onSystemEvent(void *contextinfo, tencent::xcast_data &data);
	virtual void onSystemEvent();

	// 视频事件
	virtual bool needGlobalCallbackLocalVideo();
	virtual void onGlobalLocalVideoPreview(XCastVideoFrame *frame);


	//===============================================
	
	virtual void onDidEnterRoom(int result, const char *error);
	virtual void onExitRoomComplete(int result, const char *error);
	virtual void onRoomDisconnected(int result, const char *error);

	virtual void onEndpointsUpdateInfo(XCastEndPoint info);
	virtual bool needRoomCallbackLocalVideo();
	virtual void onLocalVideoPreview(XCastVideoFrame *frame);

	virtual void onVideoPreview(XCastVideoFrame *frame);

	virtual bool needRoomCallbackTips();
	virtual void onStatTips();
};

#endif // !XCAST_OBSERVET_H_



