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

	// 设置暂停
	void SetPause(bool is_pause);

	// 从缓冲队列取出packet
	void run() override;

	// 解码pts，如果接收到的解码数据pts >= seek_pts，就return true，并且显示画面
	virtual bool ReaintPts(AVPacket* pkt, long long seek_pts);

	OwlVideoThread();
	virtual ~OwlVideoThread();
public:
	long long syn_pts_ = 0;  // 同步时间，由外部传入
	bool is_pause_ = false;  // 是否暂停
protected:
	std::mutex video_mutex_;
	IVideoCall* video_call_ = nullptr;  // 显示视频图像的接口
};

