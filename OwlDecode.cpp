#include "OwlDecode.h"
#include <iostream>
using namespace std;
extern "C" {
#include <libavcodec/avcodec.h>
}

void OwlFreePacket(AVPacket** pkt)
{
	if (!pkt || !(*pkt))  return;  // �ݴ�
	av_packet_free(pkt);
}

void OwlFreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame))  return;  // �ݴ�
	av_frame_free(frame);
}

bool OwlDecode::Open(AVCodecParameters* para)
{
	if (!para)  return false;
	Close();

	// �������򿪣��ҵ�������
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		avcodec_parameters_free(&para);
		cout << "can't find the codec id " << para->codec_id << endl;
		return false;
	}
	cout << "find the AVCodec " << para->codec_id << endl;

	decode_mutex_.lock();
	// ����������������
	codec_context_ = avcodec_alloc_context3(codec);

	// ���ý����������Ĳ���
	avcodec_parameters_to_context(codec_context_, para);
	avcodec_parameters_free(&para);  // �����ʱ�ͷŲ���ָ��

	// ���߳̽���
	codec_context_->thread_count = 8;

	// �򿪽�����������
	int re = avcodec_open2(codec_context_, 0, 0);
	if (re != 0)
	{
		avcodec_free_context(&codec_context_);
		decode_mutex_.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return false;
	}
	decode_mutex_.unlock();
	cout << "avcodec_open2 success!" << endl;

	return true;
}

bool OwlDecode::Send(AVPacket* pkt)
{
	// �ݴ���
	if (!pkt || pkt->size <= 0 || !pkt->data)  return false;
	decode_mutex_.lock();
	if (!codec_context_) {
		decode_mutex_.unlock();
		return false;
	}
	int re = avcodec_send_packet(codec_context_, pkt);
	decode_mutex_.unlock();
	av_packet_free(&pkt);  // ֻ���� av_packet_alloc ����Ĳ��������ͷ�
	if (re != 0) return false;

	return true;
}

AVFrame* OwlDecode::Receive()
{
	decode_mutex_.lock();
	if (!codec_context_) {
		decode_mutex_.unlock();
		return nullptr;
	}
	AVFrame* frame = av_frame_alloc();
	int re = avcodec_receive_frame(codec_context_, frame);
	decode_mutex_.unlock();
	if (re != 0) {
		av_frame_free(&frame);
		return nullptr;
	}
	//cout << "[" << frame->linesize[0] << "]" << " " << flush;
	pts_ = frame->pts;
	return frame;
}

void OwlDecode::Close()
{
	decode_mutex_.lock();
	if (codec_context_) {
		//avcodec_close(codec_context_);  // �ú���ֻ��յ����ݣ�δ��ָ���ÿ�
		avcodec_free_context(&codec_context_);
	}
	pts_ = 0;
	decode_mutex_.unlock();
}

void OwlDecode::Clear()
{
	decode_mutex_.lock();
	// ������뻺��
	if (codec_context_) {
		avcodec_flush_buffers(codec_context_);
	}
	decode_mutex_.unlock();
}

OwlDecode::OwlDecode()
{
}

OwlDecode::~OwlDecode()
{
}