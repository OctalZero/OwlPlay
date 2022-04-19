#pragma once
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
struct AVPacket;
struct AVCodecParameters;
class OwlDecode;

// �������ʾ��Ƶ����
class OwlVideoThread : public QThread
{
public:
	// �򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height);

	// ����packet���������
	virtual void Push(AVPacket* pkt);

	// �ӻ������ȡ��packet
	void run() override;

	OwlVideoThread();
	virtual ~OwlVideoThread();
public:
	// �����У�����Լ2s
	int maxList_ = 100;
	// �ж��߳��Ƿ��˳�
	bool is_exit_ = false;
protected:
	std::mutex mutex_;
	// ������������ģʽ���ɵ���������packet�ӵ������������У������߳�������packet��
	std::list<AVPacket*> packets_;
	OwlDecode* decode_ = nullptr;  // ������
	IVideoCall* video_call_ = nullptr;  // ��ʾ��Ƶͼ��Ľӿ�
};

