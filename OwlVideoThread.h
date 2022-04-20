#pragma once
#include "IVideoCall.h"
#include "OwlDecodeThread.h"
struct AVCodecParameters;

// �������ʾ��Ƶ����
class OwlVideoThread : public OwlDecodeThread
{
public:
	// �򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height);

	// �ӻ������ȡ��packet
	void run() override;

	OwlVideoThread();
	virtual ~OwlVideoThread();
public:
	// ͬ��ʱ�䣬���ⲿ����
	long long syn_pts_ = 0;
protected:
	std::mutex video_mutex_;
	IVideoCall* video_call_ = nullptr;  // ��ʾ��Ƶͼ��Ľӿ�
};

