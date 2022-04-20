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

	// �����߳�
	void run() override;

	OwlDemuxThread();
	virtual ~OwlDemuxThread();
public:
	bool is_exit_ = false;
protected:
	std::mutex mutex_;
	OwlDemux* demux_ = nullptr;
	OwlVideoThread* video_thread_ = nullptr;
	OwlAudioThread* audio_thread_ = nullptr;
};

