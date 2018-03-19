
#ifndef XCASTHELPER_H_
#define XCASTHELPER_H_

#include <mutex>
#include <vector>
#include <map>
#include "xcast.hh"
#include "XCastData.h"

#define kForVipKidTest 1

class XCastHelper
{
private:
	static XCastHelper					*m_instance;			// ���� 

private:
	std::unique_ptr<XCastStartParam>	m_startup_param;		// ��ʼ������
	std::shared_ptr<XCastAccountHandler>m_account_handler;		// �ʺ��߼� 
	std::shared_ptr<XCastGlobalHandler> m_global_handler;		// ȫ�ֻص�����
	std::shared_ptr<XCastRoomHandler>   m_room_handler;			// �����ڼ���
	std::unique_ptr<XCastStreamParam>	m_stream_param;			// ��������

	std::recursive_mutex				m_func_mutex;			// ��

private:
	std::map<std::string, std::shared_ptr<XCastVideoFrame>>		video_frame_map;
#ifdef kSupportIMAccount
	std::map<uint64_t, std::string>								tinyid_cache;
#endif
	std::map<uint64_t, std::shared_ptr<XCastEndpoint>>			m_endpoint_map;

	std::map<uint64_t, std::string>								m_account_cache;

private:
	bool is_startup_succ = false;								// �Ƿ��ʼ���ɹ�

	typedef enum Room_State {
		Room_Closed,											// δ����
		Room_Connecting,										// ������
		Room_Connectted											// �ѽ���
	}Room_State;

	Room_State stream_state = Room_Closed;						// ����״̬	

	//typedef enum DeviceType
	//{
	//	Device_Camera,											// ����ͷ
	//	Device_Mic,												// ��˷�
	//	Device_Speaker,											// ������
	//}DeviceType;

private:
	// XCAST�ص�
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


#if kForVipKidTest
private:
	FILE  *logFile = nullptr;
public:
	void logtoFile(const char *tag, const char * info);
#endif



public:
	/*
	* ���ܣ������ʺ��߼���tinyidתidentifier�߼�����������startContextǰ����
	*
	* handler ����Ϊ��
	* ����ֵ ��true �ɹ���falseʧ��
	*/
	bool setAccountHandler(std::shared_ptr<XCastAccountHandler> handler);

public:
	/*
	* ����: ��ʼʼ��XCast
	* param : ��ʼ���������ڲ��ᱣ���ֵ��
	* callback ����ʼ���ص�
	*/
	int startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback);

	/*
	* ����: ����ʼʼ��XCast��
	* callback ������ʼ���ص�
	*/
	int stopContext(XCHCallBack callback);

public:
	/*
	* ���ܣ���ֱȫ�ּ�������Ҫ�����豸���Լ�ϵͳ��صĻص�
	*/
	void setGlobalHandler(std::shared_ptr<XCastGlobalHandler>  handler);

public:
	/*
	* ���ܣ�������ʼ����
	* roomoption : ������������
	* roomDelegate : �������ݻص�
	* callback ����enterroom�����ص�
	*/
	int enterRoom(std::unique_ptr<XCastStreamParam>roomoption, std::shared_ptr<XCastRoomHandler> roomDelegate, XCHCallBack callback);

	/*
	* ���ܣ��˳�����
	* callback ����exitRoom�����ص�
	*/

	int exitRoom(XCHCallBack callback);


public:
	// Speaker����
	/*
	* ���ܣ���ȡ�������б�, ͬ����ѯ���ⲿ��Ҫ������
	* ���أ��������б�UTF-8��ʽ�����ⲿ����ת�룩
	*/
	std::vector<std::string> getSpeakerList() const;

	/*
	* ���ܣ���sid���������д�/�رղ���
	* sid : Ҫ����������������Ϊ�գ�Ϊ��ʱ����Ĭ��������
	* enable : true : �� / false : �ر�
	* callback �������ص�
	* ���أ������Ƿ�ɹ���0�ɹ�����0ʧ��
	*/
	int enableSpeaker(bool enable, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���� ���л��������������
	* sid ��Ҫ����������������Ϊ�գ�Ϊ��ʱ����Ĭ��������
	* headphone : false : ��� / true : ����
	*/
	int changeOutputMode(bool headphone, const char *sid = nullptr);


	/*
	* ���� ����ȡ����������
	* sid ��������ID.����Ϊ�գ�Ϊ�ջ�ȡĬ���������豸
	* ����ֵ ��Ϊ����δ�ҵ��豸������Ϊ����ֵ[0,100]
	*/
	int speakerVolume(const char *sid = nullptr) const;

	/*
	* ���� ����������������
	* sid ��������ID.����Ϊ�գ�Ϊ�ջ�ȡĬ���������豸
	* volume �� ����ֵ����ֵ[0,100] , > 100 ʱ������100��<0ʱ����0
	* ����ֵ ��Ϊ����δ�ҵ��豸��Ϊ0���óɹ�
	*/
	int setSpeakerVolume(int volume, const char *sid = nullptr);

public:
	// mic����

	/*
	* ���ܣ���ȡ��˷��б�, ͬ����ѯ���ⲿ��Ҫ������
	* ���أ���˷��б�UTF-8��ʽ�����ⲿ����ת�룩
	*/
	std::vector<std::string> getMicList() const;

	/*
	* ���ܣ���ȡ��˷�״̬
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* ����ֵ ��
	* 0 : δ�ҵ��豸
	* 1 : ֹͣ
	* 2 ��������
	*/
	XCastDeviceState getMicState(const char *micid = nullptr) const;

	/*
	* ���ܣ�������˷磬�������У�ֻ���ڷ����ڲ���
	* enableAudioOut : �Ƿ�������Ƶ, true ������ / false : ������
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	int enableMic(bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�������˷磬����Ԥ��(�Լ�˵���Լ���������)
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	int enableMicPreview(bool preview, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�������˷磬����Ԥ��(�Լ�˵���Լ���������)�Լ�����
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* enableAudioOut : �Ƿ�������Ƶ, true ������ / false : ������
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	int enableMic(bool preview, bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);


public:
	// ����ͷ����

	/*
	* ���ܣ���ȡ����ͷ�б�, �����ͬ����ѯ���ⲿ��Ҫ����
	* ���أ�����ͷ�б�UTF-8��ʽ�����ⲿ����ת�룩
	*/
	std::vector<std::string> getCameraList() const;

	/*
	* ���ܣ���ȡ����ͷ״̬
	* micid :  ����ͷid,����Ϊ�գ�Ϊ�����Ĭ������ͷ��״̬
	* ����ֵ ��
	* 0 : δ�ҵ��豸
	* 1 : ֹͣ
	* 2 ��������
	*/
	XCastDeviceState getCameraState(const char *cameraid = nullptr) const;

	// ���¼����ӿڿ�������ͷԤ���Լ����п��ƣ�
	// �ڷ�����ʱ��������ͷ����Ԥ����ͬʱ���У�
	// �ڷ�����ʱ��������ͷ����Ԥ���������ó�Ĭ������ͷ��

	/*
	* ���ܣ���������ͷ���������У�ֻ���ڷ����ڲ���
	* enableVideoOut ���Ƿ��Զ����У�������ʱ������������
	* cameraid : ����ͷid����Ϊ�գ�Ϊ�մ���Ĭ������ͷ������еĻ���
	* callback ���ص�
	* ����ֵ : ���������룬0����ɹ�������ʧ��
	*/
	int enableCamera(bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ���������ͷ������Ԥ��
	* micid :  ����ͷid,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	int enableCameraPreview(bool preview, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ���������ͷ������Ԥ��(�Լ�˵���Լ���������)�Լ�����
	* micid :  ����ͷid,����Ϊ�գ�Ϊ�����Ĭ������ͷ
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* enableVideoOut : �Ƿ�������Ƶ, true ������ / false : ������
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	int enableCamera(bool preview, bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

public:
	// �����������û�����
	/*
	* ���ܣ������Ӧ�Ļ���
	* ������
	* item �� �������
	* callback �������ص�
	* ����ֵ ����������ֵ, 0 ����ɹ�������ʧ��
	*/
	void requestView(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�����requestView���ȥ����itemList�û�����
	* ������
	* itemList �� ���û������������
	* callback �������ص�����ص����
	*/
	void requestViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ��������еĻ���
	* ������
	* callback �������ص�����ص����
	*/
	void requestAllView(XCHReqViewListCallBack callback = XCHNilCallBack);


	/*
	* ���ܣ�ȡ�������Ӧ�Ļ���
	* ������
	* item �� �������
	* callback �������ص�
	* ����ֵ ����������ֵ, 0 ����ɹ�������ʧ��
	*/
	void cancelView(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�����cancelView���ȥȡ������itemList�û�����
	* ������
	* itemList �� ���û������������
	* callback �������ص�����ص����
	*/
	void cancelViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback = XCHNilCallBack);



	/*
	* ���ܣ�ȡ�����е�Զ�̻���
	* ������
	* item �� �������
	* callback �������ص�
	* ����ֵ ����������ֵ, 0 ����ɹ�������ʧ��
	*/
	void cancelAllView(XCHReqViewListCallBack callback = XCHNilCallBack);

	// Զ����Ƶ
	//void setAudioWhiteList()
	//void cancelAudioWhiteList();


	/*
	* ���ܣ���ȡԶ��track��Ƶ������
	* ������
	* tinyid �� �û�tinyid
	* ����ֵ ��[0,100];
	*/
	int getSpeakerDynamicVolume(uint64_t tinyid) const;

private:
	int avsdkErrorCode(int xcast_err_code) const;

private:
	// ��ȡҪ�������豸�����ͨ��devidû�ҵ�����ȡĬ���豸
	std::string getOperaDevice(XCastDeviceType type, const char *devid = nullptr) const;
	std::string getOperaCamera(const char *cameraid = nullptr) const;
	std::string getOperaMic(const char *micid = nullptr) const;
	std::string getOperaSpeaker(const char *speakerid = nullptr) const;

	// ��ȡҪ�������豸
	std::vector<std::string> getDeviceList(XCastDeviceType type) const;
	XCastDeviceState getDeviceState(XCastDeviceType type, const char *devid = nullptr) const;

private:
	// ���� �������ڲ�״̬
	void clearAfterExitRoom();

private:
	int operaMic(const char *micid, bool preview, bool needExePreview, bool audioout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback = XCHNilCallBack);
	int operaCamera(const char *camid, bool preview, bool needExePreview, bool videoout, bool needExeAudioOut, bool needSetDefault, XCHCallBack callback = XCHNilCallBack);


private:
	// frame map operation
	const std::shared_ptr<XCastVideoFrame> getVideoFrameBuffer(uint64_t tinyid, XCastMediaSource source);
	void earseVideoFrameBuffer(uint64_t tinyid, XCastMediaSource source);
private:
	XCastMediaSource getDeviceVideoSourceType(XCastDeviceType type) const;

private:
	std::shared_ptr<XCastEndpoint> getEndpoint(uint64_t tinyid);
	void updateEndpointMap(uint64_t tinyid);
	void deleteEndpoint(uint64_t tinyid);

	XCastRequestViewItem getFromTrackID(std::string trackid) const;
	void remoteView(XCastRequestViewItem item, bool enable, XCHReqViewListCallBack callback = XCHNilCallBack);
	void remoteViewWithTinyid(XCastRequestViewItem item, bool enable, XCHReqViewListCallBack callback = XCHNilCallBack);
	void remoteAllView(bool enable, XCHReqViewListCallBack callback = XCHNilCallBack);
	int getSpeakerDynamicVolume(std::string trackid) const;

private:
	inline bool isSupportIMAccount() const;

private:
	int startContextWithout(std::unique_ptr<XCastStartParam> param, XCHCallBack callback);

private:
	std::string syncGetUserid(uint64_t tinyid) const;

	void getUserIDWithTinyid(uint64_t tinyid, std::function<void(std::string, int, std::string)> callback);
	void getUserIDWithTinyidFromIMSDK(uint64_t tinyid, std::function<void(std::string, int, std::string)> callback);
	void getUserIDWithTinyid(std::vector<uint64_t> tinyidlist, std::function<void(std::vector<std::string>, int, std::string)> callback);
	void getUserIDWithTinyidFromIMSDK(std::vector<uint64_t> tinyidlist, std::function<void(std::vector<std::string>, int, std::string)> callback);


	uint64_t syncGetTinyid(std::string userid) const;
	void getTinyIDWithUserID(std::string userid, std::function<void(uint64_t, int, std::string)> callback);
	void getTinyIDWithUserIDFromIMSDK(std::string userid, std::function<void(uint64_t, int, std::string)> callback);
	
	void getTinyIDWithUserID(std::vector<std::string> useridlist, std::function<void(std::vector<uint64_t>, int, std::string)> callback);
	void getTinyIDWithUserIDFromIMSDK(std::vector<std::string> useridlist, std::function<void(std::vector<uint64_t>, int, std::string)> callback);


private:
	void notifyTrackEndpointEvent(uint64_t uin, std::string userid, XCastEndpointEvent event, const bool has);
};

#endif