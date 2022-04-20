#include "OwlVideoThread.h"
#include <iostream>
#include "OwlDecode.h"
using namespace std;
bool OwlVideoThread::Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height)
{
	if (!para)  return false;
	mutex_.lock();

	// 初始化显示窗口
	video_call_ = video_call;  // 传递回调函数
	if (video_call_) {
		video_call_->Init(width, height);
	}
	// 打开解码器
	if (!decode_)  decode_ = new OwlDecode();
	int re = true;
	// decode_的Open用完后会释放para，放在最后面释放
	if (!decode_->Open(para)) {
		cout << "video OwlDecode open failed!" << endl;
		re = false;
	}
	mutex_.unlock();
	cout << "OwlVideoThread::Open：" << re << endl;

	return re;
}

void OwlVideoThread::Push(AVPacket* pkt)
{
	if (!pkt)  return;

	// 阻塞，数据不能丢
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

		// 没有数据，或者decode_没有初始化好
		if (packets_.empty() || !decode_) {
			mutex_.unlock();
			msleep(1);
			continue;
		}

		// 音视频同步
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
		// 一次Send，多次Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			if (!frame)  break;
			// 显示视频
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
	// 等待线程退出，不然线程还在运行，但其他空间已经释放了
	is_exit_ = true;
	wait();
}
