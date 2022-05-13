#include "OwlVideoThread.h"
#include <iostream>
#include "OwlDecode.h"
using namespace std;
bool OwlVideoThread::Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height)
{
	if (!para)  return false;
	Clear();

	video_mutex_.lock();
	syn_pts_ = 0;
	// ��ʼ����ʾ����
	video_call_ = video_call;  // ���ݻص�����
	if (video_call_) {
		video_call_->Init(width, height);
	}
	video_mutex_.unlock();

	int re = true;
	// decode_��Open�������ͷ�para������������ͷ�
	if (!decode_->Open(para)) {
		cout << "video OwlDecode open failed!" << endl;
		re = false;
	}

	cout << "OwlVideoThread::Open��" << re << endl;

	return re;
}


void OwlVideoThread::SetPause(bool is_pause)
{
	video_mutex_.lock();
	is_pause_ = is_pause;
	video_mutex_.unlock();
}

void OwlVideoThread::run()
{
	while (!is_exit_) {
		video_mutex_.lock();

		// ������ͣ
		if (is_pause_) {
			video_mutex_.unlock();
			msleep(5);
			continue;
		}

		// ����Ƶͬ��, ��������Ƶʱ������
		if (syn_pts_ > 0 && decode_->pts_ - syn_pts_ > 90
			&& abs(decode_->pts_ - syn_pts_) < 1000) {  // ȷ����Ƶ�ͺ�90ms���ڣ����������󲻽�������
			cout << "����Ƶͬ������" << endl;
			video_mutex_.unlock();
			msleep(1);
			continue;
		}


		if (is_flush_) {
			FlushDecodeBuffer();
		}

		AVPacket* packet = Pop();

		bool re = decode_->SendPacket(packet);
		if (!re) {
			cout << "��Ƶ��������" << endl;
			video_mutex_.unlock();
			msleep(1);
			continue;
		}
		// һ��Send�����Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->ReceiveFrame();
			// ֹͣˢ�� Decode �����������¶�ȡ����״̬
			if (decode_->eof_) {
				is_flush_ = false;
				read_state_ = 3;
			}
			if (!frame)  break;
			cout << "video pts = " << decode_->pts_ << endl;

			// ��ʾ��Ƶ
			if (video_call_) {
				video_call_->Repaint(frame);
			}

		}
		video_mutex_.unlock();
	}
}

bool OwlVideoThread::ReaintPts(AVPacket* pkt, long long seek_pts)
{
	video_mutex_.lock();

	bool re = decode_->SendPacket(pkt);
	if (!re) {
		video_mutex_.unlock();
		return true;  // ��ʾ��������
	}
	AVFrame* frame = decode_->ReceiveFrame();
	if (!frame) {
		video_mutex_.unlock();
		return false;  // û����������������һ֡
	}
	// ����λ��
	if (decode_->pts_ >= seek_pts) {
		if (video_call_) {
			video_call_->Repaint(frame);
		}
		video_mutex_.unlock();
		return true;
	}
	OwlFreeFrame(&frame);

	video_mutex_.unlock();
	return false;
}

OwlVideoThread::OwlVideoThread()
{
}

OwlVideoThread::~OwlVideoThread()
{
}
