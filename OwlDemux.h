#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class OwlDemux
{
public:
	// ��ý���ļ���������ý�� rtmp\http\rtsp
	virtual bool Open(const char* url);

	// �ռ���Ҫ�������ͷţ��ͷ�ACPacket��������ݿռ� av_packet_free
	virtual AVPacket* Read();

	// �ж��Ƿ�Ϊ��Ƶ
	virtual bool isAudio(AVPacket* pkt);

	// ��ȡ��Ƶ���������ؿռ���Ҫ���� avcodec_parameters_free
	virtual AVCodecParameters* CopyVideoPara();

	// ��ȡ��Ƶ���������ؿռ���Ҫ���� avcodec_parameters_free
	virtual AVCodecParameters* CopyAudioPara();

	// seek λ�� pos 0.0 ~ 1.0
	virtual bool Seek(double pos);

	// ��ն�ȡ����
	virtual void Clear();

	// �ر�
	virtual void Close();

	OwlDemux();
	virtual ~OwlDemux();

	// ý����ʱ�������룩
	int total_ms_ = 0;
protected:
	std::mutex mutex_;
	// ���װ������
	AVFormatContext* ic = NULL;
	// ����Ƶ��������ȡʱ��������Ƶ
	int video_stream_ = 0;
	int audio_stream_ = 1;
};

