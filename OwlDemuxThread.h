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

	// 清理缓冲
	virtual void Clear();

	// 设置暂停
	void SetPause(bool is_pause);

	// 控制视频进度
	virtual void Seek(double pos);

	// 处理线程
	void run() override;

	OwlDemuxThread();
	virtual ~OwlDemuxThread();
public:
	bool is_exit_ = false;  // 是否退出
	long long pts_ = 0;  // 当前播放到的pts
	long long total_ms_ = 0;  // 视频总时长，进度条使用
	bool is_pause_ = false;  // 是否暂停
protected:
	std::mutex mutex_;
	OwlDemux* demux_ = nullptr;
	OwlVideoThread* video_thread_ = nullptr;
	OwlAudioThread* audio_thread_ = nullptr;
};

