#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class OwlDemux
{
public:
	// 打开媒体文件，或者流媒体 rtmp\http\rtsp
	virtual bool Open(const char* url);

	// 空间需要调用者释放，释放ACPacket对象和数据空间 av_packet_free
	virtual AVPacket* Read();

	// 判断是否为音频
	virtual bool isAudio(AVPacket* pkt);

	// 获取视频参数，返回空间需要清理 avcodec_parameters_free
	virtual AVCodecParameters* CopyVideoPara();

	// 获取音频参数，返回空间需要清理 avcodec_parameters_free
	virtual AVCodecParameters* CopyAudioPara();

	// seek 位置 pos 0.0 ~ 1.0
	virtual bool Seek(double pos);

	// 清空读取缓存
	virtual void Clear();

	// 关闭
	virtual void Close();

	OwlDemux();
	virtual ~OwlDemux();

	// 媒体总时长（毫秒）
	int total_ms_ = 0;
protected:
	std::mutex mutex_;
	// 解封装上下文
	AVFormatContext* ic = NULL;
	// 音视频索引，读取时区分音视频
	int video_stream_ = 0;
	int audio_stream_ = 1;
};

