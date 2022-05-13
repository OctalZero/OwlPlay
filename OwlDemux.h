/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ���װ���࣬
  *				 ��������Ҫ�� FFmpeg ���װ������ص����ݷ����˴���,
				 ����������� FFmpeg ��ϡ���
**********************************************************************************/
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

	// ���װ����ȡAVPacket���ռ���Ҫ�������ͷţ��ͷ�ACPacket��������ݿռ� av_packet_free
	virtual AVPacket* Read();

	// ֻ����Ƶ����Ƶ�������ռ��ͷ�
	virtual AVPacket* ReadVideo();

	// �ж��Ƿ�Ϊ��Ƶ
	virtual bool isAudio(AVPacket* pkt);

	// �ж��Ƿ�Ϊ��Ƶ
	virtual bool isVideo(AVPacket* pkt);

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
public:
	int total_ms_ = 0;  // ý����ʱ�������룩
	int width_ = 0;  // ���ʿ�
	int height_ = 0;  // ���ʸ�
	int sample_rate_ = 0;  // ��Ƶ������
	int channels_ = 0;  // ��Ƶͨ����
protected:
	std::mutex mutex_;
	AVFormatContext* ic = NULL;  // ���װ������
	int video_stream_ = 0;  // ��Ƶ��������ȡʱ��������Ƶ
	int audio_stream_ = 1;  // ��Ƶ��������ȡʱ��������Ƶ
};

