#pragma once
#include <QThread>
#include <mutex>
#include "IVideoCall.h"
class OwlDemux;
class OwlVideoThread;
class OwlAudioThread;

class OwlDemuxThread : public QThread
{
public:
	// �������󲢴�
	virtual bool Open(const char* url, IVideoCall* video_call);

	// ���������߳�
	virtual void Start();

	// �ر��̣߳�������Դ
	virtual void Close();

	// ������
	virtual void Clear();

	// ������ͣ
	void SetPause(bool is_pause);

	// ������Ƶ����
	virtual void Seek(double pos);

	// �����߳�
	void run() override;

	OwlDemuxThread();
	virtual ~OwlDemuxThread();
public:
	bool is_exit_ = false;  // �Ƿ��˳�
	long long pts_ = 0;  // ��ǰ���ŵ���pts
	long long total_ms_ = 0;  // ��Ƶ��ʱ����������ʹ��
	bool is_pause_ = false;  // �Ƿ���ͣ
protected:
	std::mutex mutex_;
	OwlDemux* demux_ = nullptr;
	OwlVideoThread* video_thread_ = nullptr;
	OwlAudioThread* audio_thread_ = nullptr;
};

