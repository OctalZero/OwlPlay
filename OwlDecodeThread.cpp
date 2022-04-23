#include "OwlDecodeThread.h"
#include "OwlDecode.h"

void OwlDecodeThread::Push(AVPacket* pkt)
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

void OwlDecodeThread::Clear()
{
	mutex_.lock();
	decode_->Clear();
	// 清理缓冲队列
	while (!packets_.empty()) {
		AVPacket* packet = packets_.front();
		OwlFreePacket(&packet);
		packets_.pop_front();
	}
	mutex_.unlock();
}

void OwlDecodeThread::Close()
{
	Clear();
	// 等待线程退出
	is_exit_ = true;
	wait();
	// 线程退出后再清理decode_，在线程中可能访问到decode_
	decode_->Close();
	mutex_.lock();
	delete decode_;
	decode_ = nullptr;
	mutex_.unlock();
}

AVPacket* OwlDecodeThread::Pop()
{
	mutex_.lock();
	if (packets_.empty()) {
		mutex_.unlock();
		return nullptr;
	}
	AVPacket* packet = packets_.front();
	packets_.pop_front();
	mutex_.unlock();

	return packet;
}

OwlDecodeThread::OwlDecodeThread()
{
	// 打开解码器
	if (!decode_)  decode_ = new OwlDecode();
}

OwlDecodeThread::~OwlDecodeThread()
{
	// 等待线程退出，不然线程还在运行，但其他空间已经释放了
	is_exit_ = true;
	wait();
}
