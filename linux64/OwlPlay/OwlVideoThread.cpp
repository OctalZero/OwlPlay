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
	// 初始化显示窗口
	video_call_ = video_call;  // 传递回调函数
	if (video_call_) {
		video_call_->Init(width, height);
	}
	video_mutex_.unlock();

	int re = true;
	// decode_的Open用完后会释放para，放在最后面释放
	if (!decode_->Open(para)) {
		cout << "video OwlDecode open failed!" << endl;
		re = false;
	}

	cout << "OwlVideoThread::Open：" << re << endl;

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

		// 处理暂停
		if (is_pause_) {
			video_mutex_.unlock();
			msleep(5);
			continue;
		}

		// 音视频同步, 不存在音频时不处理
		if (syn_pts_ > 0 && syn_pts_ < decode_->pts_) {
			video_mutex_.unlock();
			msleep(1);
			continue;
		}

		AVPacket* packet = Pop();

		bool re = decode_->Send(packet);
		if (!re) {
			video_mutex_.unlock();
			msleep(1);
			continue;
		}
		// 一次Send，多次Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			//cout << "video pts = " << decode_->pts_ << endl;
			if (!frame)  break;
			// 显示视频
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

	bool re = decode_->Send(pkt);
	if (!re) {
		video_mutex_.unlock();
		return true;  // 表示结束解码
	}
	AVFrame* frame = decode_->Receive();
	if (!frame) {
		video_mutex_.unlock();
		return false;  // 没读到，继续处理下一帧
	}
	// 到达位置
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
