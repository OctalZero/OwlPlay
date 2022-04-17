#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
class OwlDecode
{
public:
	// �򿪽�����,���ܳɹ�����ͷ�para�ռ�
	virtual bool Open(AVCodecParameters* para);

	// ���͵������̣߳����ܳɹ�����ͷ�pkt�ռ䣨�����ý�����ݣ�
	virtual bool Send(AVPacket* pkt);

	// ��ȡ�������ݣ�һ��send������Ҫ���Receive������ȡ�����е�����Send NULL��Receive���
	// ÿ�θ���һ�ݣ��ɵ������ͷ� av_frame_free
	virtual AVFrame* Receive();



	// �ر�
	virtual void Close();
	// ������
	virtual void Clear();

	OwlDecode();
	~OwlDecode();
protected:
	AVCodecContext* codec_context_ = 0;
	std::mutex mutex_;
};

