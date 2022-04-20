#pragma once
#include <QThread>
#include <mutex>
#include "OwlDecodeThread.h"
class OwlAudioPlay;
class OwlResample;
struct AVCodecParameters;
// ������Ƶ�Ŀ�����
class OwlAudioThread : public OwlDecodeThread
{
public:
	// �򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para, int sample_rate, int channels);

	// ֹͣ�̣߳�������Դ
	virtual void Close() override;

	// �ӻ������ȡ��packet
	void run() override;

	OwlAudioThread();
	virtual ~OwlAudioThread();
public:
	// ��ǰ��Ƶ���ŵ�pts
	long long pts_ = 0;
protected:
	std::mutex audio_mutex_;
	OwlAudioPlay* audio_play_ = nullptr;  // ��Ƶ����
	OwlResample* resample_ = nullptr;  // �ز��� 
};

