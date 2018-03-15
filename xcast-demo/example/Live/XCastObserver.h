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
	// �豸�ص�
	virtual void onDeviceEvent_DeviceAdd(XCastDeviceHotPlugItem device);
	virtual void onDeviceEvent_DeviceUpdate(XCastDeviceHotPlugItem device);
	virtual void onDeviceEvent_DeviceRemoved(XCastDeviceHotPlugItem device);

	// XCASTϵͳ�ص�
	//virtual void onSystemEvent(void *contextinfo, tencent::xcast_data &data);
	virtual void onSystemEvent();

	// ��Ƶ�¼�
	virtual bool needGlobalCallbackLocalVideo();
	virtual void onGlobalLocalVideoPreview(XCastVideoFrame *frame);


	//===============================================
	
	virtual void onDidEnterRoom(int result, const char *error);
	virtual void onExitRoomComplete(int result, const char *error);
	virtual void onRoomDisconnected(int result, const char *error);

	virtual void onEndpointsUpdateInfo(XCastEndpointEvent event, std::vector<XCastEndpoint> infos);
	virtual bool needRoomCallbackLocalVideo();
	virtual void onLocalVideoPreview(XCastVideoFrame *frame);

	virtual void onVideoPreview(XCastVideoFrame *frame);

	virtual bool needRoomCallbackTips();
	virtual void onStatTips();

#ifdef kSupportIMAccount
	//==================================================
	virtual void tinyid_to_identifier(uint64_t tinyid, std::function<void(std::string)> func);
	virtual void identifier_to_tinyid(std::string identifier, std::function<void(uint64_t)> func);
#endif
};

#endif // !XCAST_OBSERVET_H_


