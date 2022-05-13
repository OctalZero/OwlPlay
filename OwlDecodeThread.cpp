#include "OwlDecodeThread.h"
#include "OwlDecode.h"

#include <iostream>
using namespace std;

void OwlDecodeThread::Push(AVPacket* pkt)
{
	if (read_state_ != 2 && !pkt)  return;

	// ���������ݲ��ܶ�
	while (!is_exit_) {
		mutex_.lock();
		if (packets_.size() < max_list_) {
			packets_.push_back(pkt);
			mutex_.unlock();
			break;
		}
		cout << "Push����" << endl;
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
	cout << "Pop" << endl;

	return packet;
}

void OwlDecodeThread::FlushDecodeBuffer()
{
	decode_->read_state_ = read_state_;
	AVPacket* empty_pkt = nullptr;
	decode_->AllocEmptyPacket(empty_pkt);
	if (is_flush_) {
		Push(empty_pkt);
	}
	else {
		decode_->FreeEmptyPacket(empty_pkt);
	}
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
