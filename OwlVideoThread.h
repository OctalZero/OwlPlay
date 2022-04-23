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

	// ������ͣ
	void SetPause(bool is_pause);

	// �ӻ������ȡ��packet
	void run() override;

	// ����pts��������յ��Ľ�������pts >= seek_pts����return true��������ʾ����
	virtual bool ReaintPts(AVPacket* pkt, long long seek_pts);

	OwlVideoThread();
	virtual ~OwlVideoThread();
public:
	long long syn_pts_ = 0;  // ͬ��ʱ�䣬���ⲿ����
	bool is_pause_ = false;  // �Ƿ���ͣ
protected:
	std::mutex video_mutex_;
	IVideoCall* video_call_ = nullptr;  // ��ʾ��Ƶͼ��Ľӿ�
};

