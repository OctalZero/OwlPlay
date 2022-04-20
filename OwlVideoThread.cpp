#include "OwlVideoThread.h"
#include <iostream>
#include "OwlDecode.h"
using namespace std;
bool OwlVideoThread::Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height)
{
	if (!para)  return false;
	mutex_.lock();

	// ��ʼ����ʾ����
	video_call_ = video_call;  // ���ݻص�����
	if (video_call_) {
		video_call_->Init(width, height);
	}
	// �򿪽�����
	if (!decode_)  decode_ = new OwlDecode();
	int re = true;
	// decode_��Open�������ͷ�para������������ͷ�
	if (!decode_->Open(para)) {
		cout << "video OwlDecode open failed!" << endl;
		re = false;
	}
	mutex_.unlock();
	cout << "OwlVideoThread::Open��" << re << endl;

	return re;
}

void OwlVideoThread::Push(AVPacket* pkt)
{
	if (!pkt)  return;

	// ���������ݲ��ܶ�
	while (!is_exit_) {
		mutex_.lock();
		if (packets_.size() < max_list_) {
			packets_.push_back(pkt);
			mutex_.unlock();
			break;
		}
		mutex_.unlock();
		msleep(1);
	}
}

void OwlVideoThread::run()
{
	while (!is_exit_) {
		mutex_.lock();

		// û�����ݣ�����decode_û�г�ʼ����
		if (packets_.empty() || !decode_) {
			mutex_.unlock();
			msleep(1);
			continue;
		}

		// ����Ƶͬ��
		if (syn_pts_ < decode_->pts_) {
			mutex_.unlock();
			msleep(1);
			continue;
		}

		AVPacket* packet = packets_.front();
		packets_.pop_front();

		bool re = decode_->Send(packet);
		if (!re) {
			mutex_.unlock();
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
		mutex_.unlock();
	}
}

OwlVideoThread::OwlVideoThread()
{
}

OwlVideoThread::~OwlVideoThread()
{
	// �ȴ��߳��˳�����Ȼ�̻߳������У��������ռ��Ѿ��ͷ���
	is_exit_ = true;
	wait();
}
