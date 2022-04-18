#pragma once
#include <QThread>
#include <mutex>
#include <list>
class OwlDecode;
class OwlAudioPlay;
class OwlResample;
struct AVCodecParameters;
struct AVPacket;
class OwlAudioThread : public QThread
{
public:
	// �򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters* para, int sample_rate, int channels);

	// ����packet���������
	virtual void Push(AVPacket* pkt);

	// �ӻ������ȡ��packet
	void run() override;

	OwlAudioThread();
	virtual ~OwlAudioThread();
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
	OwlAudioPlay* audio_play_ = nullptr;  // ��Ƶ����
	OwlResample* resample_ = nullptr;  // �ز��� 
};

