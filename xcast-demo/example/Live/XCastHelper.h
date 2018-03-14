#pragma once

#include<mutex>
#include "xcast.hh"
#include "XCastData.h"

#define kForVipKidTest 1


class XCastHelper 
{
private:
	static XCastHelper					*m_instance;

private:
	std::unique_ptr<XCastStartParam>	m_startup_param;
	std::unique_ptr<XCastGlobalHandler> m_global_handler;
	std::unique_ptr<XCastRoomHandler>   m_room_handler;
	std::unique_ptr<XCastStreamParam>	m_stream_param;

	std::recursive_mutex				m_func_mutex;

private:
	bool is_startup_succ = false;

	typedef enum Room_State {
		Room_Closed,
		Room_Connecting,
		Room_Connectted

	}Room_State;
	Room_State stream_state = Room_Closed;

private:
	static int32_t onXCastSystemEvent(void *contextinfo, tencent::xcast_data &data);
	static int32_t onXCastStreamEvent(void *contextinfo, tencent::xcast_data &data);
	static int32_t onXCastTrackEvent(void *contextinfo, tencent::xcast_data &data);
	static int32_t onXCastDeviceEvent(void *contextinfo, tencent::xcast_data &data);
	static int32_t onXCastTipsEvent(void *contextinfo, tencent::xcast_data &data);

private:
	XCastHelper();
	~XCastHelper();


public:
	static XCastHelper* getInstance();
	

public:
	void startContext(std::unique_ptr<XCastStartParam> param, std::function<void(int32_t, char *)> callback);
	void stopContext(std::function<void(int32_t, char *)> callback);

public:
	void setGlobalHandler(std::unique_ptr<XCastGlobalHandler>  handler);

public:
	void enterRoom(std::unique_ptr<XCastStreamParam>m_stream_param,  std::unique_ptr<XCastRoomHandler>	roomDelegate,std::function<void(int32_t, char *)> callback);
	void exitRoom(std::function<void(int32_t, char *)> callback);


private:
	void clearAfterExitRoom();
	// Speaker操作
protected:
	
	// 对默认speaker操作
	int enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 指定默认的speaker 为 sid
	// 同时进行enable操作
	int enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});
	// 切换输出类型 , speaker : 1 扬声器 0 耳机
	int changeOutputMode(bool speaker);

	// 获取当前Speaker状态，
	int getSpeakerState();

	// mic操作
protected:
	// 对默认mic操作
	int enableMic(bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 对默认mic操作
	int enableMic(const char *micid, bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 切换mic
	int switchToMic(const char *micid,  std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 在房间内时，获取采集mic状态；
	// 在房间外时，获取default mic状态;
	int getMicState();

	int enableLoopBack(bool enable);

	// 摄像头操作
protected:
	// 在房间内时，获取采集Camera状态；
	// 在房间外时，获取default Camera状态;
	int getCameraState();

	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	/*
	* preview : 使用默认渲染进行预览 true：预览 false:不预览
	* enableVideoOut ：是否自动上行，房间外时不处理，房间内
	* cameraid ：指定摄像头id，可为空，为空使用默认摄像头;
	* callback ：回调
	*/
	int enableCamera(bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	int enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	int enableCamera(const char *cameraid, bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	int updateCameraMode(const char *cameraid, bool autoSending, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	int switchCamera(const char *cameraid, bool preview, bool campture, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	//int setRotation(int rotate);
	// enableExternalCapture()
	//int fillExternalCaptureFrame()

private:
	int avsdkErrorCode(int xcast_err_code);

};

