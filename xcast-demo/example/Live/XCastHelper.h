
#ifndef XCASTHELPER_H_
#define XCASTHELPER_H_

#include <mutex>
#include <vector>
#include <map>
#include "xcast.hh"
#include "XCastData.h"

#define kForVipKidTest 1

typedef std::function<void(int32_t, const char *)> XCHCallBack;
#define XCHNilCallBack NULL
//#define XCHNilCallBack [](int32_t, const char *) {}

class XCastHelper
{
private:
	static XCastHelper					*m_instance;			// 单例 

private:
	std::unique_ptr<XCastStartParam>	m_startup_param;		// 初始化参数
	std::shared_ptr<XCastGlobalHandler> m_global_handler;		// 全局回调监听
	std::shared_ptr<XCastRoomHandler>   m_room_handler;			// 房间内监听
	std::unique_ptr<XCastStreamParam>	m_stream_param;			// 进房参数

	std::recursive_mutex				m_func_mutex;			// 锁

private:
	std::map<std::string, std::shared_ptr<XCastVideoFrame>>  video_frame_map;

private:
	bool is_startup_succ = false;								// 是否初始化成功

	typedef enum Room_State {
		Room_Closed,											// 未进房
		Room_Connecting,										// 进房中
		Room_Connectted											// 已进房
	}Room_State;

	Room_State stream_state = Room_Closed;						// 房间状态	

	typedef enum DeviceType
	{
		Device_Camera,											// 摄像头
		Device_Mic,												// 麦克风
		Device_Speaker,											// 扬声器
	}DeviceType;

private:
	// XCAST回调
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
	/*
	* 功能: 初始始化XCast
	* param : 初始化参数，内部会保存该值；
	* callback ：初始化回调
	*/
	int startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback);

	/*
	* 功能: 反初始始化XCast，
	* callback ：反初始化回调
	*/
	int stopContext(XCHCallBack callback);

public:
	/*
	* 功能：设直全局监听，主要处理设备，以及系统相关的回调
	*/
	void setGlobalHandler(std::shared_ptr<XCastGlobalHandler>  handler);

public:
	/*
	* 功能：进房开始推流
	* roomoption : 进房参数配置
	* roomDelegate : 房间数据回调
	* callback ：调enterroom操作回调
	*/
	int enterRoom(std::unique_ptr<XCastStreamParam>roomoption, std::shared_ptr<XCastRoomHandler>	roomDelegate, XCHCallBack callback);

	/*
	* 功能：退出房间
	* callback ：调exitRoom操作回调
	*/

	int exitRoom(XCHCallBack callback);


public:
	// Speaker操作
	/*
	* 功能：获取扬声器列表, 同步查询，外部不要保存结果
	* 返回：扬声器列表（UTF-8格式串，外部进行转码）
	*/
	void getSpeakerList(std::vector<std::string> &vec) const;

	/*
	* 功能：对默认的扬声器进行打开/关闭操作
	* enable : true : 打开 / false : 关闭
	* callback ：操作回调
	* 返回：操作是否成功，0成功，非0失败
	*/
	int enableSpeaker(bool enable, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：对sid扬声器进行打开/关闭操作
	* sid : 要操作的扬声器，可为空，为空时操作默认扬声器
	* enable : true : 打开 / false : 关闭
	* callback ：操作回调
	* 返回：操作是否成功，0成功，非0失败
	*/
	int enableSpeakerByID(bool enable, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能 ：切换扬声器输出类型
	* sid ：要操作的扬声器，可为空，为空时操作默认扬声器
	* headphone : false : 外放 / true : 耳机
	*/
	int changeOutputMode(const char *sid, bool headphone);


	/*
	* 功能 ：获取扬声器音量
	* sid ：扬声器ID.可以为空，为空获取默认扬声器设备
	* 返回值 ：为负，未找到设备，其他为正常值[0,100]
	*/
	int speakerVolume(const char *sid = nullptr) const;

	/*
	* 功能 ：设置扬声器音量
	* sid ：扬声器ID.可以为空，为空获取默认扬声器设备
	* volume ： 音量值正常值[0,100] , > 100 时，设置100，<0时设置0
	* 返回值 ：为负，未找到设备，为0设置成功
	*/
	int setSpeakerVolume(int volume, const char *sid = nullptr);

public:
	// mic操作

	/*
	* 功能：获取麦克风列表, 同步查询，外部不要保存结果
	* 返回：麦克风列表（UTF-8格式串，外部进行转码）
	*/
	void getMicList(std::vector<std::string> &vec) const;

	/*
	* 功能：获取麦克风状态
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* 返回值 ：
	* 0 : 未找到设备
	* 1 : 停止
	* 2 ：运行中
	*/
	int getMicState(const char *micid = nullptr) const;

	/*
	* 功能：操作麦克风，控制上行，只能在房间内操作
	* enableAudioOut : 是否上行音频, true ：上行 / false : 不上行
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	int enableMic(bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作麦克风，控制预览(自己说话自己可以听到)
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	int enableMicPreview(bool preview, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作麦克风，控制预览(自己说话自己可以听到)以及上行
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* enableAudioOut : 是否上行音频, true ：上行 / false : 不上行
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	int enableMic(bool preview, bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);


public:
	// 摄像头操作

	/*
	* 功能：获取摄像头列表, 结果是同步查询，外部不要保存
	* 返回：摄像头列表（UTF-8格式串，外部进行转码）
	*/
	void getCameraList(std::vector<std::string> &vec) const;

	/*
	* 功能：获取摄像头状态
	* micid :  摄像头id,可以为空，为空则查默认摄像头的状态
	* 返回值 ：
	* 0 : 未找到设备
	* 1 : 停止
	* 2 ：运行中
	*/
	int getCameraState(const char *cameraid = nullptr) const;

	// 以下几个接口控制摄像头预览以及上行控制；
	// 在房间内时：打开摄像头，并预览，同时上行；
	// 在房间外时：打开摄像头，并预览，并设置成默认摄像头；

	/*
	* 功能：操作摄像头，控制上行，只能在房间内操作
	* enableVideoOut ：是否自动上行，房间外时不处理，房间内
	* cameraid : 摄像头id，可为空，为空处理默认摄像头（如果有的话）
	* callback ：回调
	* 返回值 : 操作错误码，0代码成功，其他失败
	*/
	int enableCamera(bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作摄像头，控制预览
	* micid :  摄像头id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	int enableCameraPreview(bool preview, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作摄像头，控制预览(自己说话自己可以听到)以及上行
	* micid :  摄像头id,可以为空，为空则查默认摄像头
	* preview : 是否,预览, true : 预览 / false : 不预览
	* enableVideoOut : 是否上行音频, true ：上行 / false : 不上行
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	int enableCamera(bool preview, bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);
private:
	void getOperaDevice(DeviceType type, std::string &retstr, const char *cameraid = nullptr) const;
	void getOperaCamera(std::string &retstr, const char *cameraid = nullptr) const;
	void getOperaMic(std::string &retstr, const char *cameraid = nullptr) const;
	void getOperaSpeaker(std::string &retstr, const char *cameraid = nullptr) const;

private:
	int avsdkErrorCode(int xcast_err_code) const;
	void getDeviceList(DeviceType type, std::vector<std::string> &list) const;
	int getDeviceState(DeviceType type, const char *devid = nullptr) const;

private:
	// 功能 ：清理内部状态
	void clearAfterExitRoom();

private:
	int operaMic(const char *micid, bool preview, bool needExePreview, bool audioout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback = XCHNilCallBack);
	int operaCamera(const char *camid, bool preview, bool needExePreview, bool videoout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback = XCHNilCallBack);


private:
	// frame map operation
	const std::shared_ptr<XCastVideoFrame> getVideoFrameBuffer(uint64_t tinyid, XCastMediaSource source);
	void earseVideoFrameBuffer(uint64_t tinyid, XCastMediaSource source);
	XCastMediaSource getVideoSourceType(XCastDeviceType type) const;

};

#endif