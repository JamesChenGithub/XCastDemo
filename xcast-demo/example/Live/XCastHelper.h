#pragma once

#include <mutex>
#include <vector>
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

	typedef enum DeviceType
	{
		Device_Camera,
		Device_Mic,
		Device_Speaker,
	}DeviceType;

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

	// 获取扬声器列表, 结果是同步查询，外部不要保存结果
	// 返回：扬声器列表（UTF-8格式串，外部进行转码）
	std::vector<std::string> getSpeakerList() const;

	// 对默认speaker操作
	int enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 指定默认的speaker 为 sid
	// 同时进行enable操作
	int enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});
	
	// 切换输出类型 , speaker : 1 扬声器 0 耳机
	int changeOutputMode(bool speaker);

	
protected:
	// mic操作
	// 获取麦克风列表, 结果是同步查询，外部不要保存结果
	// 返回：麦克风列表（UTF-8格式串，外部进行转码）
	std::vector<std::string> getMicList() const;

	// 在房间内时，获取采集mic状态；
	// 在房间外时，获取default mic状态;
	int getMicState();

	// 对默认mic操作
	int enableMic(bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 对默认mic操作
	int enableMic(const char *micid, bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// 切换mic
	int switchToMic(const char *micid,  std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});



	int enableLoopBack(bool enable);

	// 摄像头操作
protected:
	// 获取摄像头列表, 结果是同步查询，外部不要保存
	// 返回：摄像头列表（UTF-8格式串，外部进行转码）
	std::vector<std::string> getCameraList() const;

	// cameraid : 摄像头id,可以为空，为空则查默认摄像头的状态
	// 返回值 ：
	// 0 : 未找到设备
	// 1 : 停止
	// 2 ：运行中
	int getCameraState(const char *cameraid = nullptr) const;

	// 以下几个接口控制摄像头预览以及上行控制；
	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；
	
	/*
	* preview : 使用默认渲染进行预览 true：预览 false:不预览
	* enableVideoOut ：是否自动上行，房间外时不处理，房间内
	* callback ：回调
	* 返回值 : 操作错误码，0代码成功，其他失败
	*/
	int enableCamera(bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	/*
	* 只是预览，但不控制上行
	* cameraid : 摄像头id,可为空，为空查默认摄像头
	* preview : 使用默认渲染进行预览 true：预览 false:不预览
	* callback ：回调
	* 返回值 : 操作错误码，0代码成功，其他失败
	*/
	int enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	/*
	* 控制预览以及上行，不会控制已在预览的摄像头
	* cameraid : 摄像头id,可为空，为空查默认摄像头
	* preview : 使用默认渲染进行预览 true：预览 false:不预览
	* callback ：回调
	* 返回值 : 操作错误码，0代码成功，其他失败
	*/
	int enableCamera(const char *cameraid, bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	///*
	//* 更新摄像头是否上行, 只在房间内有效
	//* cameraid : 摄像头id,可为空，为空查默认摄像头
	//* enableVideoOut ：是否自动上行
	//* callback ：回调
	//* 返回值 : 操作错误码，0代码成功，其他失败
	//*/
	//int updateCameraMode(const char *cameraid, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	//int switchCamera(const char *cameraid, bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

private:
	std::string getOperaDevice(DeviceType type, const char *cameraid = nullptr) const;
	std::string getOperaCamera(const char *cameraid = nullptr) const;
	std::string getOperaMic(const char *cameraid = nullptr) const;
	std::string getOperaSpeaker(const char *cameraid = nullptr) const;

private:
	int avsdkErrorCode(int xcast_err_code);
	std::vector<std::string> getDeviceList(DeviceType type) const;
	int getDeviceState(DeviceType type, const char *devid = nullptr) const;



};

