#include "XCastObserver.h"
#include <iostream>
#include <sstream>

#include "XCastHelper.h"


XCastObserver::XCastObserver()
{
}


XCastObserver::~XCastObserver()
{
}

//==========================================================
// 设备回调
void XCastObserver::onDeviceEvent_DeviceAdd(XCastDeviceHotPlugItem device)
{
	std::ostringstream stream;
	stream << "onDeviceEvent_DeviceAdd" << " : class = " << device.deviceClass << "  , src = " << device.src << " , state = " << device.state << " , err = " << device.err << " , err_msg =" << device.err_msg << std::endl;
	std::cout << stream.str() << std::endl;

	if (device.deviceClass == XCastDeviceType_Camera)
	{
		XCastHelper::getInstance()->enableCameraPreview(true);
	}
}
void XCastObserver::onDeviceEvent_DeviceUpdate(XCastDeviceHotPlugItem device)
{
	std::ostringstream stream;
	stream << "onDeviceEvent_DeviceUpdate" << " : class = " << device.deviceClass << "  , src = " << device.src << " , state = " << device.state << " , err = " << device.err << " , err_msg =" << device.err_msg << std::endl;
	std::cout << stream.str() << std::endl;
}
void XCastObserver::onDeviceEvent_DeviceRemoved(XCastDeviceHotPlugItem device)
{

}

void XCastObserver::onSystemEvent()
{

}

//// XCAST系统回调
//void XCastObserver::onSystemEvent(void *contextinfo, tencent::xcast_data &data)
//{
//
//}

// 视频事件
bool XCastObserver::needGlobalCallbackLocalVideo()
{
	return has_enter_room;
}
void XCastObserver::onGlobalLocalVideoPreview(XCastVideoFrame *frame)
{

}


//===============================================

void XCastObserver::onDidEnterRoom(int result, const char *error)
{
	has_enter_room = true;
}
void XCastObserver::onExitRoomComplete(int result, const char *error)
{
	has_enter_room = false;
}
void XCastObserver::onRoomDisconnected(int result, const char *error)
{
	has_enter_room = false;
}

void XCastObserver::onEndpointsUpdateInfo(XCastEndPoint info)
{

}
bool XCastObserver::needRoomCallbackLocalVideo()
{
	return has_enter_room;
}
void XCastObserver::onLocalVideoPreview(XCastVideoFrame *frame)
{

}

void XCastObserver::onVideoPreview(XCastVideoFrame *frame)
{

}

bool XCastObserver::needRoomCallbackTips()
{
	return has_enter_room;
}
void XCastObserver::onStatTips()
{

}