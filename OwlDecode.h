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
	virtual bool SendPacket(AVPacket* pkt);

	// ��ȡ�������ݣ�һ��send������Ҫ���Receive������ȡ�����е�����Send NULL��Receive���
	// ÿ�θ���һ�ݣ��ɵ������ͷ� av_frame_free
	virtual AVFrame* ReceiveFrame();

	// ����һ���յ� AVPacket
	void AllocEmptyPacket(AVPacket* pkt);
	// �ͷ�һ���յ� AVPacket
	void FreeEmptyPacket(AVPacket* pkt);

	// �ر�
	virtual void Close();

	// ������
	virtual void Clear();

	OwlDecode();
	~OwlDecode();
public:
	long long pts_ = 0;  // ��ǰ���뵽��pts
	bool eof_ = false;  // ����ȡ�����ı��
	// ��ȡ����״̬��0->��δ��ȡ��; 1->��ȡ����; 2->��ȡ������; 3->ˢ���� Decode ������
	int read_state_ = 0;
protected:
	AVCodecContext* codec_context_ = nullptr; // ����������
	std::mutex mutex_;
};

