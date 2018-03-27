#ifndef XCAST_OBSERVET_H_
#define XCAST_OBSERVET_H_

#include "XCastData.h"

class XCastObserver : public XCastGlobalHandler, public XCastRoomHandler
#ifdef kSupportIMAccount
	, public XCastAccountHandler
#endif
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
	virtual void onGlobalLocalVideoPreview(const XCastVideoFrame *frame);


	//===============================================
	virtual void onWillEnterRoom(int result, const char *error);
	virtual void onDidEnterRoom(int result, const char *error);
	virtual void onExitRoomComplete(int result, const char *error);
	virtual void onRoomDisconnected(int result, const char *error);

	virtual void onEndpointUpdateInfo(XCastEndpointEvent event, XCastEndpoint infos);
	virtual void onEndpointCaptureUpdate(XCastEndpointEvent event, XCastEndpoint infos);
	virtual bool needRoomCallbackLocalVideo();
	virtual void onLocalVideoPreview(const XCastVideoFrame *frame);

	virtual void onVideoPreview(const XCastVideoFrame *frame);

	virtual bool needRoomCallbackTips();
	virtual void onStatTips();

#ifdef kSupportIMAccount
	//==================================================
	virtual bool useIMSDKasAccount() const;
	virtual uint64_t getTinyId();
	virtual void tinyid_to_identifier(std::vector<uint64_t> tinyidlist, XCastAccountCallBack func);
	virtual void identifier_to_tinyid(std::vector<std::string> identifiedlist, XCastAccountCallBack func);
#endif
};

#endif // !XCAST_OBSERVET_H_



