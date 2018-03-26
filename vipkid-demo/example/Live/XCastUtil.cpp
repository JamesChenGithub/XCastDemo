#include "XCastUtil.h"
#include "XCastHelper.h"

#include <mutex>
#include <vector>
#include <map>

std::string XCastUtil::version()
{
	return XCastHelper::version();
}

bool XCastUtil::setAccountHandler(std::shared_ptr<XCastAccountHandler> handler)
{
	return XCastHelper::getInstance()->setAccountHandler(handler);
}

int XCastUtil::startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback)
{
	return XCastHelper::getInstance()->startContext(std::move(param), callback);
}

int XCastUtil::stopContext(XCHCallBack callback)
{
	return XCastHelper::getInstance()->stopContext(callback);
}

void XCastUtil::setGlobalHandler(std::shared_ptr<XCastGlobalHandler>  handler)
{
	return XCastHelper::getInstance()->setGlobalHandler(handler);
}

int XCastUtil::enterRoom(std::unique_ptr<XCastStreamParam>roomoption, std::shared_ptr<XCastRoomHandler>	roomDelegate, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enterRoom(std::move(roomoption), roomDelegate, callback);
}

int XCastUtil::exitRoom(XCHCallBack callback)
{
	return XCastHelper::getInstance()->exitRoom(callback);
}

std::vector<std::string> XCastUtil::getSpeakerList()
{
	return XCastHelper::getInstance()->getSpeakerList();
}

/*
* 功能：获取默认扬声器
* 返回：默认扬声器（UTF-8格式串），为空即没有扬声器设备
*/
std::string XCastUtil::getDefaultSpeaker()
{
	return XCastHelper::getInstance()->getDefaultSpeaker();
}

int XCastUtil::setDefaultSpeaker(const char *sid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->setDefaultSpeaker(sid, callback);
}

int XCastUtil::enableSpeaker(bool enable, const char *sid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableSpeaker(enable, sid, callback);
}
int XCastUtil::enableSpeakerPreview(bool preview, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableSpeakerPreview(preview, micid, callback);
}

int XCastUtil::enableSpeaker(bool preview, bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableSpeaker(preview, enableAudioOut, micid, callback);
}
//int XCastUtil::switchSpeaker(bool preview, bool enable, bool setSidAsDefault, const char *sid, XCHCallBack callback )
//{
//	return XCastHelper::getInstance()->switchSpeaker(preview, enable, setSidAsDefault, sid, callback);
//}

int XCastUtil::changeOutputMode(bool headphone, const char *sid)
{
	return XCastHelper::getInstance()->changeOutputMode(headphone, sid);
}

int XCastUtil::speakerVolume(const char *sid)
{
	return XCastHelper::getInstance()->speakerVolume(sid);
}


int XCastUtil::setSpeakerVolume(int volume, const char *sid)
{
	return XCastHelper::getInstance()->setSpeakerVolume(volume, sid);
}


// mic操作

std::vector<std::string> XCastUtil::getMicList()
{
	return XCastHelper::getInstance()->getMicList();
}

XCastDeviceState XCastUtil::getMicState(const char *micid)
{
	return XCastHelper::getInstance()->getMicState(micid);
}

int XCastUtil::setDefaultMic(const char *sid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->setDefaultMic(sid, callback);
}
std::string XCastUtil::getDefaultMic()
{
	return XCastHelper::getInstance()->getDefaultMic();
}

std::string XCastUtil::getCaptureMic()
{
	return XCastHelper::getInstance()->getCaptureMic();
}

int XCastUtil::enableMic(bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableMic(enableAudioOut, micid, callback);
}

int XCastUtil::enableMicPreview(bool preview, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableMicPreview(preview, micid, callback);
}

int XCastUtil::enableMic(bool preview, bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableMic(preview, enableAudioOut, micid, callback);
}

 int XCastUtil::switchMic(bool preview, bool enableAudioOut, bool setDefault , const char *micid , XCHCallBack callback )
{
	 return XCastHelper::getInstance()->switchMic(preview, enableAudioOut, setDefault, micid, callback);
}

std::vector<std::string> XCastUtil::getCameraList()
{
	return XCastHelper::getInstance()->getCameraList();
}

int XCastUtil::setDefaultCamera(const char *sid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->setDefaultCamera(sid, callback);
}

std::string XCastUtil::getDefaultCamera()
{
	return XCastHelper::getInstance()->getDefaultCamera();
}

std::string XCastUtil::getCaptureCamera()
{
	return XCastHelper::getInstance()->getCaptureCamera();
}

XCastDeviceState XCastUtil::getCameraState(const char *cameraid)
{
	return XCastHelper::getInstance()->getCameraState(cameraid);
}

int XCastUtil::enableCamera(bool enableVideoOut, const char *cameraid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableCamera(enableVideoOut, cameraid, callback);
}

int XCastUtil::enableCameraPreview(bool preview, const char *cameraid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableCameraPreview(preview, cameraid, callback);
}

int XCastUtil::enableCamera(bool preview, bool enableVideoOut, const char *cameraid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableCamera(preview, enableVideoOut, cameraid, callback);
}

 int XCastUtil::switchCamera(bool preview, bool enableVideoOut, bool setDefault, const char *cameraid , XCHCallBack callback )
{
	 return XCastHelper::getInstance()->switchCamera(preview, enableVideoOut, setDefault, cameraid, callback);
}

void XCastUtil::requestView(XCastRequestViewItem item, XCHReqViewListCallBack callback)
{
	XCastHelper::getInstance()->requestView(item, callback);
}

void XCastUtil::requestViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback)
{
	XCastHelper::getInstance()->requestViewList(itemList, callback);
}

void XCastUtil::requestAllView(XCHReqViewListCallBack callback)
{
	XCastHelper::getInstance()->requestAllView(callback);
}

void XCastUtil::cancelView(XCastRequestViewItem item, XCHReqViewListCallBack callback)
{
	XCastHelper::getInstance()->cancelView(item, callback);
}

void XCastUtil::cancelViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback)
{
	XCastHelper::getInstance()->cancelViewList(itemList, callback);
}
void XCastUtil::cancelAllView(XCHReqViewListCallBack callback)
{
	XCastHelper::getInstance()->cancelAllView(callback);
}

int XCastUtil::getSpeakerDynamicVolume(uint64_t tinyid)
{
	return  XCastHelper::getInstance()->getSpeakerDynamicVolume(tinyid);
}


