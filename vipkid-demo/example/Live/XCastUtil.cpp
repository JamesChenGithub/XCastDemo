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

int XCastUtil::enableSpeaker(bool enable, const char *sid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableSpeaker(enable,sid,callback);
}

int XCastUtil::changeOutputMode(bool headphone, const char *sid)
{
	return XCastHelper::getInstance()->changeOutputMode(headphone,sid);
}

int XCastUtil::speakerVolume(const char *sid)
{
	return XCastHelper::getInstance()->speakerVolume(sid);
 }


int XCastUtil::setSpeakerVolume(int volume, const char *sid)
{
	return XCastHelper::getInstance()->setSpeakerVolume(volume, sid);
}


// mic²Ù×÷

 std::vector<std::string> XCastUtil::getMicList()
{
	 return XCastHelper::getInstance()->getMicList();
}

 XCastDeviceState XCastUtil::getMicState(const char *micid)
 {
	 return XCastHelper::getInstance()->getMicState(micid);
 }


int XCastUtil::enableMic(bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableMic(enableAudioOut,micid,callback);
}

int XCastUtil::enableMicPreview(bool preview, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableMicPreview(preview, micid, callback);
}

int XCastUtil::enableMic(bool preview, bool enableAudioOut, const char *micid, XCHCallBack callback)
{
	return XCastHelper::getInstance()->enableMic(preview, enableAudioOut, micid, callback);
}

std::vector<std::string> XCastUtil::getCameraList()
{
	return XCastHelper::getInstance()->getCameraList();
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
	 XCastHelper::getInstance()->cancelView(item,callback);
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


