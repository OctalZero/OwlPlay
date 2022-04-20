#pragma once
#include "IVideoCall.h"
#include "OwlDecodeThread.h"
struct AVCodecParameters;

// 解码和显示视频的类
class OwlVideoThread : public OwlDecodeThread
{
public:
	// 打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height);

	// 从缓冲队列取出packet
	void run() override;

	OwlVideoThread();
	virtual ~OwlVideoThread();
public:
	// 同步时间，由外部传入
	long long syn_pts_ = 0;
protected:
	std::mutex video_mutex_;
	IVideoCall* video_call_ = nullptr;  // 显示视频图像的接口
};

