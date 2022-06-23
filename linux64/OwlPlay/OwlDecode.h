/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ������࣬
				 ��������Ҫ�� FFmpeg ���벿����ص����ݷ����˴���,
				 ����������� FFmpeg ��ϡ�
**********************************************************************************/
#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
extern void OwlFreePacket(AVPacket** pkt);  // ��¶�ͷź�����OwlDecodeThread����������FFmpeg��
extern void OwlFreeFrame(AVFrame** frame);  // ��¶�ͷź�����OwlDecodeThread����������FFmpeg��

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
public:
	long long pts_ = 0;  // ��ǰ���뵽��pts
protected:
	AVCodecContext* codec_context_ = nullptr;
	std::mutex mutex_;
};

