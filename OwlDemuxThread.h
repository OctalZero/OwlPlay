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
	// 创建对象并打开
	virtual bool Open(const char* url, IVideoCall* video_call);

	// 启动所有线程
	virtual void Start();

	// 关闭线程，清理资源
	virtual void Close();

	// 处理线程
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

