#include "XCastObserver.h"
#include <iostream>
#include <sstream>
#include <algorithm>

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
	return true;
}
void XCastObserver::onGlobalLocalVideoPreview(const XCastVideoFrame *frame)
{

}


//===============================================
void XCastObserver::onWillEnterRoom(int result, const char *error)
{

}
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

void XCastObserver::onEndpointsUpdateInfo(XCastEndpointEvent event, XCastEndpoint infos)
{
	std::ostringstream stream;
	stream << "onEndpointsUpdateInfo" << " : event = " << event << "  , infos.tinyid = " << infos.tinyid << std::endl;
	std::cout << stream.str() << std::endl;
}
bool XCastObserver::needRoomCallbackLocalVideo()
{
	return has_enter_room;
}
void XCastObserver::onLocalVideoPreview(const XCastVideoFrame *frame)
{

}

void XCastObserver::onVideoPreview(const XCastVideoFrame *frame)
{

}

bool XCastObserver::needRoomCallbackTips()
{
	return has_enter_room;
}
void XCastObserver::onStatTips()
{

}
#ifdef kSupportIMAccount

uint64_t XCastObserver::getTinyId()
{
	return 67890;
}

bool XCastObserver::useIMSDKasAccount() const
{
	return kSupportIMAccount;
}

void XCastObserver::tinyid_to_identifier(std::vector<uint64_t> tinyidlist, XCastAccountCallBack func)
{
	if (func)
	{
		if (tinyidlist.empty())
		{
			func(std::vector<uint64_t>(), std::vector<std::string>(), 1004, "tinyidlist is empty");
			return;
		}
		std::thread([=] {

			std::shared_ptr<std::vector<uint64_t>> tptr(new std::vector<uint64_t>(tinyidlist));
		

			std::this_thread::sleep_for(std::chrono::microseconds(500));
			std::vector<std::string> idlist;
			std::for_each(tptr->begin(), tptr->end(), [&](uint64_t tinyid) {
				char idetifier[256];
				sprintf(idetifier, "%llu", tinyid);
				idlist.push_back(std::string(idetifier));
			});
			if (func)
			{
				func(*tptr, idlist, 0, "");
			}
			
		}).detach();
	}
	
	
}

void XCastObserver::identifier_to_tinyid(std::vector<std::string> identifiedlist, XCastAccountCallBack func)
{
	if (func)
	{
		if (identifiedlist.empty())
		{
			func(std::vector<uint64_t>(), std::vector<std::string>(), 1004, "identifiedlist is empty");
			return;
		}
		std::thread([=]() {


			std::shared_ptr<std::vector<std::string>> tptr(new std::vector<std::string>(identifiedlist));
			//XCastAccountCallBack funca = std::move(func);

			std::this_thread::sleep_for(std::chrono::microseconds(500));
			std::vector<uint64_t> tinyidlist;

			std::for_each(tptr->begin(), tptr->end(), [&](std::string identifier) {
				uint64_t tinyid = strtoull(identifier.c_str(), nullptr, 10);
				tinyidlist.push_back(tinyid);
			});

			if (func)
			{
				func(tinyidlist, *tptr, 0, "");
			}
		}).detach();
	}
	
}

#endif