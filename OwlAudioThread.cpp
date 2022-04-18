#include "OwlAudioThread.h"
#include <iostream>
#include "OwlDecode.h"
#include "OwlAudioPlay.h"
#include "OwlResample.h"
using namespace std;

bool OwlAudioThread::Open(AVCodecParameters* para, int sample_rate, int channels)
{
	if (!para)  return false;
	mutex_.lock();
	if (!decode_)  decode_ = new OwlDecode();
	if (!resample_)  resample_ = new OwlResample();
	if (!audio_play_) audio_play_ = OwlAudioPlay::GetAudioPlay();

	bool re = true;  // 不在异常情况下直接返回，防止para没有释放
	if (!resample_->Open(para, false)) {
		cout << "OwlResample open failed!" << endl;
		re = false;
		//mutex_.unlock();
		//return false;
	}

	audio_play_->sample_rate_ = sample_rate;
	audio_play_->channels_ = channels;

	if (!audio_play_->Open()) {
		re = false;
		cout << "OwlAudioPlay open failed!" << endl;
		//mutex_.unlock();
		//return false;
	}
	// decode_的Open用完后会释放para，放在最后面释放
	if (!decode_->Open(para)) {
		cout << "audio OwlDecode open failed!" << endl;
		re = false;
	}
	mutex_.unlock();
	cout << "OwlAudioThread::Open：" << re << endl;

	return re;
}

void OwlAudioThread::Push(AVPacket* pkt)
{
	if (!pkt)  return;

	// 阻塞，数据不能丢
	while (!is_exit_) {
		mutex_.lock();
		if (packets_.size() < maxList_) {
			packets_.push_back(pkt);
			mutex_.unlock();
			break;
		}
		mutex_.unlock();
		msleep(1);
	}

}

void OwlAudioThread::run()
{
	// 重采样后音频输出数据，分配10M存放
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];

	while (!is_exit_) {
		mutex_.lock();
		// 没有数据，或者decode_、resample_、audio_play_没有初始化号
		if (packets_.empty() || !decode_ || !resample_ || !audio_play_) {
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
			// 重采样
			int size = resample_->Resample(frame, pcm);
			// 开始播放音频
			while (!is_exit_) {
				if (size <= 0)  break;
				// 缓冲未播完，空间不够
				if (audio_play_->GetFree() < size) {
					msleep(1);
					continue;
				}
				audio_play_->Write(pcm, size);
				break;
			}
		}
		mutex_.unlock();
	}
	delete[] pcm;
}

OwlAudioThread::OwlAudioThread()
{
}

OwlAudioThread::~OwlAudioThread()
{
	// 等待线程退出，不然线程还在运行，但其他空间已经释放了
	is_exit_ = true;
	wait();
}
