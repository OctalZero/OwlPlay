#include "OwlDecodeThread.h"
#include "OwlDecode.h"

void OwlDecodeThread::Push(AVPacket* pkt)
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

void OwlDecodeThread::Clear()
{
	mutex_.lock();
	decode_->Clear();
	// ���������
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
	// �ȴ��߳��˳�
	is_exit_ = true;
	wait();
	// �߳��˳���������decode_�����߳��п��ܷ��ʵ�decode_
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
	// �򿪽�����
	if (!decode_)  decode_ = new OwlDecode();
}

OwlDecodeThread::~OwlDecodeThread()
{
	// �ȴ��߳��˳�����Ȼ�̻߳������У��������ռ��Ѿ��ͷ���
	is_exit_ = true;
	wait();
}
