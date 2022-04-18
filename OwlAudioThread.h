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
	// 打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, int sample_rate, int channels);

	// 放入packet到缓冲队列
	virtual void Push(AVPacket* pkt);

	// 从缓冲队列取出packet
	void run() override;

	OwlAudioThread();
	virtual ~OwlAudioThread();
public:
	// 最大队列，缓冲约2s
	int maxList_ = 100;
	// 判断线程是否退出
	bool is_exit_ = false;
protected:
	std::mutex mutex_;
	// 生产者消费者模式，由调用者生产packet扔到该生产队列中，再在线程中消费packet。
	std::list<AVPacket*> packets_;
	OwlDecode* decode_ = nullptr;  // 解码器
	OwlAudioPlay* audio_play_ = nullptr;  // 音频播放
	OwlResample* resample_ = nullptr;  // 重采样 
};

