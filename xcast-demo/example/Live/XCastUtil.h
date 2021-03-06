#ifndef XCASTUTIL_H_
#define XCASTUTIL_H_

#include <mutex>
#include <string>

#include "XCastData.h"

class XCastUtil
{
public:
	/*
	* 查看版本号
	*/
	static std::string version();

	/*
	* 功能：设置帐号逻辑（tinyid转identifier逻辑），必须在startContext前设置
	*
	* handler ：可为空
	* 返回值 ：true 成功，false失败
	*/
	static bool setAccountHandler(std::shared_ptr<XCastAccountHandler> handler);

	/*
	* 功能: 初始始化XCast
	* param : 初始化参数，内部会保存该值；
	* callback ：初始化回调
	*/
	static int startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback);

	/*
	* 功能: 反初始始化XCast，
	* callback ：反初始化回调
	*/
	static int stopContext(XCHCallBack callback);

public:
	/*
	* 功能：设直全局监听，主要处理设备，以及系统相关的回调
	*/
	static void setGlobalHandler(std::shared_ptr<XCastGlobalHandler>  handler);

public:
	/*
	* 功能：进房开始推流
	* roomoption : 进房参数配置
	* roomDelegate : 房间数据回调
	* callback ：调enterroom操作回调
	*/
	static int enterRoom(std::unique_ptr<XCastStreamParam>roomoption, std::shared_ptr<XCastRoomHandler>	roomDelegate, XCHCallBack callback);

	/*
	* 功能：退出房间
	* callback ：调exitRoom操作回调
	*/

	static int exitRoom(XCHCallBack callback);


public:
	// Speaker操作
	/*
	* 功能：获取扬声器列表, 同步查询，外部不要保存结果
	* 返回：扬声器列表（UTF-8格式串，外部进行转码）
	*/
	static std::vector<std::string> getSpeakerList();

	/*
	* 功能：设置默认扬声器
	* sid ：为空或为错误的，默认扬声明器不会变更，业务上层保证传入值的合法性
	* 返回值 : 0 成功，非0失败
	*/
	static int setDefaultSpeaker(const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：获取默认扬声器
	* 返回：默认扬声器（UTF-8格式串），为空即没有扬声器设备
	*/
	static std::string getDefaultSpeaker();


	/*
	* 功能：对sid扬声器进行打开/关闭操作
	* sid : 要操作的扬声器，可为空，为空时操作默认扬声器
	* enable : true : 打开 / false : 关闭
	* callback ：操作回调
	* 返回：操作是否成功，0成功，非0失败
	*/
	static int enableSpeaker(bool enable, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);


	/*
	* 功能：操作扬声器，控制预览
	* micid :  扬声器id,可以为空，为空则查默认扬声器的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableSpeakerPreview(bool preview, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作扬声器，控制预览以及开关
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* enableAudioOut : 是否上行音频, true ：上行 / false : 不上行
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableSpeaker(bool preview, bool enable, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);
	

	/*
	* 功能 ：切换扬声器输出类型
	* sid ：要操作的扬声器，可为空，为空时操作默认扬声器
	* headphone : false : 外放 / true : 耳机
	*/
	static int changeOutputMode(bool headphone, const char *sid);


	/*
	* 功能 ：获取扬声器音量
	* sid ：扬声器ID.可以为空，为空获取默认扬声器设备
	* 返回值 ：为负，未找到设备，其他为正常值[0,100]
	*/
	static int speakerVolume(const char *sid = nullptr);

	/*
	* 功能 ：设置扬声器音量
	* sid ：扬声器ID.可以为空，为空获取默认扬声器设备
	* volume ： 音量值正常值[0,100] , > 100 时，设置100，<0时设置0
	* 返回值 ：为负，未找到设备，为0设置成功
	*/
	static int setSpeakerVolume(int volume, const char *sid = nullptr);

public:
	// mic操作

	/*
	* 功能：获取麦克风列表, 同步查询，外部不要保存结果
	* 返回：麦克风列表（UTF-8格式串，外部进行转码）
	*/
	static std::vector<std::string> getMicList();

	/*
	* 功能：设置默认麦克风
	* sid ：为空或为错误的，默认麦克风不会变更，业务上层保证传入值的合法性
	* 返回值 : 0 成功，非0失败
	*/
	static int setDefaultMic(const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：获取默认麦克风
	* 返回：默认麦克风（UTF-8格式串），为空即没有麦克风设备
	*/
	static std::string getDefaultMic();

	/*
	* 功能：获取麦克风状态
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* 返回值 ：
	* 0 : 未找到设备
	* 1 : 停止
	* 2 ：运行中
	*/
	static XCastDeviceState getMicState(const char *micid = nullptr);

	/*
	* 功能：操作麦克风，控制上行，只能在房间内操作
	* enableAudioOut : 是否上行音频, true ：上行 / false : 不上行
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableMic(bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作麦克风，控制预览(自己说话自己可以听到)
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableMicPreview(bool preview, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作麦克风，控制预览(自己说话自己可以听到)以及上行
	* micid :  麦克风id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* enableAudioOut : 是否上行音频, true ：上行 / false : 不上行
	* setsDefault : 是否设置sid为默认麦克风(为空或为错误的，默认麦克风不会变更，业务上层保证传入值的合法性)，等同于切换麦克风
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableMic(bool preview, bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);
	static int switchMic(bool preview, bool enableAudioOut, bool setDefault = false, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

public:
	// 摄像头操作

	/*
	* 功能：获取摄像头列表, 结果是同步查询，外部不要保存
	* 返回：摄像头列表（UTF-8格式串，外部进行转码）
	*/
	static std::vector<std::string> getCameraList();

	/*
	* 功能：设置默认摄像头
	* sid ：为空或为错误的，默认摄像头不会变更，业务上层保证传入值的合法性
	* 返回值 : 0 成功，非0失败
	*/
	static int setDefaultCamera(const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：获取默认摄像头
	* 返回：默认摄像头（UTF-8格式串），为空即没有摄像头设备
	*/
	static std::string getDefaultCamera();

	/*
	* 功能：获取摄像头状态
	* micid :  摄像头id,可以为空，为空则查默认摄像头的状态
	* 返回值 ：
	* 0 : 未找到设备
	* 1 : 停止
	* 2 ：运行中
	*/
	static XCastDeviceState getCameraState(const char *cameraid = nullptr);

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
	static int enableCamera(bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作摄像头，控制预览
	* micid :  摄像头id,可以为空，为空则查默认麦克风的状态
	* preview : 是否,预览, true : 预览 / false : 不预览
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableCameraPreview(bool preview, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* 功能：操作摄像头，控制预览(自己说话自己可以听到)以及上行
	* micid :  摄像头id,可以为空，为空则查默认摄像头
	* preview : 是否,预览, true : 预览 / false : 不预览
	* enableVideoOut : 是否上行视频, true ：上行 / false : 不上行
	* setsDefault : 是否设置sid为默认摄像头(为空或为错误的，默认麦克风不会变更，业务上层保证传入值的合法性)，等同于切换摄像头
	* callback ：操作回调
	* 返回值 ：操作返回值
	*/
	static int enableCamera(bool preview, bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);
	static int switchCamera(bool preview, bool enableVideoOut, bool setDefault, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	
	
public:
	// 房间内上麦用户操作
	/*
	* 功能：请求对应的画面
	* 参数：
	* item ： 请求参数
	* callback ：操作回调
	* 返回值 ：操作返回值, 0 请求成功，非零失败
	*/
	static void requestView(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* 功能：调用requestView多次去请求itemList用户画面
	* 参数：
	* itemList ： 多用户参数请求参数
	* callback ：操作回调，会回调多次
	*/
	static void requestViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* 功能：请求所有的画面
	* 参数：
	* callback ：操作回调，会回调多次
	*/
	static void requestAllView(XCHReqViewListCallBack callback = XCHNilCallBack);


	/*
	* 功能：取消请求对应的画面
	* 参数：
	* item ： 请求参数
	* callback ：操作回调
	* 返回值 ：操作返回值, 0 请求成功，非零失败
	*/
	static void cancelView(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* 功能：调用cancelView多次去取消请求itemList用户画面
	* 参数：
	* itemList ： 多用户参数请求参数
	* callback ：操作回调，会回调多次
	*/
	static void cancelViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback = XCHNilCallBack);



	/*
	* 功能：取消所有的远程画面
	* 参数：
	* item ： 请求参数
	* callback ：操作回调
	* 返回值 ：操作返回值, 0 请求成功，非零失败
	*/
	static void cancelAllView(XCHReqViewListCallBack callback = XCHNilCallBack);


	/*
	* 功能：获取远程track音频流音量
	* 参数：
	* tinyid ： 用户tinyid
	* 返回值 ：[0,100];
	*/
	static int getSpeakerDynamicVolume(uint64_t tinyid);

};
#endif

