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
	// Speaker����
protected:

	// ��ȡ�������б�, �����ͬ����ѯ���ⲿ��Ҫ������
	// ���أ��������б�UTF-8��ʽ�����ⲿ����ת�룩
	std::vector<std::string> getSpeakerList() const;

	// ��Ĭ��speaker����
	int enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// ָ��Ĭ�ϵ�speaker Ϊ sid
	// ͬʱ����enable����
	int enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});
	
	// �л�������� , speaker : 1 ������ 0 ����
	int changeOutputMode(bool speaker);

	
protected:
	// mic����
	// ��ȡ��˷��б�, �����ͬ����ѯ���ⲿ��Ҫ������
	// ���أ���˷��б�UTF-8��ʽ�����ⲿ����ת�룩
	std::vector<std::string> getMicList() const;

	// �ڷ�����ʱ����ȡ�ɼ�mic״̬��
	// �ڷ�����ʱ����ȡdefault mic״̬;
	int getMicState();

	// ��Ĭ��mic����
	int enableMic(bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// ��Ĭ��mic����
	int enableMic(const char *micid, bool enable, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	// �л�mic
	int switchToMic(const char *micid,  std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});



	int enableLoopBack(bool enable);

	// ����ͷ����
protected:
	// ��ȡ����ͷ�б�, �����ͬ����ѯ���ⲿ��Ҫ����
	// ���أ�����ͷ�б�UTF-8��ʽ�����ⲿ����ת�룩
	std::vector<std::string> getCameraList() const;

	// cameraid : ����ͷid,����Ϊ�գ�Ϊ�����Ĭ������ͷ��״̬
	// ����ֵ ��
	// 0 : δ�ҵ��豸
	// 1 : ֹͣ
	// 2 ��������
	int getCameraState(const char *cameraid = nullptr) const;

	// ���¼����ӿڿ�������ͷԤ���Լ����п��ƣ�
	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
	
	/*
	* preview : ʹ��Ĭ����Ⱦ����Ԥ�� true��Ԥ�� false:��Ԥ��
	* enableVideoOut ���Ƿ��Զ����У�������ʱ������������
	* callback ���ص�
	* ����ֵ : ���������룬0����ɹ�������ʧ��
	*/
	int enableCamera(bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	/*
	* ֻ��Ԥ����������������
	* cameraid : ����ͷid,��Ϊ�գ�Ϊ�ղ�Ĭ������ͷ
	* preview : ʹ��Ĭ����Ⱦ����Ԥ�� true��Ԥ�� false:��Ԥ��
	* callback ���ص�
	* ����ֵ : ���������룬0����ɹ�������ʧ��
	*/
	int enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	/*
	* ����Ԥ���Լ����У������������Ԥ��������ͷ
	* cameraid : ����ͷid,��Ϊ�գ�Ϊ�ղ�Ĭ������ͷ
	* preview : ʹ��Ĭ����Ⱦ����Ԥ�� true��Ԥ�� false:��Ԥ��
	* callback ���ص�
	* ����ֵ : ���������룬0����ɹ�������ʧ��
	*/
	int enableCamera(const char *cameraid, bool preview, bool enableVideoOut, std::function<void(int32_t, char *)> callback = [](int32_t, char *) {});

	///*
	//* ��������ͷ�Ƿ�����, ֻ�ڷ�������Ч
	//* cameraid : ����ͷid,��Ϊ�գ�Ϊ�ղ�Ĭ������ͷ
	//* enableVideoOut ���Ƿ��Զ�����
	//* callback ���ص�
	//* ����ֵ : ���������룬0����ɹ�������ʧ��
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

