#pragma once
// 播放音频的类
class OwlAudioPlay
{
public:
	// 打开音频播放，定义为纯虚函数，在子类中实现
	virtual bool Open() = 0;

	// 关闭音频
	virtual void Close() = 0;

	// 返回缓冲中还没有播放的时间（毫秒ms） 
	virtual long long GetNoPlayMs() = 0;

	// 播放音频
	virtual bool Write(const unsigned char* data_, int datasize_) = 0;

	// 判断音频缓冲区是否有足够空间来写
	virtual int GetFree() = 0;

	// 工厂，获取具体的音频播放器，可换成不同的接口
	static OwlAudioPlay* GetAudioPlay();

	OwlAudioPlay();
	virtual ~OwlAudioPlay();

public:
	int sample_rate_ = 44100;
	int sample_size_ = 16;  // 位数，不是字节数，是固定的，重采样后统一为16
	int channels_ = 2;
};

