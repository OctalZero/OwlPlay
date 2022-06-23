/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ����Ͳ�����Ƶ���߳��࣬�̳��� OwlDecodeThread ��
  *				 ��Ҫ������Ƶ�����߼����൱����Ƶ��������
**********************************************************************************/
#pragma once
#include <QThread>
#include <mutex>
#include "OwlDecodeThread.h"
class OwlAudioPlay;
class OwlResample;
struct AVCodecParameters;

class OwlAudioThread : public OwlDecodeThread
{
public:
	// �򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para, int sample_rate, int channels);

	// ֹͣ�̣߳�������Դ
	virtual void Close() override;

	// �����壬��Ƶ��IO�豸Ҳ�л���Ҫ������Ҫ��д������
	virtual void Clear() override;

	// ������ͣ
	void SetPause(bool is_pause);

	// �ӻ������ȡ��packet
	void run() override;

	OwlAudioThread();
	virtual ~OwlAudioThread();
public:
	long long pts_ = 0;  // ��ǰ��Ƶ���ŵ�pts
	bool is_pause_ = false;  // �Ƿ���ͣ
protected:
	std::mutex audio_mutex_;
	OwlAudioPlay* audio_play_ = nullptr;  // ��Ƶ����
	OwlResample* resample_ = nullptr;  // �ز��� 
};

