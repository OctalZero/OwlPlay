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


void OwlVideoThread::run()
{
	while (!is_exit_) {
		video_mutex_.lock();

		// ����Ƶͬ��, ��������Ƶʱ������
		if (syn_pts_ > 0 && syn_pts_ < decode_->pts_) {
			video_mutex_.unlock();
			msleep(1);
			continue;
		}

		AVPacket* packet = Pop();
		//// û�����ݣ�����decode_û�г�ʼ����
		//if (packets_.empty() || !decode_) {
		//	video_mutex_.unlock();
		//	msleep(1);
		//	continue;
		//}

		//AVPacket* packet = packets_.front();
		//packets_.pop_front();

		bool re = decode_->Send(packet);
		if (!re) {
			video_mutex_.unlock();
			msleep(1);
			continue;
		}
		// һ��Send�����Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			if (!frame)  break;
			// ��ʾ��Ƶ
			if (video_call_) {
				video_call_->Repaint(frame);
			}

		}
		video_mutex_.unlock();
	}
}

OwlVideoThread::OwlVideoThread()
{
}

OwlVideoThread::~OwlVideoThread()
{
}
