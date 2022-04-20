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


void OwlVideoThread::run()
{
	while (!is_exit_) {
		video_mutex_.lock();

		// 音视频同步, 不存在音频时不处理
		if (syn_pts_ > 0 && syn_pts_ < decode_->pts_) {
			video_mutex_.unlock();
			msleep(1);
			continue;
		}

		AVPacket* packet = Pop();
		//// 没有数据，或者decode_没有初始化好
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
		// 一次Send，多次Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			if (!frame)  break;
			// 显示视频
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
