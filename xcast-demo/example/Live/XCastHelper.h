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
	std::unique_ptr<XCastRoomHandler>  m_room_handler;
	std::unique_ptr<XCastStreamParam>	m_stream_param;

	std::recursive_mutex				m_func_mutex;

private:
	bool is_startup_succ = false;
	bool is_stream_succ = false;

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

	// Speaker����
protected:
	
	// ��Ĭ��speaker����
	int enableSpeaker(bool enable, std::function<void(int32_t, char *)> callback);

	// ָ��Ĭ�ϵ�speaker Ϊ sid
	// ͬʱ����enable����
	int enableSpeaker(const char *sid, bool enable, std::function<void(int32_t, char *)> callback);
	// �л�������� , speaker : 1 ������ 0 ����
	int changeOutputMode(bool speaker);

	// ��ȡ��ǰSpeaker״̬��
	int getSpeakerState();

	// mic����
protected:
	// ��Ĭ��mic����
	int enableMic(bool enable, std::function<void(int32_t, char *)> callback);

	// ��Ĭ��mic����
	int enableMic(const char*micid, bool enable, std::function<void(int32_t, char *)> callback);

	// �л�mic
	int switchToMic(const char *micid,  std::function<void(int32_t, char *)> callback);

	// �ڷ�����ʱ����ȡ�ɼ�mic״̬��
	// �ڷ�����ʱ����ȡdefault mic״̬;
	int getMicState();

	int enableLoopBack(bool enable);

	// ����ͷ����
protected:
	// �ڷ�����ʱ����ȡ�ɼ�Camera״̬��
	// �ڷ�����ʱ����ȡdefault Camera״̬;
	int getCameraState();

	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��
	int enableCamera(const char *cameraid, bool preview, std::function<void(int32_t, char *)> callback);

	int enableCamera(const char *cameraid, bool preview, bool campture, std::function<void(int32_t, char *)> callback);

	int updateCameraMode(const char *cameraid, bool autoSending, std::function<void(int32_t, char *)> callback);

	int switchCamera(const char *cameraid, bool preview, bool campture, std::function<void(int32_t, char *)> callback);

	//int setRotation(int rotate);
	// enableExternalCapture()
	//int fillExternalCaptureFrame()

private:
	int avsdkErrorCode(int xcast_err_code);

};

