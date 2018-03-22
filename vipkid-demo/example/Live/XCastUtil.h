#ifndef XCASTUTIL_H_
#define XCASTUTIL_H_

#include <mutex>
#include <string>

#include "XCastData.h"

class XCastUtil
{
public:
	/*
	* �鿴�汾��
	*/
	static std::string version();

	/*
	* ���ܣ������ʺ��߼���tinyidתidentifier�߼�����������startContextǰ����
	*
	* handler ����Ϊ��
	* ����ֵ ��true �ɹ���falseʧ��
	*/
	static bool setAccountHandler(std::shared_ptr<XCastAccountHandler> handler);

	/*
	* ����: ��ʼʼ��XCast
	* param : ��ʼ���������ڲ��ᱣ���ֵ��
	* callback ����ʼ���ص�
	*/
	static int startContext(std::unique_ptr<XCastStartParam> param, XCHCallBack callback);

	/*
	* ����: ����ʼʼ��XCast��
	* callback ������ʼ���ص�
	*/
	static int stopContext(XCHCallBack callback);

public:
	/*
	* ���ܣ���ֱȫ�ּ�������Ҫ�����豸���Լ�ϵͳ��صĻص�
	*/
	static void setGlobalHandler(std::shared_ptr<XCastGlobalHandler>  handler);

public:
	/*
	* ���ܣ�������ʼ����
	* roomoption : ������������
	* roomDelegate : �������ݻص�
	* callback ����enterroom�����ص�
	*/
	static int enterRoom(std::unique_ptr<XCastStreamParam>roomoption, std::shared_ptr<XCastRoomHandler>	roomDelegate, XCHCallBack callback);

	/*
	* ���ܣ��˳�����
	* callback ����exitRoom�����ص�
	*/

	static int exitRoom(XCHCallBack callback);


public:
	// Speaker����
	/*
	* ���ܣ���ȡ�������б�, ͬ����ѯ���ⲿ��Ҫ������
	* ���أ��������б�UTF-8��ʽ�����ⲿ����ת�룩
	*/
	static std::vector<std::string> getSpeakerList();

	/*
	* ���ܣ���sid���������д�/�رղ���
	* sid : Ҫ����������������Ϊ�գ�Ϊ��ʱ����Ĭ��������
	* enable : true : �� / false : �ر�
	* callback �������ص�
	* ���أ������Ƿ�ɹ���0�ɹ�����0ʧ��
	*/
	static int enableSpeaker(bool enable, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);


	/*
	* ���ܣ�����������������Ԥ��
	* micid :  ������id,����Ϊ�գ�Ϊ�����Ĭ����������״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableSpeakerPreview(bool preview, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�����������������Ԥ���Լ�����
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* enableAudioOut : �Ƿ�������Ƶ, true ������ / false : ������
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableSpeaker(bool preview, bool enable, const char *sid = nullptr, XCHCallBack callback = XCHNilCallBack);


	/*
	* ���� ���л��������������
	* sid ��Ҫ����������������Ϊ�գ�Ϊ��ʱ����Ĭ��������
	* headphone : false : ��� / true : ����
	*/
	static int changeOutputMode(bool headphone, const char *sid);


	/*
	* ���� ����ȡ����������
	* sid ��������ID.����Ϊ�գ�Ϊ�ջ�ȡĬ���������豸
	* ����ֵ ��Ϊ����δ�ҵ��豸������Ϊ����ֵ[0,100]
	*/
	static int speakerVolume(const char *sid = nullptr);

	/*
	* ���� ����������������
	* sid ��������ID.����Ϊ�գ�Ϊ�ջ�ȡĬ���������豸
	* volume �� ����ֵ����ֵ[0,100] , > 100 ʱ������100��<0ʱ����0
	* ����ֵ ��Ϊ����δ�ҵ��豸��Ϊ0���óɹ�
	*/
	static int setSpeakerVolume(int volume, const char *sid = nullptr);

public:
	// mic����

	/*
	* ���ܣ���ȡ��˷��б�, ͬ����ѯ���ⲿ��Ҫ������
	* ���أ���˷��б�UTF-8��ʽ�����ⲿ����ת�룩
	*/
	static std::vector<std::string> getMicList();

	/*
	* ���ܣ���ȡ��˷�״̬
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* ����ֵ ��
	* 0 : δ�ҵ��豸
	* 1 : ֹͣ
	* 2 ��������
	*/
	static XCastDeviceState getMicState(const char *micid = nullptr);

	/*
	* ���ܣ�������˷磬�������У�ֻ���ڷ����ڲ���
	* enableAudioOut : �Ƿ�������Ƶ, true ������ / false : ������
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableMic(bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�������˷磬����Ԥ��(�Լ�˵���Լ���������)
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableMicPreview(bool preview, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�������˷磬����Ԥ��(�Լ�˵���Լ���������)�Լ�����
	* micid :  ��˷�id,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* enableAudioOut : �Ƿ�������Ƶ, true ������ / false : ������
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableMic(bool preview, bool enableAudioOut, const char *micid = nullptr, XCHCallBack callback = XCHNilCallBack);


public:
	// ����ͷ����

	/*
	* ���ܣ���ȡ����ͷ�б�, �����ͬ����ѯ���ⲿ��Ҫ����
	* ���أ�����ͷ�б�UTF-8��ʽ�����ⲿ����ת�룩
	*/
	static std::vector<std::string> getCameraList();

	/*
	* ���ܣ���ȡ����ͷ״̬
	* micid :  ����ͷid,����Ϊ�գ�Ϊ�����Ĭ������ͷ��״̬
	* ����ֵ ��
	* 0 : δ�ҵ��豸
	* 1 : ֹͣ
	* 2 ��������
	*/
	static XCastDeviceState getCameraState(const char *cameraid = nullptr);

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
	static int enableCamera(bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ���������ͷ������Ԥ��
	* micid :  ����ͷid,����Ϊ�գ�Ϊ�����Ĭ����˷��״̬
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableCameraPreview(bool preview, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ���������ͷ������Ԥ��(�Լ�˵���Լ���������)�Լ�����
	* micid :  ����ͷid,����Ϊ�գ�Ϊ�����Ĭ������ͷ
	* preview : �Ƿ�,Ԥ��, true : Ԥ�� / false : ��Ԥ��
	* enableVideoOut : �Ƿ�������Ƶ, true ������ / false : ������
	* callback �������ص�
	* ����ֵ ����������ֵ
	*/
	static int enableCamera(bool preview, bool enableVideoOut, const char *cameraid = nullptr, XCHCallBack callback = XCHNilCallBack);

public:
	// �����������û�����
	/*
	* ���ܣ������Ӧ�Ļ���
	* ������
	* item �� �������
	* callback �������ص�
	* ����ֵ ����������ֵ, 0 ����ɹ�������ʧ��
	*/
	static void requestView(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�����requestView���ȥ����itemList�û�����
	* ������
	* itemList �� ���û������������
	* callback �������ص�����ص����
	*/
	static void requestViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ��������еĻ���
	* ������
	* callback �������ص�����ص����
	*/
	static void requestAllView(XCHReqViewListCallBack callback = XCHNilCallBack);


	/*
	* ���ܣ�ȡ�������Ӧ�Ļ���
	* ������
	* item �� �������
	* callback �������ص�
	* ����ֵ ����������ֵ, 0 ����ɹ�������ʧ��
	*/
	static void cancelView(XCastRequestViewItem item, XCHReqViewListCallBack callback = XCHNilCallBack);

	/*
	* ���ܣ�����cancelView���ȥȡ������itemList�û�����
	* ������
	* itemList �� ���û������������
	* callback �������ص�����ص����
	*/
	static void cancelViewList(std::vector<XCastRequestViewItem> itemList, XCHReqViewListCallBack callback = XCHNilCallBack);



	/*
	* ���ܣ�ȡ�����е�Զ�̻���
	* ������
	* item �� �������
	* callback �������ص�
	* ����ֵ ����������ֵ, 0 ����ɹ�������ʧ��
	*/
	static void cancelAllView(XCHReqViewListCallBack callback = XCHNilCallBack);


	/*
	* ���ܣ���ȡԶ��track��Ƶ������
	* ������
	* tinyid �� �û�tinyid
	* ����ֵ ��[0,100];
	*/
	static int getSpeakerDynamicVolume(uint64_t tinyid);

};
#endif

